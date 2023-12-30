#include "Internal.h"
#include "PlatformSpecifics.h"
#include "Renderer.h"
#include "stdbool.h"
#include <Platform/Core.h>
#include <assert.h>

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


static const char *
errorTypeGetString(WGPUErrorType type)
{
   switch (type)
   {
      case WGPUErrorType_NoError:
         return "NoError";
      case WGPUErrorType_Validation:
         return "Validation";
      case WGPUErrorType_OutOfMemory:
         return "OutOfMemory";
      case WGPUErrorType_Internal:
         return "Internal";
      case WGPUErrorType_Unknown:
         return "Unknown";
      case WGPUErrorType_DeviceLost:
         return "DeviceLost";
      default:
         return "UNKNOWN";
   }
}

static const char *
featureNameGetString(WGPUFeatureName featureName)
{
   switch (featureName)
   {
      case WGPUFeatureName_Undefined:
         return "Undefined";
      case WGPUFeatureName_DepthClipControl:
         return "DepthClipControl";
      case WGPUFeatureName_Depth32FloatStencil8:
         return "Depth32FloatStencil8";
      case WGPUFeatureName_TimestampQuery:
         return "TimestampQuery";
      case WGPUFeatureName_PipelineStatisticsQuery:
         return "PipelineStatisticsQuery";
      case WGPUFeatureName_TextureCompressionBC:
         return "TextureCompressionBC";
      case WGPUFeatureName_TextureCompressionETC2:
         return "TextureCompressionETC2";
      case WGPUFeatureName_TextureCompressionASTC:
         return "TextureCompressionASTC";
      case WGPUFeatureName_IndirectFirstInstance:
         return "IndirectFirstInstance";
      case WGPUFeatureName_ShaderF16:
         return "ShaderF16";
      case WGPUFeatureName_RG11B10UfloatRenderable:
         return "RG11B10UfloatRenderable";
      case WGPUFeatureName_BGRA8UnormStorage:
         return "BGRA8UnormStorage";
      case WGPUFeatureName_Float32Filterable:
         return "Float32Filterable";
      case WGPUFeatureName_DawnInternalUsages:
         return "DawnInternalUsages";
      case WGPUFeatureName_DawnMultiPlanarFormats:
         return "DawnMultiPlanarFormats";
      case WGPUFeatureName_DawnNative:
         return "DawnNative";
      case WGPUFeatureName_ChromiumExperimentalDp4a:
         return "ChromiumExperimentalDp4a";
      case WGPUFeatureName_TimestampQueryInsidePasses:
         return "TimestampQueryInsidePasses";
      case WGPUFeatureName_ImplicitDeviceSynchronization:
         return "ImplicitDeviceSynchronization";
      case WGPUFeatureName_SurfaceCapabilities:
         return "SurfaceCapabilities";
      case WGPUFeatureName_TransientAttachments:
         return "TransientAttachments";
      case WGPUFeatureName_MSAARenderToSingleSampled:
         return "MSAARenderToSingleSampled";
      case WGPUFeatureName_DualSourceBlending:
         return "DualSourceBlending";
      case WGPUFeatureName_D3D11MultithreadProtected:
         return "D3D11MultithreadProtected";
      case WGPUFeatureName_ANGLETextureSharing:
         return "ANGLETextureSharing";
      case WGPUFeatureName_ChromiumExperimentalSubgroups:
         return "ChromiumExperimentalSubgroups";
      case WGPUFeatureName_ChromiumExperimentalSubgroupUniformControlFlow:
         return "ChromiumExperimentalSubgroupUniformControlFlow";
      case WGPUFeatureName_ChromiumExperimentalReadWriteStorageTexture:
         return "ChromiumExperimentalReadWriteStorageTexture";
      case WGPUFeatureName_PixelLocalStorageCoherent:
         return "PixelLocalStorageCoherent";
      case WGPUFeatureName_PixelLocalStorageNonCoherent:
         return "PixelLocalStorageNonCoherent";
      case WGPUFeatureName_Norm16TextureFormats:
         return "Norm16TextureFormats";
      case WGPUFeatureName_MultiPlanarFormatExtendedUsages:
         return "MultiPlanarFormatExtendedUsages";
      case WGPUFeatureName_MultiPlanarFormatP010:
         return "MultiPlanarFormatP010";
      case WGPUFeatureName_HostMappedPointer:
         return "HostMappedPointer";
      case WGPUFeatureName_MultiPlanarRenderTargets:
         return "MultiPlanarRenderTargets";
      case WGPUFeatureName_SharedTextureMemoryVkDedicatedAllocation:
         return "SharedTextureMemoryVkDedicatedAllocation";
      case WGPUFeatureName_SharedTextureMemoryAHardwareBuffer:
         return "SharedTextureMemoryAHardwareBuffer";
      case WGPUFeatureName_SharedTextureMemoryDmaBuf:
         return "SharedTextureMemoryDmaBuf";
      case WGPUFeatureName_SharedTextureMemoryOpaqueFD:
         return "SharedTextureMemoryOpaqueFD";
      case WGPUFeatureName_SharedTextureMemoryZirconHandle:
         return "SharedTextureMemoryZirconHandle";
      case WGPUFeatureName_SharedTextureMemoryDXGISharedHandle:
         return "SharedTextureMemoryDXGISharedHandle";
      case WGPUFeatureName_SharedTextureMemoryD3D11Texture2D:
         return "SharedTextureMemoryD3D11Texture2D";
      case WGPUFeatureName_SharedTextureMemoryIOSurface:
         return "SharedTextureMemoryIOSurface";
      case WGPUFeatureName_SharedTextureMemoryEGLImage:
         return "SharedTextureMemoryEGLImage";
      case WGPUFeatureName_SharedFenceVkSemaphoreOpaqueFD:
         return "SharedFenceVkSemaphoreOpaqueFD";
      case WGPUFeatureName_SharedFenceVkSemaphoreSyncFD:
         return "SharedFenceVkSemaphoreSyncFD";
      case WGPUFeatureName_SharedFenceVkSemaphoreZirconHandle:
         return "SharedFenceVkSemaphoreZirconHandle";
      case WGPUFeatureName_SharedFenceDXGISharedHandle:
         return "SharedFenceDXGISharedHandle";
      case WGPUFeatureName_SharedFenceMTLSharedEvent:
         return "SharedFenceMTLSharedEvent";
      default:
         return "UNKNOWN";
   }
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

static WGPUAdapter
createAdapter(AspectPlatformSpecifics *specifics, Error **err)
{
   AdapterRequest *request = NULL;
   WGPUAdapter adapter = NULL;

   //Apparently required for native?
   const char *allowUnsafeApisToggle = "allow_unsafe_apis";
   request = malloc(sizeof(*request));
   if (request == NULL)
   {
      REPORT_NULL_FAULT(adapterReq, err);
      goto onFailure;
   }
   wgpuInstanceRequestAdapter(specifics->instance,
                              &(WGPURequestAdapterOptions) {
                                 .nextInChain=(WGPUChainedStruct *) &(WGPUDawnTogglesDescriptor) {
                                    .chain={
                                       .sType=WGPUSType_DawnTogglesDescriptor},
                                    .enabledToggleCount=1,
                                    .enabledToggles=&allowUnsafeApisToggle},
                                 .compatibleSurface=specifics->surface},
                              handleRequestAdapterCompleted,
                              request);

   if (request->adapter == NULL)
   {
      ERRORF(err, 0, "WGPU adapter request failed: %d - %s", request->status, request->message);
      goto onFailure;
   }
   adapter = request->adapter;
   free(request);
   return adapter;

   onFailure:
   if (request) free(request);

   return NULL;
}

static WGPUDevice
createDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const *deviceDescriptor, Error **err)
{
   WGPUDevice device = NULL;
   DeviceRequest *request = NULL;

   request = malloc(sizeof(*request));
   if (request == NULL)
   {
      REPORT_NULL_FAULT(deviceReq, err);
      goto onFailure;
   }

   wgpuAdapterRequestDevice(
      adapter,
      deviceDescriptor,
      handleDeviceRequestCompleted,
      request);

   if (request->device == NULL)
   {
      REPORT_NULL_FAULT(deviceReq->device, err);
      goto onFailure;
   }

   wgpuDeviceSetUncapturedErrorCallback(request->device, handleDeviceError, NULL);
   wgpuDeviceSetDeviceLostCallback(request->device, handleDeviceLost, NULL);
   device = request->device;
   free(request);
   return device;

   onFailure:
   if (device) wgpuDeviceRelease(device);
   if (request) free(request);
   return NULL;
}

