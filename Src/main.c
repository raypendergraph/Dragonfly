#include <Platform/IO.h>
#include <Platform/Core.h>
#include <Platform/Text.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>
#include "Aspect/Aspect.h"

typedef struct
{
   WGPUAdapter adapter;
   WGPURequestAdapterStatus status;
   const char *message;
} AdapterRequest;

typedef struct
{
   WGPUDevice device;
   WGPURequestDeviceStatus status;
   const char *message;
} DeviceRequest;

typedef struct
{
   WGPUInstance instance;
   WGPUDevice device;
   WGPUAdapter adapter;
   WGPUSwapChain swapChain;
   WGPUQueue queue;

   WGPUBuffer indexBuffer;
   WGPUBuffer vertexBuffer;
   WGPUBuffer uniformBuffer;
} RenderContext;


WGPURenderPipeline
createRenderPipeline(RenderContext *ctx, WGPUBindGroupLayout bindGroupLayout, Error **err)
{
   const char *shaderSource = pfmLoadContentAtPathAsString(
      "/Users/rpendergraph/CLionProjects/learning/Resources/shader.wgsl", err);
   assert(shaderSource);


   WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(
      ctx->device,
      &(WGPUPipelineLayoutDescriptor) {
         .label="Pipeline Layout",
         .bindGroupLayoutCount=1,
         .bindGroupLayouts=(WGPUBindGroupLayout[]) {bindGroupLayout}});
   assert(pipelineLayout);

   WGPUShaderModule shaderModule =
      aspectNewWGSLShaderModule(ctx->device, shaderSource, "shaderModule");
   assert(shaderModule);

   WGPURenderPipelineDescriptor d = {
      .label="renderPipeline",
      .layout=pipelineLayout,
      .multisample=(WGPUMultisampleState) {
         .count=1,
         .mask=~0u,
      },
      .vertex=(const WGPUVertexState) {
         .module=shaderModule,
         .entryPoint="vs_main",
         .bufferCount=1,
         .buffers=&(WGPUVertexBufferLayout) {
            .arrayStride=5 * sizeof(float),
            .stepMode=WGPUVertexStepMode_Vertex,
            .attributeCount=2,
            .attributes=(WGPUVertexAttribute[2]) {
               {

                  .shaderLocation=0,
                  .format=WGPUVertexFormat_Float32x2,
                  .offset=0

               },
               {
                  .shaderLocation=1,
                  .format=WGPUVertexFormat_Float32x3,
                  .offset=2 * sizeof(float)
               }
            }}},
      .fragment=&(const WGPUFragmentState) {
         .entryPoint="fs_main",
         .module=shaderModule,
         .targetCount=1,
         .targets=(const WGPUColorTargetState[1]) {
            {
               .format=WGPUTextureFormat_BGRA8Unorm,
               .writeMask=WGPUColorWriteMask_All,
               .blend = &(WGPUBlendState) {
                  .color={
                     .srcFactor=WGPUBlendFactor_SrcAlpha,
                     .dstFactor=WGPUBlendFactor_OneMinusSrcAlpha,
                     .operation=WGPUBlendOperation_Add},
                  .alpha={
                     .srcFactor=WGPUBlendFactor_Zero,
                     .dstFactor=WGPUBlendFactor_One,
                     .operation=WGPUBlendOperation_Add}}
            }
         }},
      .primitive=(const WGPUPrimitiveState) {
         .topology=WGPUPrimitiveTopology_TriangleList,
         .stripIndexFormat=WGPUIndexFormat_Undefined,
         .frontFace=WGPUFrontFace_CCW,
         .cullMode=WGPUCullMode_None

      }};
   return wgpuDeviceCreateRenderPipeline(ctx->device, &d);
}

static void
handleRequestAdapterCompleted(WGPURequestAdapterStatus status, WGPUAdapter adapter, char const *message,
                              void *userData)
{
   AdapterRequest *ctx = (AdapterRequest *) userData;
   ctx->status = status;
   ctx->adapter = adapter;
   ctx->message = message;
   fprintf(stdout, "request adapter completed\n");
}

void
handleDeviceRequestCompleted(WGPURequestDeviceStatus status, WGPUDevice device, char const *message, void *userData)
{
   DeviceRequest *ctx = (DeviceRequest *) userData;
   ctx->status = status;
   ctx->device = device;
   ctx->message = message;
   fprintf(stdout, "device request completed\n");
}

static void
handleDeviceLost(WGPUDeviceLostReason reason, char const *message, void *userdata)
{
   fprintf(stderr, "Device was lost [%s]: %s\n", aspectDeviceLostReasonGetString(reason), message);
}

static void
handleDeviceError(WGPUErrorType type, char const *message, void *userData)
{
   printf("Device error [%s]: %s\n", aspectErrorTypeGetString(type), message);
}

static void
handleQueueWorkComplete(WGPUQueueWorkDoneStatus status, void *userData)
{
   fprintf(stdout, "Queue work done [%s]\n", aspectQueueWorkDoneStatusGetString(status));
}

