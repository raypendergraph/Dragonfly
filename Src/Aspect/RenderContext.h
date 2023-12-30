
#ifndef ASPECT_RENDERCONTEXT_H
#define ASPECT_RENDERCONTEXT_H

#include "Renderer.h"
#include "Types.h"
#include "Material.h"

typedef struct AspectRenderContext
{
   float t;
   VertexBuffer2D *vertices;
   IndexBuffer *indices;
   AspectRenderer *renderer;
   WGPUBuffer vertexBuffer;
   WGPUBuffer indexBuffer;
   WGPUBuffer uniformBuffer;
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
} AspectRenderContext;
#endif
