#include "webgpu/webgpu.h"
#include <Platform/Core.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "RenderContext.h"
#include "Material.h"
#include "Scene.h"
#include "HandmadeMath.h"

AspectMaterial *
aspectMaterialNewFromFile(WGPUDevice device, char const *path, Error **err);

bool
aspectMaterialWriteToQueue(AspectRenderer *ctx, AspectMaterial *material, Error **err);

WGPURenderPipeline
createRenderPipeline(AspectRenderer *renderer, AspectRenderContext *ctx, Error **err)
{
   char *shaderSource = NULL;
   WGPUPipelineLayout pipelineLayout = NULL;
   WGPUShaderModule shaderModule = NULL;
   WGPURenderPipeline pipeline = NULL;

   shaderSource = pfmLoadContentAtPathAsString(
      "/Users/rpendergraph/CLionProjects/learning/Resources/shader.wgsl", err);

   if (shaderSource == NULL)
   {
      REPORT_NULL_FAULT(shaderSource, err);
      goto onFailure;
   }

   pipelineLayout = wgpuDeviceCreatePipelineLayout(
      renderer->device,
      &(WGPUPipelineLayoutDescriptor) {
         .label="Pipeline Layout",
         .bindGroupLayoutCount=1,
         .bindGroupLayouts=(WGPUBindGroupLayout[1]) {ctx->tempBindGroupLayout}});

   if (pipelineLayout == NULL)
   {
      REPORT_NULL_FAULT(pipelineLayout, err);
      goto onFailure;
   }

   shaderModule = wgpuDeviceCreateShaderModule(renderer->device, &(WGPUShaderModuleDescriptor) {
      .nextInChain=(const WGPUChainedStruct *) &(WGPUShaderModuleWGSLDescriptor) {
         .chain={
            .sType=WGPUSType_ShaderModuleWGSLDescriptor
         },
         .code=shaderSource
      },
      .label="Shader Module"
   });

//   WGPURenderPipelineDescriptor d = {
//      .label="renderPipeline",
//      .layout=pipelineLayout,
//      .depthStencil = &(WGPUDepthStencilState) { // Describes the optional depth-stencil properties, including the testing, operations, and bias.
//         .format = WGPUTextureFormat_Depth24Plus, // The special format of the depth "Texture".
//         .depthWriteEnabled=true, // Take action after a pixel has been identified.
//         .depthCompare=WGPUCompareFunction_Less, //How pixels are discriminated.
//         .stencilReadMask=0, // Logical bitwise filter of what to sample off.
//         .stencilWriteMask=0, // Logical bitwise filter of what to write back.
//         .depthBias=0, // Constant depth bias added to each fragment.
//         .depthBiasSlopeScale=0, // Depth bias that scales with the fragment’s slope.
//         .depthBiasClamp=0, // The maximum depth bias of a fragment.
//         .stencilFront=(WGPUStencilFaceState) { // describe how stencil comparisons and operations are performed
//            .compare = WGPUCompareFunction_Always, // The GPUCompareFunction used when testing fragments against depthStencilAttachment stencil values.
//            .failOp = WGPUStencilOperation_Keep, // The GPUStencilOperation performed if the fragment stencil comparison test described by compare fails.
//            .depthFailOp = WGPUStencilOperation_Keep, // The GPUStencilOperation performed if the fragment depth comparison described by depthCompare fails.
//            .passOp = WGPUStencilOperation_Keep, // The GPUStencilOperation performed if the fragment stencil comparison test described by compare passes.
//         },
//         .stencilBack=(WGPUStencilFaceState) {
//            .compare = WGPUCompareFunction_Always,
//            .failOp = WGPUStencilOperation_Keep,
//            .depthFailOp = WGPUStencilOperation_Keep,
//            .passOp = WGPUStencilOperation_Keep,
//         },
//      },
//      .multisample=(WGPUMultisampleState) { // describe how a GPURenderPipeline interacts with a render pass’s multisampled attachments
//         .count=1, // Number of samples per pixel.
//         .mask=~0u,
//      },
//      .vertex=(const WGPUVertexState) {
//         .module=shaderModule,
//         .entryPoint="vs_main",
//         .bufferCount=1,
//         .buffers=&(WGPUVertexBufferLayout) {
//            .arrayStride=6 * sizeof(float),
//            .stepMode=WGPUVertexStepMode_Vertex,
//            .attributeCount=2,
//            .attributes=(WGPUVertexAttribute[2]) {
//               {
//                  .shaderLocation=0,
//                  .format=WGPUVertexFormat_Float32x3,
//                  .offset=0
//               },
//               {
//                  .shaderLocation=1,
//                  .format=WGPUVertexFormat_Float32x3,
//                  .offset=3 * sizeof(float)
//               }
//            }}},
//      .fragment=&(const WGPUFragmentState) {
//         .entryPoint="fs_main",
//         .module=shaderModule,
//         .targetCount=1,
//         .targets=(const WGPUColorTargetState[1]) {
//            {
//               .format=WGPUTextureFormat_BGRA8Unorm,
//               .writeMask=WGPUColorWriteMask_All,
//               .blend = &(WGPUBlendState) {
//                  .color={
//                     .srcFactor=WGPUBlendFactor_SrcAlpha,
//                     .dstFactor=WGPUBlendFactor_OneMinusSrcAlpha,
//                     .operation=WGPUBlendOperation_Add},
//                  .alpha={
//                     .srcFactor=WGPUBlendFactor_Zero,
//                     .dstFactor=WGPUBlendFactor_One,
//                     .operation=WGPUBlendOperation_Add}}
//            }
//         }},
//      .primitive=(const WGPUPrimitiveState) {
//         .topology=WGPUPrimitiveTopology_TriangleList,
//         .stripIndexFormat=WGPUIndexFormat_Undefined,
//         .frontFace=WGPUFrontFace_CCW,
//         .cullMode=WGPUCullMode_None
//
//      }};

   WGPURenderPipelineDescriptor d = {
      .multisample=(WGPUMultisampleState) { // describe how a GPURenderPipeline interacts with a render pass’s multisampled attachments
         .count=1, // Number of samples per pixel.
         .mask=~0u,
      },
      .vertex=(const WGPUVertexState) {
         .module=shaderModule,
         .entryPoint="vs_main",
         .bufferCount=1,
         .buffers = (WGPUVertexBufferLayout[1]) {
            {
               .arrayStride=24,
               .attributeCount=2,
               .attributes=(WGPUVertexAttribute[2]) {
                  {
                     .shaderLocation=0,
                     .format= WGPUVertexFormat_Float32x3,
                     .offset=0},
                  {
                     .shaderLocation=1,
                     .format=WGPUVertexFormat_Float32x3,
                     .offset=12
                  }
               }
            }
         }
      },
      .fragment=&(WGPUFragmentState) {
         .module=shaderModule,
         .entryPoint="fs_main",
         .targetCount=1,
         .targets=(WGPUColorTargetState[1]) {
            {
//               .blend=&(WGPUBlendState) {
//                  .color=(WGPUBlendComponent) {
//                     .srcFactor=WGPUBlendFactor_SrcAlpha,
//                     .dstFactor=WGPUBlendFactor_OneMinusSrcAlpha,
//                     .operation=WGPUBlendOperation_Add
//                  },
//                  .alpha=(WGPUBlendComponent) {
//                     .srcFactor=WGPUBlendFactor_Zero,
//                     .dstFactor=WGPUBlendFactor_One,
//                     .operation=WGPUBlendOperation_Add
//                  }
//               },
               .format=WGPUTextureFormat_BGRA8Unorm,
               .writeMask=WGPUColorWriteMask_All}}},
      .primitive=(WGPUPrimitiveState) {
         .topology=WGPUPrimitiveTopology_TriangleList},
      .layout=pipelineLayout
   };
   pipeline = wgpuDeviceCreateRenderPipeline(renderer->device, &d);
   if (pipeline == NULL)
   {
      REPORT_NULL_FAULT(pipeline, err);
      goto onFailure;
   }
   free(shaderSource);
   return pipeline;

   onFailure:
   if (shaderSource) free(shaderSource);
   if (pipelineLayout) wgpuPipelineLayoutRelease(pipelineLayout);
   if (shaderModule) wgpuShaderModuleRelease(shaderModule);
   return NULL;
}

