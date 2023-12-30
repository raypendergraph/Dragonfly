#ifndef ASPECT_PLATFORMSPECIFICS_H
#define ASPECT_PLATFORMSPECIFICS_H

#include <webgpu/webgpu.h>

typedef struct AspectPlatformSpecifics
{
   WGPUSurface surface;
   WGPUInstance instance;
} AspectPlatformSpecifics;

#endif
