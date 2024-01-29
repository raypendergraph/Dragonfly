
#include "Behavior.h"
#include "SceneNode.h"
#include <HandmadeMath.h>
#include <assert.h>

static bool
rotateNode(struct SceneNode *node, struct RotateBehavior *behavior)
{
   assert(node);
   assert(behavior);
   return aspectSceneNodeRotateAroundAxes(node, behavior->eulerAngles);
}

bool
aspectBehaviorApplyToSceneNode(struct Behavior *behavior, struct SceneNode *node)
{
   switch (behavior->kind)
   {
      case ASPECT_BEHAVIOR_TYPE_ROTATE:
         return rotateNode(node, (struct RotateBehavior *) behavior);
      default:
         assert(false);
         return false;
   }
}
