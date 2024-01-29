#ifndef ASPECT_SCENE_NODE_ITERATOR_H
#define ASPECT_SCENE_NODE_ITERATOR_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

struct SceneNode;
typedef struct StackFrame
{
   size_t index;
   size_t count;
   struct SceneNode *array;

} StackFrame;

struct SceneNodeIterator
{
   int16_t stackPtr;
   StackFrame stack[];
};


bool
aspectSceneNodeIteratorIsDone(struct SceneNodeIterator *it);

struct SceneNode *
aspectSceneNodeIteratorNext(struct SceneNodeIterator *it);

void
aspectSceneNodeIteratorDestroy(struct SceneNodeIterator *it);

#endif
