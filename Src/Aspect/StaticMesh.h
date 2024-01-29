#ifndef ASPECT_MESH_H
#define ASPECT_MESH_H

#include "Types.h"
#include <Platform/Core.h>
#include <assert.h>
#include "Behavior.h"

struct Behavior;

struct StaticMesh
{
   struct Behavior behavior;
   VertexBuffer *vertices;
   IndexBuffer *indices;

};

void
aspectMeshBehaviorDestroy(struct StaticMesh *mesh);

struct StaticMesh *
aspectMeshBehaviorNew(Error **err);

#endif