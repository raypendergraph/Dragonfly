import { Mesh } from "./Mesh.js";
import { Camera } from "./camera.js";
import { vec3, mat4 } from "gl-matrix";
import { Transform } from "./transform.js";

export class Scene {

    /** @type {Transform[]} */
    #transforms;
    /** @type {Camera} */
    #camera;

    /** @type {Float32Array} */
    #objectData;

    /** @type {Number} */
    #objectCount;
    constructor() {
        this.#transforms = [];
        this.#objectData = new Float32Array(16 * 1024);
        for (let y = -5, i = 0; y < 5; y++, i++) {
            this.#transforms.push(
                new Transform([2, y, 0], 0)
            );
            const blank = mat4.create();
            for (let j = 0; j < 16; j++) {
                this.#objectData[16 * i + j] = blank.at(j);
            }
            this.#objectCount = i+1;
        }
        this.#camera = new Camera(
            [-2, 0, 0.5], 0, 0
        );

    }

    update() {
        for (let i = 0; i < this.#transforms.length; i++) {
            const t = this.#transforms[i];
            t.update();
            for (let j = 0; j < 16; j++) {
                this.#objectData[16 * i + j] = t.model.at(j);
            }
        }

        this.#camera.update();
    }

    /**
     *
     * @returns {Camera}
     */
    getCamera() {
        return this.#camera;
    }

    /**
     * @returns {Transform[]}
     */
    get transforms() {
        return this.#transforms;
    }

    get objectData(){
        return this.#objectData;
    }
    get objectCount(){
        return this.#objectCount;
    }
    /**
     *
     * @param {number}dX
     * @param {number}dY
     */
    spinCamera(dX, dY) {
        this.#camera.eulers[2] -= dX;
        this.#camera.eulers[2] %= 360;

        this.#camera.eulers[1] = Math.min(
            89, Math.max(
                -89,
                this.#camera.eulers[1] + dY
            )
        );
    }

    /**
     *
     * @param {number}forwardAmount
     * @param {number}rightAmount
     */
    movePlayer(forwardAmount, rightAmount) {
        vec3.scaleAndAdd(
            this.#camera.position, this.#camera.position,
            this.#camera.forward, forwardAmount
        );

        vec3.scaleAndAdd(
            this.#camera.position, this.#camera.position,
            this.#camera.right, rightAmount
        );
    }
}