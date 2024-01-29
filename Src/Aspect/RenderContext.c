#include "webgpu/webgpu.h"
#include <Platform/Core.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "RenderContext.h"
#include "Material.h"
#include "Scene.h"
#include "HandmadeMath.h"
#include "SceneNodeIterator.h"
#include <GLFW/glfw3.h>

struct SceneNodeIterator *
aspectRenderContextNewSceneNodeIterator(AspectRenderContext *ctx, Error **err)
{
   struct SceneNodeIterator *it =
      malloc(sizeof(struct SceneNodeIterator) + ctx->sceneDepth * sizeof(StackFrame));
   if (it == NULL)
   {
      ERROR(err, PFM_ERR_ALLOC_FAILED, "it");
      return NULL;
   }

   it->stack[0] = (StackFrame) {
      .array = ctx->nodes,
      .count = ctx->nodeCount
   };

   return it;
}

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
               .arrayStride=20,
               .attributeCount=2,
               .attributes=(WGPUVertexAttribute[2]) {
                  {
                     .shaderLocation=0,
                     .format= WGPUVertexFormat_Float32x3,
                     .offset=0},
                  {
                     .shaderLocation=1,
                     .format=WGPUVertexFormat_Float32x2,
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
               .format=WGPUTextureFormat_BGRA8Unorm,
               .writeMask=WGPUColorWriteMask_All
            }
         }
      },
      .primitive=(WGPUPrimitiveState) {
         .topology=WGPUPrimitiveTopology_TriangleList,
      },
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
      0.f, 1.f, 0.0, 0.5, 0.0,
      -1.f, -1.f, -0.0, 0.0, 1.0,
      1.f, -1.f, -0.0, 1.0, 1.0
   };
   const size_t OBJECT_BUFFER_SIZE = sizeof(HMM_Mat4) * 1024;
   ctx->objectBuffer = wgpuDeviceCreateBuffer(renderer->device, &(WGPUBufferDescriptor) {
      .size=OBJECT_BUFFER_SIZE,
      .usage=WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage
   });

   ctx->tempBindGroupLayout =
      wgpuDeviceCreateBindGroupLayout(renderer->device,
                                      &(WGPUBindGroupLayoutDescriptor) {
                                         .label="Bind Group Layout",
                                         .entryCount=4,
                                         .entries=(WGPUBindGroupLayoutEntry[4]) {
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
                                            },
                                            {
                                               .binding=3,
                                               .visibility=WGPUShaderStage_Vertex,
                                               .buffer=(WGPUBufferBindingLayout) {
                                                  .type=WGPUBufferBindingType_ReadOnlyStorage,
                                                  .hasDynamicOffset=false,
                                               }
                                            },
                                         }
                                      });
   ctx->tempBindGroup =
      wgpuDeviceCreateBindGroup(renderer->device,
                                &(WGPUBindGroupDescriptor) {
                                   .layout=ctx->tempBindGroupLayout,
                                   .label="Bind Group",
                                   .entryCount=4,
                                   .entries=(WGPUBindGroupEntry[4]) {
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
                                      },
                                      {
                                         .binding=3,
                                         .size=OBJECT_BUFFER_SIZE,
                                         .buffer=ctx->objectBuffer
                                      },
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
      .label="Vertex Buffer"
   });


   void *memory = wgpuBufferGetMappedRange(ctx->vertexBuffer, 0, sizeof vertices);
   memcpy(memory, vertices, sizeof vertices);
   wgpuBufferUnmap(ctx->vertexBuffer);
   return true;
}

#include "Behavior.h"

struct SceneNode
demoCreateSceneNode(char *label, float x)
{
   struct Actor *actor = malloc(sizeof(struct Actor));
   struct RotateBehavior *rotate = malloc(sizeof(struct RotateBehavior));
   rotate->behavior.kind = ASPECT_BEHAVIOR_TYPE_ROTATE;
   rotate->eulerAngles = HMM_V3(0, .1f, 0);
   actor->behaviorCount = 1;
   actor->behaviors = (struct Behavior *) rotate;
   return (struct SceneNode) {
      .label=label,
      .actor=actor,
      .position=HMM_V3(x, 0, 0),
      .rotation=HMM_Q(0, 0, 0, 1),
      .scale=HMM_V3(1, 1, 1)
   };
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

   ctx->nodes = malloc(3 * sizeof(struct SceneNode));
   ctx->nodeCount = 3;
   ctx->nodes[0] = demoCreateSceneNode("Node 1", -2.f);
   ctx->nodes[1] = demoCreateSceneNode("Node 2", 0.f);
   ctx->nodes[2] = demoCreateSceneNode("Node 3", 2.f);
   //TODO get the depth when loading the scene
   ctx->sceneDepth = 1;
   // Uniform Buffer
   ctx->uniformBufferSize = sizeof(HMM_Mat4) * 2;
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
   HMM_Vec3 position = HMM_V3(0, 0, 0);
   ctx->camera = aspectCameraNew(position, 0, 0, err);
   if (ctx->camera == NULL)
   {
      REPORT_NULL_FAULT(ctx->camera, err);
      goto onFailure;
   }
   ctx->pipeline = createRenderPipeline(renderer, ctx, err);
   return ctx;

   onFailure:
   aspectRenderContextDestroy(ctx);
   return NULL;
}


