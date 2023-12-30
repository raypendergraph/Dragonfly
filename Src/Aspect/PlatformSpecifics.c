#include "WGPUUtils.h"
#include "PlatformSpecifics.h"
#include <Platform/Core.h>

#include <stdlib.h>

typedef void GLFWwindow;

#if __APPLE__

WGPUSurface newMetalSurface(WGPUInstance instance, GLFWwindow *window);


AspectPlatformSpecifics *
aspectPlatformSpecificsNewForGLFW(GLFWwindow *window, Error **err)
{
   WGPUInstance instance = NULL;
   WGPUSurface surface = NULL;
   AspectPlatformSpecifics *specifics = NULL;

   instance = wgpuCreateInstance(NULL);
   if (instance == NULL)
   {
      REPORT_NULL_FAULT(instance, err);
      goto onFailure;
   }

   surface = newMetalSurface(instance, window);
   if (surface == NULL)
   {
      REPORT_NULL_FAULT(surface, err);
      goto onFailure;
   }
   specifics = malloc(sizeof(*specifics));
   if (specifics == NULL)
   {
      ERROR(err, PFM_ERR_ALLOC_FAILED, "could not allocate the renderer")
      goto onFailure;
   }
   specifics->surface = surface;
   specifics->instance = instance;
   return specifics;

   onFailure:
   if (specifics) free(specifics);
   if (instance) wgpuInstanceRelease(instance);
   if (surface) wgpuSurfaceRelease(surface);
   return NULL;
}

#endif

WGPUSurface aspectNewSurfaceForGLFW(WGPUInstance instance, GLFWwindow *window)
{
#ifdef __APPLE__
   return newMetalSurface(instance, window);
#endif
}

void
aspectPlatformSpecificsDestroy(AspectPlatformSpecifics *specifics)
{
   if (specifics == NULL)
   {
      return;
   }
   wgpuSurfaceRelease(specifics->surface);
   wgpuInstanceRelease(specifics->instance);
   free(specifics);
}