void
aspectRendererDestroy(AspectRenderer *renderer)
{
   if (renderer == NULL)
   {
      return;
   }
   if (renderer->adapter) wgpuAdapterRelease(renderer->adapter);
   if (renderer->queue) wgpuQueueRelease(renderer->queue);
   if (renderer->device) wgpuDeviceRelease(renderer->device);
   if (renderer->swapChain) wgpuSwapChainRelease(renderer->swapChain);
   free(renderer);
}

AspectRenderer *
aspectRendererNew(AspectRendererOptions options, AspectPlatformSpecifics *specifics, Error **err)
{
   AspectRenderer *renderer = malloc(sizeof(*renderer));
   if (renderer == NULL)
   {
      ERROR(err, PFM_ERR_ALLOC_FAILED, "could not allocate the renderer")
      return NULL;
   }

   if (specifics == NULL)
   {
      REPORT_NULL_FAULT(specifics, err);
      goto onFailure;
   }
   assert(specifics->surface);
   assert(specifics->instance);

   renderer->adapter = createAdapter(specifics, err);
   if (renderer->adapter == NULL)
   {
      REPORT_NULL_FAULT(renderer->adapter, err);
      goto onFailure;
   }

   WGPUSupportedLimits supportedLimits = {};
   if (!wgpuAdapterGetLimits(renderer->adapter, &supportedLimits))
   {
      ERROR(err, 0, "could not get the adapter limits");
      goto onFailure;
   }

   WGPUDeviceDescriptor dd = {
      .requiredLimits=&(WGPURequiredLimits) {
         .limits=(WGPULimits) {
            .maxBufferSize=6 * 6 * sizeof(float), // six 2D data
            .maxInterStageShaderComponents=3,
            .maxVertexAttributes=2,
            .maxVertexBufferArrayStride=5 * sizeof(float),
            .maxVertexBuffers=1,
            .maxBindGroups=1,
            .maxUniformBuffersPerShaderStage=1,
            .maxUniformBufferBindingSize= 16 * 4,
            .minStorageBufferOffsetAlignment=supportedLimits.limits.minStorageBufferOffsetAlignment,
            .minUniformBufferOffsetAlignment=supportedLimits.limits.minStorageBufferOffsetAlignment}}};

   renderer->device = createDevice(renderer->adapter, &dd, err);
   if (renderer->device == NULL)
   {
      REPORT_NULL_FAULT(renderer->device, err);
      goto onFailure;
   }

   renderer->queue = wgpuDeviceGetQueue(renderer->device);
   if (renderer->queue == NULL)
   {
      REPORT_NULL_FAULT(queue, err);
      goto onFailure;
   }

   wgpuQueueOnSubmittedWorkDone(renderer->queue, 0U, handleQueueWorkComplete, NULL);

   // This is not implemented yet in Dawn
   //WGPUTextureFormat swapChainFormat=wgpuSurfaceGetPreferredFormat(surface, adapter);
   //swapChainDesc.format=swapChainFormat;
   renderer->swapChain = wgpuDeviceCreateSwapChain(
      renderer->device,
      specifics->surface,
      &(WGPUSwapChainDescriptor) {
         .width=640,
         .height=480,
         .format=WGPUTextureFormat_BGRA8Unorm,
         .usage=WGPUTextureUsage_RenderAttachment,
         .presentMode=WGPUPresentMode_Fifo
      });
   if (renderer->swapChain == NULL)
   {
      REPORT_NULL_FAULT(swapChain, err);
      goto onFailure;
   }

   return renderer;

   onFailure:
   aspectRendererDestroy(renderer);
   return NULL;
}
