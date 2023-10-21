#include <stdio.h>
#include <webgpu/webgpu.h>
#include <GLFW/glfw3.h>

const char *aspectQueueWorkDoneStatusGetString(WGPUQueueWorkDoneStatus status)
{
    switch (status)
    {
        case WGPUQueueWorkDoneStatus_Success:
            return "Success";
        case WGPUQueueWorkDoneStatus_Error:
            return "Error";
        case WGPUQueueWorkDoneStatus_Unknown:
            return "Unknown";
        case WGPUQueueWorkDoneStatus_DeviceLost:
            return "DeviceLost";
        default:
            return "UNKNOWN";
    }
}

const char *aspectDeviceLostReasonGetString(WGPUDeviceLostReason reason)
{
    switch (reason)
    {

        case WGPUDeviceLostReason_Undefined:
            return "Undefined";
        case WGPUDeviceLostReason_Destroyed:
            return "Destroyed";
        default:
            return "UNKNOWN";
    }
}

const char *aspectErrorTypeGetString(WGPUErrorType type)
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

const char *aspectFeatureNameGetString(WGPUFeatureName featureName)
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

void aspectAdapterPrintFeatures(WGPUAdapter adapter)
{
    size_t featureCount = wgpuAdapterEnumerateFeatures(adapter, NULL);
    WGPUFeatureName features[featureCount];
    wgpuAdapterEnumerateFeatures(adapter, features);

    printf("Adapter features:\n");
    for (size_t i = 0; i < featureCount; i += 1)
    {
        printf("%s\n", aspectFeatureNameGetString(features[i]));
    }
}

WGPUSurface newMetalSurface(WGPUInstance instance, GLFWwindow *window);

WGPUSurface aspectNewSurfaceForGLFW(WGPUInstance instance, GLFWwindow *window)
{
#ifdef __APPLE__
    return newMetalSurface(instance, window);
#endif
}

WGPUShaderModule
aspectNewWGSLShaderModule(WGPUDevice device, const char *shaderSource, const char *label)
{
    const WGPUShaderModuleDescriptor smd = {
       .label=label,
       .nextInChain = &(WGPUChainedStruct) {
          (const WGPUChainedStruct *) &(WGPUShaderModuleWGSLDescriptor) {
             .chain=&(WGPUChainedStruct) {
                .sType=WGPUSType_ShaderModuleWGSLDescriptor},
             .code=shaderSource}}};

    return wgpuDeviceCreateShaderModule(device, &smd);
}