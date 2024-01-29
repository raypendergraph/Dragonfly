#include "StaticMesh.h"
#include "Graph.h"

void
aspectMeshBehaviorDestroy(struct StaticMesh *mesh)
{
   if (mesh == NULL) return;
   assert(mesh->vertices);
   free(mesh->vertices);
   assert(mesh->indices);
   free(mesh->indices);
   free(mesh);
}

struct StaticMesh *
aspectStaticMeshNewFromData(VertexBuffer *vertices, IndexBuffer *indices, Error **err)
{
   if (vertices == NULL)
   {
      REPORT_NULL_FAULT(vertices, err);
      return NULL;
   }

   if (indices == NULL)
   {
      REPORT_NULL_FAULT(indices, err);
      return NULL;
   }

   struct StaticMesh *mesh = malloc(sizeof(struct StaticMesh));
   if (mesh == NULL)
   {
      ERROR(err, PFM_ERR_ALLOC_FAILED, "mesh");
      return NULL;
   }

   mesh->vertices = vertices;
   mesh->indices = indices;

   return mesh;
}