WGPUDevice
createDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const *deviceDescriptor)
{
   DeviceRequest *deviceReq = malloc(sizeof(*deviceReq));
   wgpuAdapterRequestDevice(
      adapter,
      deviceDescriptor,
      handleDeviceRequestCompleted,
      deviceReq);

   if (deviceReq->device == NULL)
   {
      fprintf(stderr, "Request device failed: %s", deviceReq->message);
      assert(false);
      goto final;
   }

   wgpuDeviceSetUncapturedErrorCallback(deviceReq->device, handleDeviceError, NULL);
   wgpuDeviceSetDeviceLostCallback(deviceReq->device, handleDeviceLost, NULL);
   WGPUDevice device = deviceReq->device;

   final:
   free(deviceReq);
   return device;
}

WGPUAdapter
createAdapter(WGPUInstance instance, WGPURequestAdapterOptions const *options)
{
   AdapterRequest *adapterReq = malloc(sizeof(*adapterReq));
   wgpuInstanceRequestAdapter(
      instance,
      options,
      handleRequestAdapterCompleted,
      adapterReq);
   assert(adapterReq->adapter);
   if (adapterReq->adapter == NULL)
   {
      fprintf(stderr, "Request adapter failed: %s", adapterReq->message);
      assert(false);
   }
   //aspectAdapterPrintFeatures(adapterReq->adapter);
   return adapterReq->adapter;
}

RenderContext
*initializeWGPU(GLFWwindow *window)
{
   WGPUInstance instance = wgpuCreateInstance(NULL);
   assert(instance);

   WGPUSurface surface = aspectNewSurfaceForGLFW(instance, window);
   assert(surface);

   //Apparently required for native?
   const char *allowUnsafeApisToggle = "allow_unsafe_apis";
   WGPUAdapter adapter = createAdapter(instance, &(WGPURequestAdapterOptions) {
      .nextInChain=(WGPUChainedStruct *) &(WGPUDawnTogglesDescriptor) {
         .chain={
            .sType=WGPUSType_DawnTogglesDescriptor},
         .enabledToggleCount=1,
         .enabledToggles=&allowUnsafeApisToggle},
      .compatibleSurface=surface});
   assert(adapter);

   WGPUSupportedLimits supportedLimits = {};
   bool getLimitsSucceeded = wgpuAdapterGetLimits(adapter, &supportedLimits);
   assert(getLimitsSucceeded);

   WGPUDeviceDescriptor dd = {
      .requiredLimits=&(WGPURequiredLimits) {
         .limits=(WGPULimits) {
            .maxBufferSize=6 * 5 * sizeof(float), // six 2D data
            .maxInterStageShaderComponents=3,
            .maxVertexAttributes=2,
            .maxVertexBufferArrayStride=5 * sizeof(float),
            .maxVertexBuffers=1,
            .maxBindGroups=1,
            .maxUniformBuffersPerShaderStage=1,
            .maxUniformBufferBindingSize= 16 * 4,
            .minStorageBufferOffsetAlignment=supportedLimits.limits.minStorageBufferOffsetAlignment,
            .minUniformBufferOffsetAlignment=supportedLimits.limits.minStorageBufferOffsetAlignment}}};

   WGPUDevice device = createDevice(adapter, &dd);
   assert(device);

   WGPUQueue queue = wgpuDeviceGetQueue(device);
   assert(queue);
   wgpuQueueOnSubmittedWorkDone(queue, 0U, handleQueueWorkComplete, NULL);


   // This is not implemented yet in Dawn
   //WGPUTextureFormat swapChainFormat=wgpuSurfaceGetPreferredFormat(surface, adapter);
   //swapChainDesc.format=swapChainFormat;
   WGPUSwapChain swapChain = wgpuDeviceCreateSwapChain(
      device,
      surface,
      &(WGPUSwapChainDescriptor) {
         .width=640,
         .height=480,
         .format=WGPUTextureFormat_BGRA8Unorm,
         .usage=WGPUTextureUsage_RenderAttachment,
         .presentMode=WGPUPresentMode_Fifo
      });
   assert(swapChain);

   RenderContext *ctx = malloc(sizeof(*ctx));
   *ctx = (RenderContext) {
      .instance=instance,
      .adapter=adapter,
      .device=device,
      .swapChain=swapChain,
      .queue=queue

   };

   return ctx;

//    cleanup:
//    if (swapChain) wgpuSwapChainRelease(swapChain);
//    if (instance) wgpuInstanceRelease(instance);
//    if (adapter) wgpuAdapterRelease(adapter);
//    if (device) wgpuDeviceRelease(device);
//    if (commandEncoder) wgpuCommandEncoderRelease(commandEncoder);
//    if (command) wgpuCommandBufferRelease(command);


}

