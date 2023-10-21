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

    WGPUAdapter adapter = createAdapter(instance, &(WGPURequestAdapterOptions) {
       .compatibleSurface=surface
    });
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







// typedef struct
// {
//     WGPUAdapter adapter;
//     WGPUDevice device;
//     WGPUQueue queue;
// } DrawingContext;

// void handleUncapturedCallback(WGPUErrorType type, char const *message, void *userdata)
// {
// }

// void requestAdapterCallback(WGPURequestAdapterStatus status, WGPUAdapter adapter, char const *message, void *userdata)
// {
//     if (status == WGPURequestAdapterStatus_Success)
//     {
//         DrawingContext *ctx=(DrawingContext *)userdata;
//         ctx->adapter=adapter;
//     }
//     else
//     {
//         printf("request_adapter status=%#.8x message=%s\n", status, message);
//     }
// }

// void requestDeviceCallback(WGPURequestDeviceStatus status, WGPUDevice device, char const *message, void *userdata)
// {
//     if (status == WGPURequestAdapterStatus_Success)
//     {
//         DrawingContext *ctx=(DrawingContext *)userdata;
//         ctx->device=device;
//     }
//     else
//     {
//         printf("request_device status=%#.8x message=%s\n", status, message);
//     }
// }
// int main(int argc, char *argv[])
// {
//     WGPUInstance instance=wgpuCreateInstance(NULL);
//     WGPURequestAdapterOptions options=(WGPURequestAdapterOptions){

//     };
//     DrawingContext context={};
//     wgpuInstanceRequestAdapter(instance, &options, requestAdapterCallback, &context);
//     assert(context.adapter);
//     wgpuAdapterRequestDevice(context.adapter,
//                              &(WGPUDeviceDescriptor){},
//                              requestDeviceCallback,
//                              &context);
//     assert(context.device);
//     wgpuDeviceSetUncapturedErrorCallback(context.device, handleUncapturedCallback, &context);

//     WGPUBindGroupLayout bindGroupLayout=wgpuDeviceCreateBindGroupLayout(context.device, &(WGPUBindGroupLayoutDescriptor){});
//     assert(bindGroupLayout);

//     WGPUPipelineLayout pipelineLayout =
//         wgpuDeviceCreatePipelineLayout(
//             context.device,
//             &(WGPUPipelineLayoutDescriptor){
//                 .bindGroupLayoutCount=1,
//                 .bindGroupLayouts={bindGroupLayout},
//             });

//     assert(pipelineLayout);

//     WGPUShaderModule fragmentModule=createShaderModule(context.device, fragmentShader);
//     WGPUShaderModule vertexModule=createShaderModule(context.device, vertexShader);

//     WGPURenderPipeline renderPipeline =
//         wgpuDeviceCreateRenderPipeline(
//             context.device,
//             &(WGPURenderPipelineDescriptor){
//                 .fragment=&(WGPUFragmentState){
//                     .entryPoint="fs_main",
//                     .module=fragmentModule},
//                 .layout=pipelineLayout,
//                 .primitive={.topology=WGPUPrimitiveTopology_TriangleList},
//                 .vertex={
//                     .entryPoint="vs_main",
//                     .module=fragmentModule},
//             });
//     assert(renderPipeline);

//     WGPUCommandEncoder commandEncoder=wgpuDeviceCreateCommandEncoder( context.device, NULL);
//     assert(commandEncoder);

//     // WGPUBindGroup bindGroup=wgpuDeviceCreateBindGroup(context.device, &(WGPUBindGroupDescriptor){
//     //     .layout=bindGroupLayout
//     // });
// }

// WGPURenderPipelineDescriptor createRenderPipelineDescriptor(WGPUDevice device)
// {
//     WGPUBindGroupLayout bindGroupLayout =
//         wgpuDeviceCreateBindGroupLayout(
//             device,
//             &(WGPUBindGroupDescriptor){
//                 .entryCount=0});

//     WGPUBindGroup bindGroup =
//         wgpuDeviceCreateBindGroup(
//             device,
//             &(WGPUBindGroupDescriptor){
//                 .label="my_bind_group",
//                 .layout=&bindGroupLayout,
//                 .entryCount=0});

