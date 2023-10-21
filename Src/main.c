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
} RenderContext;


WGPURenderPipeline
createRenderPipeline(WGPUDevice device, const char *shaderSource)
{
    WGPUBindGroupLayout bindGroupLayout =
       wgpuDeviceCreateBindGroupLayout(
          device,
          &(WGPUBindGroupLayoutDescriptor) {});

    WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(
       device,
       &(WGPUPipelineLayoutDescriptor) {
          .bindGroupLayouts=(WGPUBindGroupLayout[]) {bindGroupLayout},
          .bindGroupLayoutCount=0});
    assert(pipelineLayout);

    WGPUShaderModule shaderModule =
       aspectNewWGSLShaderModule(device, shaderSource, "shaderModule");
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
          .entryPoint="vs_main"},
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

    return wgpuDeviceCreateRenderPipeline(device, &d);
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

static void
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
handleDeviceUncapturedError(WGPUErrorType type, char const *message, void *userData)
{
    printf("Device error [%s]: %s\n", aspectErrorTypeGetString(type), message);
}

static void
handleQueueWorkComplete(WGPUQueueWorkDoneStatus status, void *userData)
{
    fprintf(stdout, "Queue work done [%s]\n", aspectQueueWorkDoneStatusGetString(status));
}

WGPUDevice
createDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const *options)
{
    DeviceRequest *deviceReq = malloc(sizeof(*deviceReq));
    wgpuAdapterRequestDevice(
       adapter,
       options,
       handleDeviceRequestCompleted,
       deviceReq);

    if (deviceReq->device == NULL)
    {
        fprintf(stderr, "Request device failed: %s", deviceReq->message);
        assert(false);
    }

    wgpuDeviceSetUncapturedErrorCallback(deviceReq->device, handleDeviceUncapturedError, NULL);
    wgpuDeviceSetDeviceLostCallback(deviceReq->device, handleDeviceLost, NULL);
    return deviceReq->device;
}

WGPUAdapter
createAdapter(WGPUInstance instance, WGPURequestAdapterOptions const *options)
{
    AdapterRequest *adapterReq = malloc(sizeof(*adapterReq));
    wgpuInstanceRequestAdapter(
       instance /* equivalent of navigator.gpu */,
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

    const char *const toggles[] = {"dump_shaders", "disable_symbol_renaming"};
    WGPUDawnTogglesDescriptor dawnToggles = {
       .chain={
          .sType=WGPUSType_DawnTogglesDescriptor
       },
       .enabledToggleCount=2,
       .enabledToggles=toggles
    };

    WGPUDevice device = createDevice(adapter, &(WGPUDeviceDescriptor) {
//       .nextInChain = (WGPUChainedStruct *) &dawnToggles,
       .label="WGPU Device",
       .defaultQueue.label="Default queue"
    });
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
    int result = glfwInit();
    assert(result);
    Error *err = NULL;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(640, 480, "Learn WebGPU", NULL, NULL);
    assert(window);
    RenderContext *ctx = initializeWGPU(window);
    assert(ctx);
    const char *shaderSource = pfmLoadContentAtPathAsString(
       "/Users/rpendergraph/CLionProjects/learning/Resources/shader.wgsl", &err);
    if (shaderSource == NULL)
    {
        printf("Error reading shader: %s", pfmErrorToString(err));
        exit(-1);
    }
    WGPURenderPipeline pipeline = createRenderPipeline(ctx->device, shaderSource);
    assert(pipeline);

    while (!glfwWindowShouldClose(window))
    {
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
        wgpuRenderPassEncoderDraw(renderPassEncoder, 3, 1, 0, 0);
        wgpuRenderPassEncoderEnd(renderPassEncoder);
        wgpuTextureViewRelease(currentTexture);

        WGPUCommandBufferDescriptor cmdBufferDesc = {};
        cmdBufferDesc.nextInChain = NULL;
        cmdBufferDesc.label = "Command buffer";
        WGPUCommandBuffer command = wgpuCommandEncoderFinish(commandEncoder,
                                                             &cmdBufferDesc);

        wgpuQueueSubmit(ctx->queue, 1, &command);

        wgpuSwapChainPresent(ctx->swapChain);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
