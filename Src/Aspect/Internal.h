#ifndef ASPECT_INTERNAL_H
#define ASPECT_INTERNAL_H

#include <GLFW/glfw3.h>
#include <Platform/Core.h>
#include <stdbool.h>
#include <webgpu/webgpu.h>
#include "Types.h"



//typedef struct VertexBuffer2D VertexBuffer2D;
//typedef struct VertexBuffer VertexBuffer;
//typedef struct IndexBuffer IndexBuffer;
//typedef struct Entity Entity;
//typedef struct EntityGroup EntityGroup;
//typedef struct Scene Scene;
//typedef struct GLFWwindow GLFWWwindow;

//TODO slated for deletion when we find out what to do with these

typedef struct AspectEntity
{
   VertexBuffer *vertexBuffer;
   IndexBuffer *indexBuffer;
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
#endif