//     WGPUPipelineLayout pipelineLayout =
//         wgpuDeviceCreatePipelineLayout(
//             device,
//             &(WGPUPipelineLayoutDescriptor){
//                 .bindGroupLayouts=&(WGPUBindGroupLayout[]){bindGroup},
//                 .bindGroupLayoutCount=1});

//     WGPUShaderModule vertexShaderModule=createShaderModule(device, vertexShader);
//     WGPUShaderModule fragmentShaderModule=createShaderModule(device, fragmentShader);

//     return (WGPURenderPipelineDescriptor){
//         .label="renderPipeline",
//         .layout=pipelineLayout,
//         .vertex =
//             (const WGPUVertexState){
//                 .module=vertexShaderModule,
//                 .entryPoint="vs_main",
//                 .bufferCount=1,
//                 .buffers=(WGPUVertexBufferLayout[]){
//                     {.arrayStride=20,
//                      .attributeCount=2,
//                      .attributes=(WGPUVertexAttribute[]){
//                          {.shaderLocation=0,
//                           .format=WGPUVertexFormat_Float32x2,
//                           .offset=0},
//                          {.shaderLocation=1,
//                           .format=WGPUVertexFormat_Float32x3,
//                           .offset=8}}}}},
//         .fragment=&(const WGPUFragmentState){
//             .module=fragmentShaderModule,
//             .entryPoint="fs_main",
//             .targetCount=1,
//             .targets=(const WGPUColorTargetState[]){
//                 {.format=WGPUTextureFormat_BGRA8Unorm},
//             },

//         },
//         .primitive=(const WGPUPrimitiveState){
//             .topology=WGPUPrimitiveTopology_TriangleList,
//         }};
// }



// WGPUSurfaceCapabilities createSurfaceCapabilities(WGPUInstance instance)
// {
//     WGPUSurface surface=wgpuInstanceCreateSurface(
//         instance,
//         &(const WGPUSurfaceDescriptor){
//             .nextInChain=(const WGPUChainedStruct *)&(const WGPUSurfaceDescriptorFromMetalLayer){
//                 .chain =
//                     (const WGPUChainedStruct){
//                         .sType=WGPUSType_SurfaceDescriptorFromMetalLayer,
//                     },
//                 .layer=layer,
//             },
//         });

//     WGPUSurfaceCapabilities surfaceCapabilities={};
//     wgpuSurfaceGetCapabilities(surface, context.adapter, &surfaceCapabilities);
//     wgpuShaderModuleGetCompilationInfo(shader_module,
//                                        wgpu_compilation_info_callback, NULL);
// }
// typedef struct
// {
//     size_t size;
//     char data[]; // Flexible array member
// } ByteArray;

// void writeTexture(WGPUQueue queue, WGPUTextureDescriptor textureDescriptor)
// {
//     WGPUDevice device;
//     ByteArray *arr=createImage(textureDescriptor);
//     assert(arr);
//     WGPUTexture texture=wgpuDeviceCreateTexture(device, &textureDescriptor);
//     assert(texture);
//     WGPUImageCopyTexture destination={
//         .texture=texture,
//         .mipLevel=0,
//         .origin={0, 0, 0},
//         .aspect=WGPUTextureAspect_All};

//     WGPUTextureDataLayout source={
//         .offset=0,
//         .bytesPerRow=4,
//         .rowsPerImage=textureDescriptor.size.height};

//     wgpuQueueWriteTexture(queue, &destination, arr->data, arr->size, &source, &textureDescriptor.size);

//     WGPUBindGroupLayoutEntry bindGroupLayoutEntries[]={
//         {},
//         {}
//     }
// }

// ByteArray *createImage(WGPUTextureDescriptor descriptor)
// {
//     size_t width=descriptor.size.width;
//     size_t height=descriptor.size.height;
//     size_t dataSize=4 * width * height;

//     // Allocate memory for the struct + the data
//     ByteArray *result=malloc(sizeof(ByteArray) + dataSize);
//     if (!result)
//     {
//         // Handle error, potentially return NULL
//         return NULL;
//     }

//     result->size=dataSize;
//     for (size_t i=0; i < width; ++i)
//     {
//         for (size_t j=0; j < height; ++j)
//         {
//             char *p=&result->data[4 * (j * width + i)];
//             p[0]=(char)i; // r
//             p[1]=(char)j; // g
//             p[2]=128;     // b
//             p[3]=255;     // a
//         }
//     }
//     return result;
// }