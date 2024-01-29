#ifndef ASPECT_CAMERA_H
#define ASPECT_CAMERA_H

#include "HandmadeMath.h"
#include <Platform/Core.h>

typedef struct Camera
{
   HMM_Vec3 position;
   HMM_Vec3 eulers;
   HMM_Mat4 view;
   HMM_Vec3 forwards;
   HMM_Vec3 right;
   HMM_Vec3 up;
} Camera;

Camera *
aspectCameraNew(HMM_Vec3 position, float phi, float theta, Error **err);

void
aspectCameraDestroy(Camera *camera);

void
aspectCameraUpdate(Camera *camera);

bool
aspectCameraGetView(Camera *camera, HMM_Mat4 *outView);

#endif
