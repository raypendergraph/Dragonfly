#ifndef ASPECT_SCENE_H
#define ASPECT_SCENE_H

#include <stdbool.h>
#include "Types.h"

typedef struct AspectEntity
{
} AspectEntity;

typedef struct
{
   size_t size;
   AspectEntity data[];
} AspectEntityGroup;

typedef struct
{
   AspectEntityGroup *entities;
} AspectScene;

bool
sceneBuildGeometry2D(AspectScene *scene, VertexBuffer2D **outVertices, IndexBuffer **outIndices, Error **err);

#endif