void
aspectRenderContextDestroy(AspectRenderContext *ctx)
{
   if (ctx == NULL)
   {
      return;
   }
   if (ctx->vertexBuffer)
   {
      wgpuBufferDestroy(ctx->vertexBuffer);
      wgpuBufferRelease(ctx->vertexBuffer);
   }
   if (ctx->indexBuffer)
   {
      wgpuBufferDestroy(ctx->indexBuffer);
      wgpuBufferRelease(ctx->indexBuffer);
   }
   if (ctx->uniformBuffer)
   {
      wgpuBufferDestroy(ctx->uniformBuffer);
      wgpuBufferRelease(ctx->uniformBuffer);
   }
//   if (ctx->geometryBindGroupLayout) wgpuBindGroupLayoutRelease(ctx->geometryBindGroupLayout);
//   if (ctx->geometryBindGroup) wgpuBindGroupRelease(ctx->geometryBindGroup);
//
//   if (ctx->materialsBindGroupLayout) wgpuBindGroupLayoutRelease(ctx->materialsBindGroupLayout);
//   if (ctx->materialsBindGroup) wgpuBindGroupRelease(ctx->materialsBindGroup);
   //TODO clear the depth stencil attachment
}


/*
 * Computes and sets scene geometry data into the context. You must clean up `ctx` if a failure is indicated.
 */
