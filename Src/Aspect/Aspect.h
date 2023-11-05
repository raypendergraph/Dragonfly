//
// Created by Ray Pendergraph on 10/10/23.
//

#ifndef LEARNING_ASPECT_H
#define LEARNING_ASPECT_H

#include <webgpu/webgpu.h>
#include <string.h>

typedef struct GLFWwindow GLFWWwindow;

const char *
aspectFeatureNameGetString(WGPUFeatureName featureName);

const char *
aspectErrorTypeGetString(WGPUErrorType type);

const char *
aspectQueueWorkDoneStatusGetString(WGPUQueueWorkDoneStatus status);

const char *
aspectDeviceLostReasonGetString(WGPUDeviceLostReason reason);

void
aspectAdapterPrintFeatures(WGPUAdapter adapter);

WGPUSurface
aspectNewSurfaceForGLFW(WGPUInstance instance, struct GLFWwindow *window);

WGPUShaderModule
aspectNewWGSLShaderModule(WGPUDevice device, const char *shaderSource, const char *label);


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

struct VertexBuffer2D
{
   size_t count;
   Vertex2D data[];
};

struct IndexBuffer
{
   size_t count;
   uint32_t data[];
};

typedef struct VertexBuffer2D VertexBuffer2D;

typedef struct IndexBuffer IndexBuffer;

typedef struct Entity Entity;

typedef struct EntityGroup EntityGroup;

typedef struct Scene Scene;

Scene *
aspectSceneCreateFromFile(char *fileName, Error **err);

bool
aspectIndexBufferCopy(IndexBuffer *src, IndexBuffer *dst, size_t dstOffset, Error **err);

bool
aspectVertexBuffer2dCopy(VertexBuffer2D *src, VertexBuffer2D *dst, size_t dstOffset, Error **err);

bool
aspectSceneBuildGeometry2D(Scene *scene, VertexBuffer2D **outVertices, IndexBuffer **outIndices, Error **err);

const char *
aspectVertexBuffer2dToString(VertexBuffer2D *vb);

#endif
