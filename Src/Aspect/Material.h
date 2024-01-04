#ifndef ASPECT_MATERIAL_H
#define ASPECT_MATERIAL_H

#include "WGPUUtils.h"
#include <Platform/Codec.h>

typedef struct AspectMaterial
{
   WGPUTexture texture;
   WGPUTextureView textureView;
   WGPUSampler sampler;
   WGPUBindGroup bindGroup;
   CodecDecodedPNG *png;
} AspectMaterial;

bool
materialGetWGPUTexture(AspectMaterial *material, WGPUTexture *texture, Error **err);

AspectMaterial *
aspectMaterialNewTest(WGPUDevice device, size_t width, size_t height, Error **err);

#endif
