#include <Platform/Core.h>
#include <Platform/Text.h>
#include <assert.h>
#include <libxml/xmlmemory.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "Internal.h"

#define XML_API_FAILURE (-1)


const char *
aspectVertexBuffer2dToString(VertexBuffer2D *vb)
{
   StringBuilder *strb = pfmStringBuilderNew(NULL);
   char buffer[128] = {};
   for (size_t i = 0; i < vb->count; i++)
   {
      Vertex2D v = vb->data[i];
      snprintf(buffer, 128, "x=%f y=%f  r=%f g=%f b=%f a=%f\n", v.x, v.y, v.r, v.g, v.b);
      pfmStringBuilderAppendString(strb, buffer, NULL);
   }
   return pfmStringBuilderToString(strb, NULL);
}

static bool
parseAttributeAsFloat(xmlNode *node, const char *attr, float *outValue)
{
   assert(outValue);
   assert(attr);
   assert(node);
   xmlChar *value = xmlGetProp(node, (xmlChar *) attr);
   if (value == NULL)
   {
      return false;
   }

   return pfmASCIIStringParseFloat((const char *) value, outValue, NULL);
}


static bool
parseVertices(VertexBuffer *buffer, xmlNode *vertexGroup, Error **err)
{
   assert(buffer);
   assert(vertexGroup);
   xmlNodePtr vertexNode = vertexGroup->children;
   for (size_t i = 0; vertexNode != NULL; i++)
   {
      parseAttributeAsFloat(vertexNode, "x", &buffer->data[i].x);
      parseAttributeAsFloat(vertexNode, "y", &buffer->data[i].y);
      parseAttributeAsFloat(vertexNode, "z", &buffer->data[i].z);
      parseAttributeAsFloat(vertexNode, "r", &buffer->data[i].r);
      parseAttributeAsFloat(vertexNode, "g", &buffer->data[i].g);
      parseAttributeAsFloat(vertexNode, "b", &buffer->data[i].b);
      vertexNode = vertexNode->next;
   }

   return true;
}

static bool
parseIndices(IndexBuffer *buffer, xmlNode *indexGroup, Error **err)
{
   assert(buffer);
   assert(indexGroup);
   xmlNode *current = indexGroup->children;
   const char *whitespace = "\t\n ";
   size_t dataIndex = 0;
   while (current != NULL)
   {
      xmlChar *elementText = xmlNodeGetContent(current);
      if (elementText == NULL)
      {
         //TODO
      }
      char *savePtr;
      char *token = strtok_r((char *) elementText, whitespace, &savePtr);
      for (; token != NULL; dataIndex++)
      {
         unsigned long parsed;
         if (!pfmASCIIStringParseULong(token, &parsed, err))
         {
            return false;
         }
         if (parsed > UINT32_MAX)
         {
            //TODO
         }
         buffer->data[dataIndex] = parsed;
         token = strtok_r(NULL, whitespace, &savePtr);
      }
      current = current->next;
   }
   return true;
}

static bool
parseEntity(AspectEntity *entity, xmlNode *entityNode, Error **err)
{
   xmlNode *current = entityNode->children;
   while (current != NULL)
   {
      if (pfmASCIIStringInsensitiveCompare("vertices", (const char *) current->name, 32))
      {
         size_t childCount = xmlChildElementCount(current);
         VertexBuffer *vb = malloc(sizeof(*vb) + sizeof(Vertex) * childCount);
         vb->count = childCount;
         entity->vertexBuffer = vb;
         if (!parseVertices(vb, current, err))
         {
            goto onFailure;
         }
      } else if (pfmASCIIStringInsensitiveCompare("indices", (const char *) current->name, 32))
      {
         size_t childCount = xmlChildElementCount(current);
         // TODO hard coded three indexes per shape.
         IndexBuffer *ib = malloc(sizeof(*ib) + sizeof(uint32_t) * childCount * 3);
         ib->count = childCount * 3;
         entity->indexBuffer = ib;
         if (!parseIndices(ib, current, err))
         {
            goto onFailure;
         }
      } else
      {
         fprintf(stderr, "unknown geometry element [%s]\n", current->name);
      }

      current = current->next;
   }
   return true;

   onFailure:
   if (entity->vertexBuffer)
   {
      free(entity->vertexBuffer);
   }

   if (entity->indexBuffer)
   {
      free(entity->indexBuffer);
   }
   return false;
}

static bool
parseEntityGroup(AspectEntityGroup *group, xmlNode *groupNode, Error **err)
{
   assert(group);
   assert(groupNode);
   xmlNodePtr current = groupNode->children;
   for (size_t i = 0; current != NULL; i++)
   {
      if (!parseEntity(&group->data[i], current, err))
      {
         return false;
      }
      current = current->next;
   }
   return true;
}