static bool
contextComputeSceneGeometry(AspectRenderContext *ctx, AspectScene *scene, AspectRenderer *renderer, Error **err)
{
   float vertices[18] = {
      0.0, 0.0, 0.5, 1.0, 0.0, 0.0,
      0.0, -0.5, -0.5, 0.0, 1.0, 0.0,
      0.0, 0.5, -0.5, 0.0, 0.0, 1.0
   };

   ctx->tempBindGroupLayout =
      wgpuDeviceCreateBindGroupLayout(renderer->device,
                                      &(WGPUBindGroupLayoutDescriptor) {
                                         .label="Bind Group Layout",
                                         .entryCount=3,
                                         .entries=(WGPUBindGroupLayoutEntry[3]) {
                                            {
                                               .binding=0,
                                               .visibility=WGPUShaderStage_Vertex,
                                               .buffer=(WGPUBufferBindingLayout) {
                                                  .type=WGPUBufferBindingType_Uniform
                                               }
                                            },
                                            {
                                               .binding=1,
                                               .visibility=WGPUShaderStage_Fragment,
                                               .texture=(WGPUTextureBindingLayout) {
                                                  .sampleType=WGPUTextureSampleType_Float,
                                                  .viewDimension=WGPUTextureViewDimension_2D
                                               }
                                            },
                                            {
                                               .binding=2,
                                               .visibility=WGPUShaderStage_Fragment,
                                               .sampler=(WGPUSamplerBindingLayout) {
                                                  .type=WGPUSamplerBindingType_Filtering
                                               }
                                            }
                                         }
                                      });
   ctx->tempBindGroup =
      wgpuDeviceCreateBindGroup(renderer->device,
                                &(WGPUBindGroupDescriptor) {
                                   .layout=ctx->tempBindGroupLayout,
                                   .label="Bind Group",
                                   .entryCount=3,
                                   .entries=(WGPUBindGroupEntry[3]) {
                                      {
                                         .binding=0,
                                         .size=ctx->uniformBufferSize,
                                         .buffer=ctx->uniformBuffer
                                      },
                                      {
                                         .binding=1,
                                         .textureView=ctx->theOnlyMaterial->textureView
                                      },
                                      {
                                         .binding=2,
                                         .sampler=ctx->theOnlyMaterial->sampler
                                      }
                                   }
                                });
   if (ctx->tempBindGroup == NULL)
   {
      REPORT_NULL_FAULT(ctx->tempBindGroup, err);
      return NULL;
   }
   ctx->vertexBuffer = wgpuDeviceCreateBuffer(renderer->device, &(WGPUBufferDescriptor) {
      .mappedAtCreation=true,
      .usage= WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
      .size=sizeof vertices,
      .label="Toy Vertex Buffer"
   });


   void *memory = wgpuBufferGetMappedRange(ctx->vertexBuffer, 0, sizeof vertices);
   memcpy(memory, vertices, sizeof vertices);
   wgpuBufferUnmap(ctx->vertexBuffer);
   return true;
//   assert(ctx);
//   assert(scene);
//   if (!sceneBuildGeometry2D(scene, &ctx->vertices, &ctx->indices, err))
//   {
//      return false;
//   }
//
//   assert(ctx->vertices);
//   assert(ctx->indices);
//
//   // Vertex Buffer
//   size_t vertexBufferSize = ctx->vertices->count * sizeof(*ctx->vertices->data);
//   ctx->vertexBuffer = wgpuDeviceCreateBuffer(ctx->renderer->device, &(WGPUBufferDescriptor) {
//      .label="Vertex Buffer",
//      .size=vertexBufferSize,
//      .usage=WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex
//   });
//
//   if (ctx->vertexBuffer == NULL)
//   {
//      REPORT_NULL_FAULT(ctx->vertexBuffer, err);
//      return false;
//   }
//   AspectRenderer *renderer = ctx->renderer;
//   // TODO we may not need to do this here
//   wgpuQueueWriteBuffer(renderer->queue,
//                        ctx->vertexBuffer,
//                        0,
//                        &ctx->vertices->data,
//                        ctx->vertices->count * sizeof(*ctx->vertices->data));
//
//   // Index Buffer
//   ctx->indexBuffer = wgpuDeviceCreateBuffer(renderer->device, &(WGPUBufferDescriptor) {
//      .label="Index Buffer",
//      .size=ctx->indices->count * sizeof(*ctx->indices->data),
//      .usage=WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index
//   });
//   if (ctx->indexBuffer == NULL)
//   {
//      REPORT_NULL_FAULT(ctx->indexBuffer, err);
//      return false;
//   }
//
//   // Geometry Bind Group Layout
//   ctx->geometryBindGroupLayout = wgpuDeviceCreateBindGroupLayout(
//      renderer->device,
//      &(WGPUBindGroupLayoutDescriptor) {
//         .entryCount=1,
//         .entries=(WGPUBindGroupLayoutEntry[]) {
//            {
//               .binding=0,
//               .visibility=WGPUShaderStage_Vertex
//            },
//            {
//               .binding=1,
//               .visibility=WGPUShaderStage_Vertex,
//               .buffer={
//                  .type=WGPUBufferBindingType_ReadOnlyStorage,
//                  .hasDynamicOffset=false
//               }
//            }
//         }
//      });
//
//   if (ctx->geometryBindGroupLayout == NULL)
//   {
//      REPORT_NULL_FAULT(ctx->geometryBindGroupLayout, err);
//      return false;
//   }
//
//
//   ctx->geometryBindGroup = wgpuDeviceCreateBindGroup(renderer->device, &(WGPUBindGroupDescriptor) {
//      .layout = ctx->geometryBindGroupLayout,
//      .label = "Vertex Bind Group",
//      .entryCount = 1,
//      .entries = (WGPUBindGroupEntry[]) {
//         {
//            .binding=0,
//            .buffer=ctx->uniformBuffer,
//            .offset=0,
//            .size=ctx->uniformBufferSize
//         },
//         {
//            .binding=1,
//            .buffer=ctx->vertexBuffer,
//            .offset=0,
//            .size=vertexBufferSize
//         }
//      }
//   });
//   if (ctx->geometryBindGroup == NULL)
//   {
//      REPORT_NULL_FAULT(ctx->geometryBindGroup, err);
//      return false;
//   }
   return true;
}