int
main(int argc, char **argv)
{
   Error **err = NULL;
   Scene *scene = aspectSceneCreateFromFile("/Users/rpendergraph/CLionProjects/learning/Resources/scene.xml", &err);
   int result = glfwInit();
   assert(result);
   glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
   glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
   GLFWwindow *window = glfwCreateWindow(640, 480, "Learn WebGPU", NULL, NULL);
   assert(window);
   RenderContext *ctx = initializeWGPU(window);
   assert(ctx);
   VertexBuffer2D *vertexData;
   IndexBuffer *indexData;
   aspectSceneBuildGeometry2D(scene, &vertexData, &indexData, err);
   printf("%s", aspectVertexBuffer2dToString(vertexData));
   ctx->vertexBuffer = wgpuDeviceCreateBuffer(ctx->device, &(WGPUBufferDescriptor) {
      .label="Vertex Buffer",
      .size=vertexData->count * sizeof(*vertexData->data),
      .usage=WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex
   });
   assert(ctx->vertexBuffer);
   wgpuQueueWriteBuffer(ctx->queue,
                        ctx->vertexBuffer,
                        0,
                        &vertexData->data,
                        vertexData->count * sizeof(*vertexData->data));

   ctx->indexBuffer = wgpuDeviceCreateBuffer(ctx->device, &(WGPUBufferDescriptor) {
      .label="Index Buffer",
      .size=indexData->count * sizeof(*indexData->data),
      .usage=WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index
   });
   assert(ctx->indexBuffer);
   wgpuQueueWriteBuffer(ctx->queue,
                        ctx->indexBuffer,
                        0,
                        &indexData->data,
                        indexData->count * sizeof(*indexData->data));

   ctx->uniformBuffer = wgpuDeviceCreateBuffer(ctx->device, &(WGPUBufferDescriptor) {
      .label="Uniform Buffer",
      .size=sizeof(float),
      .usage=WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
   });
   float currentTime = 1.0f;
   wgpuQueueWriteBuffer(ctx->queue, ctx->uniformBuffer, 0, &currentTime, sizeof(float));
   assert(ctx->uniformBuffer);


   WGPUBindGroupLayout bindGroupLayout =
      wgpuDeviceCreateBindGroupLayout(
         ctx->device,
         &(WGPUBindGroupLayoutDescriptor) {
            .entryCount=1,
            .entries=(WGPUBindGroupLayoutEntry[]) {
               {
                  .binding=0,
                  .buffer={
                     .minBindingSize=sizeof(float),
                     .type=WGPUBufferBindingType_Uniform,
                  },
                  .visibility=WGPUShaderStage_Vertex,
               }
            }
         });
   WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(ctx->device, &(WGPUBindGroupDescriptor) {
      .layout = bindGroupLayout,
      .label = "Uniform Bind Group",
      .entryCount = 1,
      .entries = (WGPUBindGroupEntry[]) {
         {
            .binding=0,
            .buffer=ctx->uniformBuffer,
            .offset=0,
            .size=sizeof(float)
         }
      }
   });

   WGPURenderPipeline pipeline = createRenderPipeline(ctx, bindGroupLayout, err);
   assert(pipeline);
   while (!glfwWindowShouldClose(window))
   {
      wgpuDeviceTick(ctx->device);
      glfwPollEvents();

      WGPUTextureView currentTexture = wgpuSwapChainGetCurrentTextureView(ctx->swapChain);
      if (currentTexture == NULL)
      {
         printf("ignoring NULL texture view");
         continue;
      }
      WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(
         ctx->device,
         &(WGPUCommandEncoderDescriptor) {
            .label="Command Encoder"
         });
      assert(commandEncoder);

      WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(
         commandEncoder,
         &(WGPURenderPassDescriptor) {
            .colorAttachmentCount=1,
            .colorAttachments=&(WGPURenderPassColorAttachment) {
               .view=currentTexture,
               .loadOp=WGPULoadOp_Clear,
               .storeOp=WGPUStoreOp_Store,
               .clearValue={.r=0.9, .g=0.1, .b=0.2, .a=1.0}
            }
         });
      assert(renderPassEncoder);
      wgpuRenderPassEncoderSetPipeline(renderPassEncoder, pipeline);
      wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0, ctx->vertexBuffer, 0,
                                           sizeof(*vertexData->data) * vertexData->count);
      wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, ctx->indexBuffer, WGPUIndexFormat_Uint32, 0,
                                          sizeof(*indexData->data) *
                                          indexData->count);
      wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0, bindGroup, 0, NULL);
      float t = (float) glfwGetTime();
      wgpuQueueWriteBuffer(ctx->queue, ctx->uniformBuffer, 0, &t, sizeof(float));
      wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, indexData->count, 1, 0, 0, 0);

      wgpuRenderPassEncoderEnd(renderPassEncoder);
      wgpuTextureViewRelease(currentTexture);

      WGPUCommandBuffer command = wgpuCommandEncoderFinish(commandEncoder,
                                                           &(WGPUCommandBufferDescriptor) {.label="Command Buffer"});

      wgpuQueueSubmit(ctx->queue, 1, &command);

      wgpuSwapChainPresent(ctx->swapChain);
      currentTime = (float) glfwGetTime();
   }

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