static bool
parseScene(AspectScene *scene, xmlNode *sceneNode, Error **err)
{
   assert(sceneNode);
   assert(scene);

   xmlNodePtr current = sceneNode->children;
   while (current != NULL)
   {
      if (pfmASCIIStringInsensitiveCompare("entities", (const char *) current->name, 32))
      {
         size_t entityCount = xmlChildElementCount(current);
         AspectEntityGroup *eg = malloc(sizeof(*scene->entities) + entityCount * sizeof(AspectEntity));
         eg->size = entityCount;
         scene->entities = eg;
         if (!parseEntityGroup(eg, current, err))
         {
            goto onFailure;
         }
      } else
      {
         fprintf(stderr, "unknown scene child %s", current->name);
      }
      current = current->next;
   }
   return true;

   onFailure:
   if (scene->entities)
   {
      free(scene->entities);
   }

   return false;
}

AspectScene *
aspectSceneNewFromFile(char *fileName, Error **err)
{
   AspectScene *scene = NULL;
   xmlDoc *doc = NULL;

   doc = xmlReadFile(fileName, NULL, XML_PARSE_NOBLANKS);
   if (doc == NULL)
   {
      ERRORF(err, 0, "could not parse or read scene file %s", fileName);
      goto onFailure;
   }

   xmlNode *sceneNode = xmlDocGetRootElement(doc);
   if (sceneNode == NULL)
   {
      ERROR(err, 0, "scene document seems to be empty");
      goto onFailure;
   }

   scene = malloc(sizeof(*scene));
   if (!parseScene(scene, sceneNode, err))
   {
      goto onFailure;
   }

   xmlFree(doc);
   return scene;


   onFailure:
   if (doc)
   {
      xmlFree(doc);
   }
   if (scene)
   {
      free(scene);
   }
   return false;
}

bool
aspectIndexBufferCopy(IndexBuffer *src, IndexBuffer *dst, size_t dstOffset, Error **err)
{
   if (src == NULL)
   {
      REPORT_NULL_FAULT(src, err);
      return false;
   }

   if (dst == NULL)
   {
      REPORT_NULL_FAULT(dst, err);
      return false;
   }

   bool badInput = dstOffset > dst->count * sizeof(*dst->data)//start is bigger than size
                   || dst->count - dstOffset < src->count; // source is bigger than target

   if (badInput)
   {
      ERRORF(err, 90, "index buffer copy input math is bad src=%d dst=%d dstOffset=%d",
             src->count, dst->count, dstOffset);
      return false;
   }

   memcpy(&dst->data[dstOffset], &src->data[0], sizeof(*src->data) * src->count);
   return true;
}

bool
aspectVertexBuffer2dCopy(VertexBuffer2D *src, VertexBuffer2D *dst, size_t dstOffset, Error **err)
{
   if (src == NULL)
   {
      REPORT_NULL_FAULT(src, err);
      return false;
   }

   if (dst == NULL)
   {
      REPORT_NULL_FAULT(dst, err);
      return false;
   }
   bool badInput = dstOffset > dst->count //start is bigger than size
                   || dst->count - dstOffset < src->count; // source is bigger than target

   if (badInput)
   {
      ERRORF(err, 90, "vertex buffer copy input math is bad src=%d dst=%d dstOffset=%d",
             src->count, dst->count, dstOffset);
      return false;
   }

   memcpy(&dst->data[dstOffset], &src->data[0], sizeof(*src->data) * src->count);

   return true;
}

bool
sceneBuildGeometry2D(AspectScene *scene, VertexBuffer2D **outVertices, IndexBuffer **outIndices, Error **err)
{
   if (scene == NULL)
   {
      REPORT_NULL_FAULT(scene, err);
      return false;
   }
   if (outVertices == NULL)
   {
      REPORT_NULL_FAULT(outVertices, err);
      return false;
   }
   if (outIndices == NULL)
   {
      REPORT_NULL_FAULT(outIndices, err);
      return false;
   }
   size_t entityCount = scene->entities->size;
   AspectEntity *entities = scene->entities->data;
   size_t vertexCount = 0;
   size_t indexCount = 0;
   for (size_t i = 0; i < entityCount; i++)
   {
      vertexCount += entities[i].vertexBuffer->count;
      indexCount += entities[i].indexBuffer->count;
   }

   *outVertices = malloc(sizeof(VertexBuffer2D) + sizeof(Vertex2D) * vertexCount);
   (*outVertices)->count = vertexCount;
   *outIndices = malloc(sizeof(IndexBuffer) + sizeof(uint32_t) * indexCount);
   (*outIndices)->count = indexCount;


   for (size_t i = 0, vPtr = 0, iPtr = 0; i < entityCount; i++)
   {
      if (!aspectIndexBufferCopy(entities[i].indexBuffer, *outIndices, iPtr, err))
      {
         goto onFailure;
      }
      iPtr += entities[i].indexBuffer->count;
      if (!aspectVertexBuffer2dCopy(entities[i].vertexBuffer, *outVertices, vPtr, err))
      {
         goto onFailure;
      }
      vPtr += entities[i].vertexBuffer->count;
   }
   return true;

   onFailure:
   if (*outIndices)
   {
      free(*outIndices);
   }
   *outIndices = NULL;
   if (*outVertices)
   {
      free(*outVertices);
   }
   *outVertices = NULL;

   return false;
}