AspectRenderContext *
aspectRenderContextNew(AspectRenderer *renderer,
                       AspectScene *scene,
                       Error **err)
{
   assert(renderer);
   //assert(scene);

   AspectRenderContext *ctx = malloc(sizeof(AspectRenderContext));
   if (ctx == NULL)
   {
      REPORT_NULL_FAULT(context, err);
      return NULL;
   }

   ctx->renderer = renderer;


   // Uniform Buffer
   ctx->uniformBufferSize = 64 * 3;
   ctx->uniformBuffer = wgpuDeviceCreateBuffer(renderer->device, &(WGPUBufferDescriptor) {
      .label="Uniform Buffer",
      .size=ctx->uniformBufferSize,
      .usage=WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
   });
   if (ctx->uniformBuffer == NULL)
   {
      REPORT_NULL_FAULT(ctx->uniformBuffer, err);
      goto onFailure;
   }

   ctx->theOnlyMaterial = aspectMaterialNewFromFile(renderer->device,
                                                    "/Users/rpendergraph/CLionProjects/learning/Resources/test.png",
                                                    err);
   if (!aspectMaterialWriteToQueue(renderer, ctx->theOnlyMaterial, err))
   {
      goto onFailure;
   }
   if (!contextComputeSceneGeometry(ctx, scene, renderer, err))
   {
      goto onFailure;
   }

//   wgpuQueueWriteBuffer(renderer->queue, ctx->uniformBuffer, 0, ctx->uniformBuffer, ctx->uniformBufferSize);

//   // Materials Bind group
//   ctx->materialsBindGroupLayout = wgpuDeviceCreateBindGroupLayout(
//      renderer->device,
//      &(WGPUBindGroupLayoutDescriptor) {
//         .entryCount=1,
//         .entries=(WGPUBindGroupLayoutEntry[]) {
//            {
//               .binding=0,
//               .visibility=WGPUShaderStage_Fragment,
//            },
//            {
//               .binding=1,
//               .visibility=WGPUShaderStage_Fragment,
//            }
//         }
//      });
//
//   if (ctx->materialsBindGroupLayout == NULL)
//   {
//      REPORT_NULL_FAULT(ctx->materialsBindGroupLayout, err);
//      goto onFailure;
//   }
//
//
//   ctx->materialsBindGroup = wgpuDeviceCreateBindGroup(renderer->device, &(WGPUBindGroupDescriptor) {
//      .layout = ctx->materialsBindGroupLayout,
//      .label = "Materials Bind Group",
//      .entryCount = 1,
//      .entries = (WGPUBindGroupEntry[]) {
//         {
//            .binding=0,
//            .buffer=ctx->uniformBuffer,
//            .offset=0,
//            .size=ctx->uniformBufferSize
//         }
//      }
//   });
//   if (ctx->materialsBindGroup == NULL)
//   {
//      REPORT_NULL_FAULT(ctx->bindGroup, err);
//      goto onFailure;
//   }


//   ctx->depthTexture = wgpuDeviceCreateTexture(renderer->device, &(WGPUTextureDescriptor) {
//      .dimension=WGPUTextureDimension_2D,
//      .format=WGPUTextureFormat_Depth24Plus,
//      .mipLevelCount=1,
//      .sampleCount=1,
//      .size={640, 480, 1},
//      .usage=WGPUTextureUsage_CopyDst,
//      .viewFormatCount=1,
//      .viewFormats=(WGPUTextureFormat[]) {
//         WGPUTextureFormat_Depth24Plus
//      },
//   });
//   if (ctx->depthTexture == NULL)
//   {
//      REPORT_NULL_FAULT(ctx->depthTexture, err);
//      goto onFailure;
//   }

//   ctx->depthTextureView = wgpuTextureCreateView(ctx->depthTexture, &(WGPUTextureViewDescriptor) {
//      .aspect=WGPUTextureAspect_DepthOnly,
//      .baseArrayLayer=0,
//      .arrayLayerCount=1,
//      .baseMipLevel=0,
//      .mipLevelCount=1,
//      .dimension=WGPUTextureViewDimension_2D,
//      .format=WGPUTextureFormat_Depth24Plus
//   });
//   if (ctx->depthTextureView == NULL)
//   {
//      REPORT_NULL_FAULT(ctx->depthTextureView, err);
//      goto onFailure;
//   }
//   ctx->depthStencilAttachment = (WGPURenderPassDepthStencilAttachment) {
//      .depthClearValue=1.0f, // The initial value of the depth buffer, meaning "far"
//      .depthLoadOp=WGPULoadOp_Clear,
//      .depthReadOnly = false,
//      .depthStoreOp=WGPUStoreOp_Store,
//      .stencilLoadOp = WGPULoadOp_Undefined,
//      .stencilReadOnly = true,
//      .stencilStoreOp = WGPUStoreOp_Undefined,
//      .view = ctx->depthTextureView,
//   };

   ctx->pipeline = createRenderPipeline(renderer, ctx, err);
   return ctx;

   onFailure:
   aspectRenderContextDestroy(ctx);
   return NULL;
}


