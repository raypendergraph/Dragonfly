#ifndef ASPECT_TRANSFORM_H
#define ASPECT_TRANSFORM_H

typedef struct AspectTransform
{
   HMM_Quat rotation;
   HMM_Vec3 position;
   HMM_Vec3 scale;
} AspectTransform;
#endif
