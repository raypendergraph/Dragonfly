#include <HandmadeMath.h>
#include <stdbool.h>
#include "Actor.h"
#include "SceneNode.h"
#include <assert.h>

inline bool
aspectSceneNodeRotateAroundAxes(struct SceneNode *node, HMM_Vec3 eulerAngles)
{
   if (node == NULL) return false;

   HMM_Quat rotX = HMM_QFromAxisAngle_RH(HMM_V3(1.0f, 0.0f, 0.0f), eulerAngles.X);
   HMM_Quat rotY = HMM_QFromAxisAngle_RH(HMM_V3(0.0f, 1.0f, 0.0f), eulerAngles.Y);
   HMM_Quat rotZ = HMM_QFromAxisAngle_RH(HMM_V3(0.0f, 0.0f, 1.0f), eulerAngles.Z);

   HMM_Quat combinedRotation = HMM_MulQ(rotZ, HMM_MulQ(rotY, rotX));
   node->rotation = HMM_MulQ(node->rotation, combinedRotation);
   return true;
}

void
aspectSceneNodeOnUpdate(struct SceneNode *node)
{
   struct Actor *actor = node->actor;
   assert(actor);
   for (size_t i = 0; i < actor->behaviorCount; i++)
   {
      aspectBehaviorApplyToSceneNode(actor->behaviors, node);
   }
}

inline HMM_Mat4
aspectSceneNodeCalculateModel(struct SceneNode const *node)
{
   assert(node);
   if (node == NULL) return HMM_M4();

   HMM_Mat4 scale = HMM_Scale(node->scale);
   HMM_Mat4 rotation = HMM_QToM4(node->rotation);
   HMM_Mat4 position = HMM_Translate(node->position);
   return HMM_MulM4(position, HMM_MulM4(rotation, scale));
}
