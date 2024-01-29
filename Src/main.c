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

typedef struct MyUniforms
{
   uint8_t data[128];
} MyUniforms;

static_assert(sizeof(MyUniforms) % 16 == 0, "uniforms not properly padded");
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

//typedef struct
//{
//   WGPUInstance instance;
//   WGPUDevice device;
//   WGPUAdapter adapter;
//   WGPUSwapChain swapChain;
//   WGPUQueue queue;
//
//   WGPUBuffer indexBuffer;
//   WGPUBuffer vertexBuffer;
//   WGPUBuffer uniformBuffer;
//} AspectRenderContext;


//WGPURenderPipeline
//createRenderPipeline(RenderContext *ctx, WGPUBindGroupLayout bindGroupLayout, Error **err)
//{
//   const char *shaderSource = pfmLoadContentAtPathAsString(
//      "/Users/rpendergraph/CLionProjects/learning/Resources/shader.wgsl", err);
//   assert(shaderSource);
//
//
//   WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(
//      ctx->device,
//      &(WGPUPipelineLayoutDescriptor) {
//         .label="Pipeline Layout",
//         .bindGroupLayoutCount=1,
//         .bindGroupLayouts=(WGPUBindGroupLayout[]) {bindGroupLayout}});
//   assert(pipelineLayout);
//
//   WGPUShaderModule shaderModule =
//      aspectNewWGSLShaderModule(ctx->device, shaderSource, "shaderModule");
//   assert(shaderModule);
//
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
//   return wgpuDeviceCreateRenderPipeline(ctx->device, &d);
//}

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
   fprintf(stderr, "Device was lost [%s]: %s\n", deviceLostReasonGetString(reason), message);
}

static void
handleDeviceError(WGPUErrorType type, char const *message, void *userData)
{
   printf("Device error [%s]: %s\n", errorTypeGetString(type), message);
}

static void
handleQueueWorkComplete(WGPUQueueWorkDoneStatus status, void *userData)
{
   fprintf(stdout, "Queue work done [%s]\n", queueWorkDoneStatusGetString(status));
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

static void
keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
   AspectRenderContext *ctx = glfwGetWindowUserPointer(window);
   aspectRenderContextHandleKeypress(ctx, key, scancode, action, mods);
}

int
main(int argc, char **argv)
{
   Error **err = NULL;
   AspectScene *scene = aspectSceneNewFromFile("/Users/rpendergraph/CLionProjects/learning/Resources/pyramid.xml", err);
   if (scene == NULL)
   {
      printf("Load scene file failed.");
   }
   int result = glfwInit();
   assert(result);
   glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
   glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
   GLFWwindow *window = glfwCreateWindow(640, 480, "Learn WebGPU", NULL, NULL);
   assert(window);

   AspectPlatformSpecifics *specifics = aspectPlatformSpecificsNewForGLFW(window, err);
   assert(specifics);
   AspectRenderer *renderer = aspectRendererNew((AspectRendererOptions) {}, specifics, err);
   assert(renderer);
   glfwSetWindowUserPointer(window, renderer);
   glfwSetKeyCallback(window, keyCallback);
   AspectRenderContext *ctx = aspectRenderContextNew(renderer, scene, err);
   while (!glfwWindowShouldClose(window))
   {
      glfwPollEvents();
      aspectRenderContextOnUpdate(ctx);
      aspectRenderContextRender(ctx, err);
   }

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

