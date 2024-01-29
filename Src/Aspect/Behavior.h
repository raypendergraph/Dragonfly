#ifndef ASPECT_BEHAVIOR_H
#define ASPECT_BEHAVIOR_H

#include <uuid/uuid.h>
#include <stdlib.h>
#include <Platform/Core.h>
#include <HandmadeMath.h>

struct SceneNode;

typedef enum
{
   ASPECT_BEHAVIOR_TYPE_UNDEFINED = 0,
   ASPECT_BEHAVIOR_TYPE_MESH = 1,
   ASPECT_BEHAVIOR_TYPE_ROTATE = 2,
   ASPECT_BEHAVIOR_TYPE_FORCE32 = 0x7FFFFFFF
} AspectBehaviorKind;

struct Behavior
{
   AspectBehaviorKind kind;
};

struct RotateBehavior
{
   struct Behavior behavior;
   HMM_Vec3 eulerAngles;
} AspectRotateBehavior;

/**
 * Executes the behavior on a given scene node.
 * @param node The particular scene node
 * @param behavior  The particular behavior.
 * @return true if successful, false otherwise. Please see documentation for "Error Handling and the Scene"
 */
bool
aspectBehaviorApplyToSceneNode(struct Behavior *behavior, struct SceneNode *node);

#endif
