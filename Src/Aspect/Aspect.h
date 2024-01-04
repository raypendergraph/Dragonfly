//
// Created by Ray Pendergraph on 10/10/23.
//

#ifndef LEARNING_ASPECT_H
#define LEARNING_ASPECT_H

#include "Types.h"
#include <webgpu/webgpu.h>
#include <string.h>

typedef void Material;
typedef void AspectScene;
typedef void AspectMaterial;
typedef void AspectContext;
typedef void AspectRenderer;
typedef void AspectRenderContext;
typedef void AspectPlatformSpecifics;

const char *
featureNameGetString(WGPUFeatureName featureName);

const char *
errorTypeGetString(WGPUErrorType type);

const char *
queueWorkDoneStatusGetString(WGPUQueueWorkDoneStatus status);

const char *
deviceLostReasonGetString(WGPUDeviceLostReason reason);


void
aspectAdapterPrintFeatures(WGPUAdapter adapter);

void
aspectPlatformSpecificsDestroy(AspectPlatformSpecifics *specifics);

AspectPlatformSpecifics *
aspectPlatformSpecificsNewForGLFW(GLFWwindow *window, Error **err);

AspectRenderer *
aspectRendererNew(AspectRendererOptions options, AspectPlatformSpecifics *specifics, Error **err);

WGPUShaderModule
aspectNewWGSLShaderModule(WGPUDevice device, const char *shaderSource, const char *label);


AspectScene *
aspectSceneNewFromFile(char *fileName, Error **err);

bool
aspectIndexBufferCopy(IndexBuffer *src, IndexBuffer *dst, size_t dstOffset, Error **err);

bool
aspectVertexBuffer2dCopy(VertexBuffer2D *src, VertexBuffer2D *dst, size_t dstOffset, Error **err);

bool
aspectSceneBuildGeometry2D(AspectScene *scene, VertexBuffer2D **outVertices, IndexBuffer **outIndices, Error **err);

const char *
aspectVertexBuffer2dToString(VertexBuffer2D *vb);

// Material

Material *
aspectMaterialNewFromFile(WGPUDevice device, char const *path, Error **err);

AspectMaterial *
aspectMaterialNewTest(WGPUDevice device, size_t width, size_t height, Error **err);

bool
aspectMaterialGetData(AspectMaterial *material, uint8_t **data, size_t *size, Error **err);

//bool
//aspectMaterialGetWGPUTexture(AspectMaterial *material, WGPUTexture *texture, Error **err);

bool
aspectMaterialWriteToQueue(AspectContext *ctx, AspectMaterial *material, Error **err);

AspectRenderContext *
aspectRenderContextNew(AspectRenderer *renderer,
                       AspectScene *scene,
//                       VertexBuffer2D const *vertices,
//                       IndexBuffer const *indices,
                       Error **err);

bool
aspectRenderContextRender(AspectRenderContext *ctx, Error **err);

#endif
