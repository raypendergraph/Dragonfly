#include "Camera.h"
#include <Platform/Core.h>

Camera *
aspectCameraNew(HMM_Vec3 position, float phi, float theta, Error **err)
{
   Camera *camera = malloc(sizeof(Camera));
   if (camera == NULL)
   {
      ERROR(err, PFM_ERR_ALLOC_FAILED, "camera");
      return NULL;
   }
   camera->position = position;
   camera->eulers = HMM_V3(0, phi, theta);
   camera->forwards = HMM_V3(0, 0, 0);
   camera->right = HMM_V3(0, 0, 0);
   camera->up = HMM_V3(0, 0, 0);
   return camera;
}

void
aspectCameraDestroy(Camera *camera)
{
   if (camera == NULL)
   {
      return;
   }

   free(camera);
}

void
aspectCameraUpdate(Camera *camera)
{
   camera->forwards = HMM_V3(
      HMM_CosF(HMM_ToRad(camera->eulers.Z)) * HMM_CosF(HMM_ToRad(camera->eulers.Y)),
      HMM_SinF(HMM_ToRad(camera->eulers.Z)) * HMM_CosF(HMM_ToRad(camera->eulers.Y)),
      HMM_SinF(HMM_ToRad(camera->eulers.Y))
   );

   camera->right = HMM_Cross(camera->forwards, HMM_V3(0, 0, 1));
   camera->up = HMM_Cross(camera->right, camera->forwards);
   camera->view = HMM_LookAt_RH(camera->position, HMM_AddV3(camera->position, camera->forwards), camera->up);
}

bool
aspectCameraGetView(Camera *camera, HMM_Mat4 *outView)
{
   if (camera == NULL || outView == NULL)
   {
      return false;
   }
   *outView = camera->view;
   return true;
}