bool
aspectRenderContextRender(AspectRenderContext *ctx, Error **err)
{

   if (ctx == NULL)
   {
      REPORT_NULL_FAULT(ctx, err);
      return false;
   }

   WGPUTextureView currentTexture = NULL;
   WGPUCommandEncoder commandEncoder = NULL;
   WGPURenderPassEncoder renderPass = NULL;

//   ctx->t += .01f;
//   if (ctx->t > 2 * M_PI)
//   {
//      ctx->t = 0;
//   }
//
   aspectCameraUpdate(ctx->camera);

   AspectRenderer *renderer = ctx->renderer;
   assert(renderer);


   // Model data
   for (size_t i = 0; i < ctx->nodeCount; i++)
   {
      HMM_Mat4 model = aspectSceneNodeCalculateModel(&ctx->nodes[i]);
      wgpuQueueWriteBuffer(renderer->queue, ctx->objectBuffer, i * sizeof(model), &model, sizeof(model));
   }
   // Camera View
   HMM_Mat4 view = HMM_LookAt_RH(HMM_V3(0, 0, -3), HMM_V3(0, 0, 0), HMM_V3(0, 1, 0));
   wgpuQueueWriteBuffer(ctx->renderer->queue, ctx->uniformBuffer, 0, &view, sizeof view);
   // Projection
   HMM_Mat4 projection = HMM_Perspective_RH_ZO(M_PI_4, 800.f / 600.f, .1f, 10.f);
   wgpuQueueWriteBuffer(ctx->renderer->queue, ctx->uniformBuffer, 64, &projection, sizeof projection);

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

   currentTexture = wgpuSwapChainGetCurrentTextureView(renderer->swapChain);
   if (currentTexture == NULL)
   {
      REPORT_NULL_FAULT(currentTexture, err);
      return false;
   }

   renderPass = wgpuCommandEncoderBeginRenderPass(
      commandEncoder,
      &(WGPURenderPassDescriptor) {
         .label="Render Pass",
         .colorAttachmentCount=1,
         .colorAttachments=(WGPURenderPassColorAttachment[1]) {
            {
               .view=currentTexture,
               .loadOp=WGPULoadOp_Clear,
               .storeOp=WGPUStoreOp_Store,
               .clearValue={.2, 0.4, 0.2, 1.}
            }
         },
         //.depthStencilAttachment = &ctx->depthStencilAttachment
      });

   if (renderPass == NULL)
   {
      REPORT_NULL_FAULT(renderPass, err);
      goto onFailure;
   }

   wgpuRenderPassEncoderSetPipeline(renderPass, ctx->pipeline);
   wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, ctx->vertexBuffer, 0, 60);
   wgpuRenderPassEncoderSetBindGroup(renderPass, 0, ctx->tempBindGroup, 0, NULL);
   wgpuRenderPassEncoderDraw(renderPass, 3, ctx->nodeCount, 0, 0);
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

void
aspectRenderContextOnUpdate(AspectRenderContext *ctx)
{
   Error *todo = NULL;
   struct SceneNodeIterator *it = aspectRenderContextNewSceneNodeIterator(ctx, &todo);
   struct SceneNode *node = aspectSceneNodeIteratorNext(it);
   while (node != NULL)
   {
      aspectSceneNodeOnUpdate(node);
      node = aspectSceneNodeIteratorNext(it);
   }
   aspectSceneNodeIteratorDestroy(it);
}

void
aspectRenderContextHandleKeypress(AspectRenderContext *ctx, int key, int scancode, int action, int mods)
{
   printf("key=%d code=%d action=%d mods=%d\n", key, scancode, action, mods);
//   switch(key)
//   {
//      case GLFW_KEY_RIGHT:
//         switch (action)
//         {
//            case GLFW_PRESS:
//            case GLFW_REPEAT:
//               ctx->camera->eulers
//         }
//      case GLFW_KEY_UP:
//         switch (action)
//         {
//            case GLFW_PRESS:
//            case GLFW_REPEAT:
//
//
//         }
//
//   }
//   ctx->camera->
}