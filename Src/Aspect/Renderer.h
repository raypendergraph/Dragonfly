#ifndef ASPECT_RENDERER_H
#define ASPECT_RENDERER_H

#include "WGPUUtils.h"


typedef struct AspectRenderer
{
   WGPUInstance instance;
   WGPUSurface surface;
   WGPUDevice device;
   WGPUAdapter adapter;
   WGPUSwapChain swapChain;
   WGPUQueue queue;
//   WGPUBuffer indexBuffer;
//   WGPUBuffer vertexBuffer;
//   WGPUBuffer uniformBuffer;
} AspectRenderer;
#endif
