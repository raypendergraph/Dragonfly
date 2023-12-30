
#ifndef ASPECT_TYPES_H
#define ASPECT_TYPES_H
/*
This file contains value types that are part of the public API but are also used in the implementation.
These will be mostly value types that are either holders for complex call options or common types.
*/
#include <stdlib.h>


typedef struct AspectRendererOptions
{
} AspectRendererOptions;

typedef union Vertex
{
   struct
   {
      float x;
      float y;
      float z;
      float r;
      float g;
      float b;
      //float a;
   };
   float array[6];
} Vertex;

typedef union Vertex2D
{
   struct
   {
      float x;
      float y;
      float r;
      float g;
      float b;
      //float a;
   };
   float array[5];
} Vertex2D;

typedef struct
{
   size_t count;
   Vertex2D data[];
} VertexBuffer2D;

typedef struct
{
   size_t count;
   Vertex data[];
} VertexBuffer;

typedef struct IndexBuffer
{
   size_t count;
   uint32_t data[];
} IndexBuffer;
#endif //ASPECT_TYPES_H
