import { Camera } from "./camera.js";
import { mat4 } from "gl-matrix";
import Material from "./webgpu/Material.js";
import Logger from "./Logger.js";
import Mesh from "./Mesh.js";
const logger = Logger.getDefaultLogger();

export class Renderer {

    /** @type{HTMLCanvasElement} */
    #canvas;

    /** @type {GPUAdapter} */
    #adapter;

    /** @type {GPUDevice} */
    #device;

    /** @type {GPUCanvasContext} */
    #context;

    /** @type {GPUTextureFormat} */
    #format;

    /** @type {GPUBuffer} */
    #uniformBuffer;

    /** @type {GPUBindGroup} */
    #bindGroup;

    /** @type {GPURenderPipeline} */
    #pipeline;

    /** @type {Mesh} */
    #mesh;

    /** @type {Material} */
    #material;

    /** @type {GPUBuffer} */
    #objectBuffer;

    /** @type {GPURenderPassDepthStencilAttachment} */
    #depthStencilAttachment;

    /** @type {GPUTexture} */
    #depthStencilBuffer;

    /** @type {GPUTextureView} */
    #depthStencilView;

    /** * @type {GPUDepthStencilState} */
    #depthStencilState;

    /**
     *
     * @param {HTMLCanvasElement}canvas
     */
    constructor(canvas) {
        this.#canvas = canvas;
    }

    start() {

    }

    stop() {

    }

    async initialize() {
        logger.debug("renderer init");
        logger.debug("setting up device");
        await this.#setupDevice();
        logger.debug("creating assets");
        await this.#createAssets();
        logger.debug("making pipeline");
        await this.#makeDepthBufferResources();
        await this.#makePipeline();
        logger.debug("renderer init done");
    }
    async #makeDepthBufferResources() {
        this.#depthStencilState = {
            format: "depth24plus-stencil8",
            depthWriteEnabled: true,
            depthCompare: "less-equal",
        };

        this.#depthStencilBuffer = this.#device.createTexture({
            size: {
                width: this.#canvas.width,
                height: this.#canvas.height,
                depthOrArrayLayers: 1
            },
            format: "depth24plus-stencil8",
            usage: GPUTextureUsage.RENDER_ATTACHMENT,
        });

        this.#depthStencilView = this.#depthStencilBuffer.createView({
            format: "depth24plus-stencil8",
            dimension: "2d",
            aspect: "all"
        });
        this.#depthStencilAttachment = {
            view: this.#depthStencilView,
            depthClearValue: 1.0,
            depthLoadOp: "clear",
            depthStoreOp: "store",
            stencilLoadOp: "clear",
            stencilStoreOp: "discard"

        }

    }
    async #setupDevice() {
        logger.debug("setup device");
        this.#adapter = await navigator.gpu?.requestAdapter();
        this.#device = await this.#adapter?.requestDevice({ "label": "macOS Device" });
        this.#context = this.#canvas.getContext("webgpu");
        this.#format = "bgra8unorm";
        this.#context.configure({
            device: this.#device,
            format: this.#format,
            alphaMode: "opaque"
        });
        logger.debug("exit setup device");
    }

    async #makePipeline() {

        this.#uniformBuffer = this.#device.createBuffer({
            size: 64 * 2,
            usage: GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST
        });

        const bindGroupLayout = this.#device.createBindGroupLayout({
            entries: [
                {
                    binding: 0,
                    visibility: GPUShaderStage.VERTEX,
                    buffer: {}
                },
                {
                    binding: 1,
                    visibility: GPUShaderStage.FRAGMENT,
                    texture: {}
                },
                {
                    binding: 2,
                    visibility: GPUShaderStage.FRAGMENT,
                    sampler: {}
                },
                {
                    binding: 3,
                    visibility: GPUShaderStage.VERTEX,
                    buffer: {
                        type: "read-only-storage",
                        hasDynamicOffset: false
                    }
                },
            ]

        });

        this.#bindGroup = this.#device.createBindGroup({
            layout: bindGroupLayout,
            entries: [
                {
                    binding: 0,
                    resource: {
                        buffer: this.#uniformBuffer
                    }
                },
                {
                    binding: 1,
                    resource: this.#material.view
                },
                {
                    binding: 2,
                    resource: this.#material.sampler
                },
                {
                    binding: 3,
                    resource: { buffer: this.#objectBuffer }
                }
            ]
        });

        //TODO
        const response = await fetch("shader.wgsl");
        if (!response.ok) {
            throw new Error(`Failed to fetch shader. Status: ${response.status}`);
        }
        const shader = await response.text();

        const pipelineLayout = this.#device.createPipelineLayout({
            bindGroupLayouts: [bindGroupLayout]
        });

        this.#pipeline = this.#device.createRenderPipeline({
            vertex: {
                module: this.#device.createShaderModule({
                    code: shader
                }),
                entryPoint: "vs_main",
                buffers: [this.#mesh.bufferLayout,]
            },

            fragment: {
                module: this.#device.createShaderModule({
                    code: shader
                }),
                entryPoint: "fs_main",
                targets: [{
                    format: this.#format
                }]
            },

            primitive: {
                topology: "triangle-list"
            },

            layout: pipelineLayout,
            depthStencil: this.#depthStencilState
        });

    }

    async #createAssets() {
        this.#mesh = new Mesh(this.#device);
        this.#material = await Material.buildFromUrl(this.#device, "test.png");
        this.#objectBuffer = this.#device.createBuffer({
            size: 64 * 1024,
            usage: GPUBufferUsage.STORAGE | GPUBufferUsage.COPY_DST
        });
    }

    /**
     *
     * @param {Camera}camera
     * @param {Float32Array}objectData
     * @param {number}count
     * @returns {Promise<void>}
     */
    async render(camera, objectData, count) {
        //make transforms
        const projection = mat4.create();
        mat4.perspective(projection, Math.PI / 4, 800 / 600, 0.1, 10);
        this.#device.queue.writeBuffer(this.#objectBuffer, 0, objectData, 0, objectData.length);
        this.#device.queue.writeBuffer(this.#uniformBuffer, 0, camera.view);
        this.#device.queue.writeBuffer(this.#uniformBuffer, 64, projection);

        const commandEncoder = this.#device.createCommandEncoder();
        const textureView = this.#context.getCurrentTexture().createView();
        const renderpass = commandEncoder.beginRenderPass({
            colorAttachments: [{
                view: textureView,
                clearValue: { r: 0.6, g: 0.0, b: 0.25, a: 1.0 },
                loadOp: "clear",
                storeOp: "store"
            }],
            depthStencilAttachment: this.#depthStencilAttachment
        });

        renderpass.setPipeline(this.#pipeline);
        renderpass.setVertexBuffer(0, this.#mesh.buffer);
        renderpass.setBindGroup(0, this.#bindGroup);
        renderpass.draw(3, count, 0, 0);
        renderpass.end();
        this.#device.queue.submit([commandEncoder.finish()]);
    }
}