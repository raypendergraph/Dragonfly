#include "Material.h"
#include "Renderer.h"
#include <Platform/Codec.h>
#include <Platform/IO.h>
#include <assert.h>
#include <stdlib.h>
#include <webgpu/webgpu.h>

bool
materialGetWGPUTexture(AspectMaterial *material, WGPUTexture *texture, Error **err)
{
   if (material == NULL)
   {
      REPORT_NULL_FAULT(material, err);
      return false;
   }
   if (texture == NULL)
   {
      REPORT_NULL_FAULT(texture, err);
      return false;
   }
   *texture = material->texture;
   return true;
}

void aspectAspectMaterialDestroy(AspectMaterial *mat)
{
   if (mat == NULL)
   {
      return;
   }
   if (mat->texture)
   {
      wgpuTextureDestroy(mat->texture);
   }
   if (mat->textureView)
   {
      wgpuTextureViewRelease(mat->textureView);
   }
   if (mat->sampler)
   {
      wgpuSamplerRelease(mat->sampler);
   }
   if (mat->bindGroup)
   {
      wgpuBindGroupRelease(mat->bindGroup);
   }
}

bool
aspectAspectMaterialGetData(AspectMaterial *material, uint8_t **data, size_t *size, Error **err)
{
   assert(material != NULL && material->png != NULL);
   if (!codecDecodedPNGGetData(material->png, data, size, err))
   {
      return false;
   }
   return true;
}

AspectMaterial *
aspectMaterialNewFromBuffer(WGPUDevice device, IOStaticBuffer const *buffer, Error **err)
{

   AspectMaterial *material = malloc(sizeof(AspectMaterial));
   if (material == NULL)
   {
      ERROR(err, PFM_ERR_ALLOC_FAILED, "material");
      return NULL;
   }

   material->png = codecDecodedPNGNew(buffer, err);
   if (material->png == NULL)
   {
      goto onFailure;
   }
   material->texture = wgpuDeviceCreateTexture(device, &(WGPUTextureDescriptor) {
      .format=WGPUTextureFormat_RGBA8Unorm,
      .dimension=WGPUTextureDimension_2D,
      .size = (WGPUExtent3D) {
         .width=material->png->width,
         .height=material->png->height,
         .depthOrArrayLayers=1
      },
      .mipLevelCount=1,
      .sampleCount=1,
      .label="hot damn",
      .usage=WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst | WGPUTextureUsage_RenderAttachment
   });
   if (material->texture == NULL)
   {
      REPORT_NULL_FAULT(material->texture, err);
      goto onFailure;
   }


   material->textureView = wgpuTextureCreateView(material->texture, &(WGPUTextureViewDescriptor) {
      .format=WGPUTextureFormat_RGBA8Unorm,
      .dimension=WGPUTextureViewDimension_2D,
      .aspect=WGPUTextureAspect_All,
      .mipLevelCount=1,
      .arrayLayerCount=1
   });
   if (material->textureView == NULL)
   {
      REPORT_NULL_FAULT(material->textureView, err);
      goto onFailure;
   }


   material->sampler = wgpuDeviceCreateSampler(device, &(WGPUSamplerDescriptor) {
      .addressModeU=WGPUAddressMode_Repeat,
      .addressModeV=WGPUAddressMode_Repeat,
      .magFilter=WGPUFilterMode_Linear,
      .minFilter=WGPUFilterMode_Nearest,
      .mipmapFilter=WGPUMipmapFilterMode_Nearest,
      .maxAnisotropy=1,
   });

   if (material->sampler == NULL)
   {
      REPORT_NULL_FAULT(material->sampler, err);
      goto onFailure;
   }

   return material;
//
//   material->bindGroup = wgpuDeviceCreateBindGroup(device, &(WGPUBindGroupDescriptor) {
//      .layout=layout,
//      .entryCount=2,
//      .entries=(WGPUBindGroupEntry[]) {
//         {
//            .binding=0,
//            .textureView=material->textureView
//         },
//         {
//            .binding=1,
//            .sampler=material->sampler
//         }
//      }
//   });
//   if (material->bindGroup == NULL)
//   {
//      REPORT_NULL_FAULT(material->bindGroup, err);
//      goto onFailure;
//   }
//   return material;

   onFailure:
   aspectAspectMaterialDestroy(material);
   return false;
}

AspectMaterial *
aspectMaterialNewFromFile(WGPUDevice device, char const *path, Error **err)
{
   FILE *stream = NULL;
   IOStaticBuffer *buffer = NULL;
   AspectMaterial *material = malloc(sizeof(AspectMaterial));
   if (material == NULL)
   {
      REPORT_NULL_FAULT(material, err);
      goto onFailure;
   }

   stream = fopen(path, "rb");
   if (stream == NULL)
   {
      ERRORF(err, PFM_ERR_IO_FAULT, "could not open %s", path);
      goto onFailure;
   }

   buffer = ioStaticBufferNewFromFileContents(stream, err);
   if (buffer == NULL)
   {
      goto onFailure;
   }

   AspectMaterial *out = aspectMaterialNewFromBuffer(device, buffer, err);
   if (out == NULL)
   {
      goto onFailure;
   }

   return out;

   onFailure:
   if (buffer)
   {
      free(buffer);
   }

   if (stream)
   {
      fclose(stream);
   }

   return false;
}

bool
aspectMaterialWriteToQueue(AspectRenderer *ctx, AspectMaterial *material, Error **err)
{
   if (ctx == NULL)
   {
      REPORT_NULL_FAULT(ctx, err);
      return false;
   }

   if (material == NULL)
   {
      REPORT_NULL_FAULT(ctx, err);
      return false;
   }

   WGPUImageCopyTexture destination = {
      .texture=material->texture,
   };

   CodecDecodedPNG *png = material->png;

   WGPUTextureDataLayout source = {
      .bytesPerRow = 4 * png->width,
      .rowsPerImage = png->height
   };

   uint8_t *textureData;
   size_t textureSize;
   if (!codecDecodedPNGGetData(material->png, &textureData, &textureSize, err))
   {
      return false;
   }

   wgpuQueueWriteTexture(ctx->queue,
                         &destination,
                         textureData,
                         textureSize,
                         &source,
                         &(WGPUExtent3D) {.width=png->width, .height=png->height});
   return true;
}
