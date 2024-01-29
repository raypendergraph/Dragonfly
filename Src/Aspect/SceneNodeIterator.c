#include "SceneNodeIterator.h"
#include "SceneNode.h"

bool
aspectSceneNodeIteratorIsDone(struct SceneNodeIterator *it)
{
   return it->stackPtr < 0;
}

struct SceneNode *
aspectSceneNodeIteratorNext(struct SceneNodeIterator *it)
{
   if (aspectSceneNodeIteratorIsDone(it))
   {
      return false;
   }
   StackFrame *frame = &it->stack[it->stackPtr];
   struct SceneNode *out = &frame->array[frame->index];

   // If we are not the last node in the frame
   if (frame->index < frame->count - 1)
   {
      frame->index++;
      return out;
   }

   // We are the last node decrement the stack and return out. Done looks for negative stack.
   it->stackPtr--;
   return out;
}

void aspectSceneNodeIteratorDestroy(struct SceneNodeIterator *it)
{
   if (it == NULL) return;
   free(it);
}