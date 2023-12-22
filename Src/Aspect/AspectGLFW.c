#include "Types.h"

#if __APPLE__

AspectRenderer *
aspectRendererNewForGLFW(AspectRendererOptions options, GLFWwindow *window, Error **err)
{
   AspectRenderer *renderer = NULL;
   renderer = malloc(sizeof(*renderer));
   if (renderer == NULL)
   {
      ERROR(err, PFM_ERR_ALLOC_FAILED, "could not allocate the renderer")
      goto onFailure;
   }

   onFailure:
   if (renderer) aspectRendererDestroy(renderer);

}

#endif