bool
aspectRenderContextRender(AspectRenderContext *ctx, Error **err)
{

   WGPUTextureView currentTexture = NULL;
   WGPUCommandEncoder commandEncoder = NULL;
   WGPURenderPassEncoder renderPass = NULL;

   if (ctx == NULL)
   {
      REPORT_NULL_FAULT(ctx, err);
      return false;
   }
   ctx->t += .01f;
   if (ctx->t > 2 * M_PI)
   {
      ctx->t = 0;
   }
   HMM_Mat4 projection = HMM_Perspective_RH_ZO(M_PI_4, 800.f / 600.f, .1f, 10.f);
   HMM_Mat4 view = HMM_LookAt_RH(HMM_V3(-2, 0, 2), HMM_V3(0, 0, 0), HMM_V3(0, 0, 1));
   HMM_Mat4 model = HMM_Rotate_RH(ctx->t, HMM_V3(0, 0, 1));
   wgpuQueueWriteBuffer(ctx->renderer->queue, ctx->uniformBuffer, 0, &model, sizeof model);
   wgpuQueueWriteBuffer(ctx->renderer->queue, ctx->uniformBuffer, 64, &view, sizeof view);
   wgpuQueueWriteBuffer(ctx->renderer->queue, ctx->uniformBuffer, 128, &projection, sizeof projection);

   AspectRenderer *renderer = ctx->renderer;
   assert(renderer);

   currentTexture = wgpuSwapChainGetCurrentTextureView(renderer->swapChain);
   if (currentTexture == NULL)
   {
      REPORT_NULL_FAULT(currentTexture, err);
      return false;
   }

   commandEncoder = wgpuDeviceCreateCommandEncoder(
      renderer->device,
      &(WGPUCommandEncoderDescriptor) {
         .label="Command Encoder"
      });
   if (commandEncoder == NULL)
   {
      REPORT_NULL_FAULT(commandEncoder, err);
      goto onFailure;
   }

   renderPass = wgpuCommandEncoderBeginRenderPass(
      commandEncoder,
      &(WGPURenderPassDescriptor) {
         .colorAttachmentCount=1,
         .colorAttachments=&(WGPURenderPassColorAttachment) {
            .view=currentTexture,
            .loadOp=WGPULoadOp_Clear,
            .storeOp=WGPUStoreOp_Store,
            .clearValue={.2, 0.1, 0.2, 1}
         },
         //.depthStencilAttachment = &ctx->depthStencilAttachment
      });

   if (renderPass == NULL)
   {
      REPORT_NULL_FAULT(renderPass, err);
      goto onFailure;
   }
/*
 *         this.device.queue.writeBuffer(
            this.objectBuffer, 0,
            renderables.model_transforms, 0,
            renderables.model_transforms.length
        );
        this.device.queue.writeBuffer(this.uniformBuffer, 0, <ArrayBuffer>view);
        this.device.queue.writeBuffer(this.uniformBuffer, 64, <ArrayBuffer>projection);
 */
   wgpuRenderPassEncoderSetPipeline(renderPass, ctx->pipeline);
//   wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0, ctx->vertexBuffer, 0,
//                                        sizeof(*ctx->vertices->data) * ctx->vertices->count);
//   wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, ctx->indexBuffer, WGPUIndexFormat_Uint32, 0,
//                                       sizeof(*ctx->indices->data) * ctx->indices->count);
   wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, ctx->vertexBuffer, 0, 72);
   wgpuRenderPassEncoderSetBindGroup(renderPass, 0, ctx->tempBindGroup, 0, NULL);
//   wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, ctx->indices->count, 1, 0, 0, 0);
   wgpuRenderPassEncoderDraw(renderPass, 3, 1, 0, 0);
   wgpuRenderPassEncoderEnd(renderPass);
   WGPUCommandBuffer command = wgpuCommandEncoderFinish(commandEncoder,
                                                        &(WGPUCommandBufferDescriptor) {.label="Command Buffer"});
   wgpuQueueSubmit(renderer->queue, 1, &command);
   wgpuSwapChainPresent(renderer->swapChain);
   wgpuTextureViewRelease(currentTexture);
   wgpuDeviceTick(renderer->device);
   return true;
   onFailure:

   return false;
}
