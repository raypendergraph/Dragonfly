#ifndef ASPECT_NODE_H
#define ASPECT_NODE_H

#include "Material.h"
#include "HandmadeMath.h"

typedef struct AspectNode
{
   HMM_Mat4 transform;
} AspectNode;

typedef struct AspectGraph
{
   AspectNode root;
} AspectGraph;

#endif
