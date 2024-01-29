
#ifndef ASPECT_RENDERCONTEXT_H
#define ASPECT_RENDERCONTEXT_H

#include "Renderer.h"
#include "Camera.h"
#include "Types.h"
#include "Material.h"
#include "SceneNode.h"
#include "SceneNodeIterator.h"

typedef struct AspectRenderContext
{
   uint8_t sceneDepth;
   float t;
   Camera *camera;
   VertexBuffer2D *vertices;
   IndexBuffer *indices;
   AspectRenderer *renderer;
   WGPUBuffer vertexBuffer;
   WGPUBuffer indexBuffer;
   WGPUBuffer uniformBuffer;
   WGPUBuffer objectBuffer;
   size_t uniformBufferSize;

   WGPUVertexBufferLayout tempVertexBufferLayout;
   WGPUBindGroup tempBindGroup;
   WGPUBindGroupLayout tempBindGroupLayout;


   AspectMaterial *theOnlyMaterial;
//   WGPUBindGroup geometryBindGroup;
//   WGPUBindGroupLayout geometryBindGroupLayout;
//   WGPUBindGroup materialsBindGroup;
//   WGPUBindGroupLayout materialsBindGroupLayout;
   WGPURenderPassDepthStencilAttachment depthStencilAttachment;
   WGPUTextureView depthTextureView;
   WGPUTexture depthTexture;
   WGPURenderPipeline pipeline;
   size_t nodeCount;
   struct SceneNode *nodes;
} AspectRenderContext;

void
aspectRenderContextOnUpdate(AspectRenderContext *ctx);

struct SceneNodeIterator *
aspectRenderContextNewNodeIterator(AspectRenderContext *ctx, Error **err);

#endif
