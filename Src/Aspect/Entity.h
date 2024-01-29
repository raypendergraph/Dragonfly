
#ifndef ASPECT_ENTITY_H
#define ASPECT_ENTITY_H

#include <uuid/uuid.h>

typedef struct AspectEntity
{
   uuid_t id;
   char *name;
} AspectEntity;
#endif
