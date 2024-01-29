
#ifndef ASPECT_ACTOR_H
#define ASPECT_ACTOR_H

#include "Behavior.h"
#include "Entity.h"

struct Actor
{
   AspectEntity entity;
   size_t behaviorCount;
   struct Behavior *behaviors;
};
#endif
