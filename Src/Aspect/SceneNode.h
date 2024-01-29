
#ifndef ASPECT_SCENE_NODE_H
#define ASPECT_SCENE_NODE_H

#include "Actor.h"
#include "Transform.h"
#include "HandmadeMath.h"

struct SceneNode
{
   HMM_Quat rotation;
   HMM_Vec3 position;
   HMM_Vec3 scale;

   char *label;
   struct Actor *actor;
   size_t childCount;
   struct SceneNode *children;
};

bool
aspectSceneNodeRotateAroundAxes(struct SceneNode *node, HMM_Vec3 eulerAngles);

void
aspectSceneNodeOnUpdate(struct SceneNode *node);

HMM_Mat4
aspectSceneNodeCalculateModel(struct SceneNode const *node);

#endif