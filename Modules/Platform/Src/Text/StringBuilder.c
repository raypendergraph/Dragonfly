#include "Platform/Core.h"
#include "Platform/Text.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct Header
{
} Header;

typedef struct StringBuilder
{
   size_t length;
   uint16_t capacity;
   char *buffer;
} StringBuilder;

const size_t MAX_CAPACITY = PFM_TEXT_STRING_MAX_SIZE - sizeof(StringBuilder);
const size_t MIN_BUFFER_ALLOCATION = 128;

static void internalResize(StringBuilder *builder, size_t delta, Error **err)
{
   char *resizedBuffer = realloc(builder->buffer, builder->capacity + delta);
   if (resizedBuffer == NULL)
   {
      ERROR(err, 8, "could not allocate enough memory for this string");
      return;
   }

   builder->buffer = resizedBuffer;
   builder->capacity += delta;
}

static size_t autoResize(StringBuilder *builder, size_t required, Error **err)
{
   size_t currentFreeSpace = builder->capacity - builder->length;
   if (required < currentFreeSpace)
   {
      return currentFreeSpace;
   }

   size_t totalAvailableCapacity = MAX_CAPACITY - builder->length;
   if (required > totalAvailableCapacity)
   {
      //error
      return currentFreeSpace;
   }
   size_t delta = clampUnsigned(2 * required, MIN_BUFFER_ALLOCATION, totalAvailableCapacity);
   internalResize(builder, delta, err);

   return builder->capacity - builder->length;
}

/**
 *
 * @param err
 * @return
 */
StringBuilder *
pfmStringBuilderNew(Error **err)
{
   StringBuilder *builder = malloc(sizeof(StringBuilder));
   if (builder == NULL)
   {
      ERROR(err, PFM_ERR_ALLOC_FAILED, "could not allocate StringBuilder");
      return NULL;
   }

   return builder;
}

/**
 *
 * @param builder
 */
void
pfmStringBuilderDestroy(StringBuilder *builder)
{
   free(builder->buffer);
   free(builder);
}


inline size_t
pfmStringBuilderGetLength(StringBuilder *builder, Error **err)
{
   if (builder == NULL)
   {
      REPORT_NULL_FAULT(builder, err);
      return 0;
   }
   return builder->length;
}

inline size_t
pfmStringBuilderGetAvailable(StringBuilder *builder, Error **err)
{

   return builder->capacity - builder->length;
}

inline size_t
pfmStringBuilderGetCapacity(StringBuilder *builder, Error **err)
{
   if (builder == NULL)
   {
      REPORT_NULL_FAULT(builder, err);
      return 0;
   }

   return builder->capacity;
}

size_t
pfmStringBuilderAddCapacity(StringBuilder *builder, size_t deltaSize, Error **err)
{
   if (builder == NULL)
   {
      REPORT_NULL_FAULT(builder, err);
      return 0;
   }

   if ((builder->capacity + deltaSize) > MAX_CAPACITY)
   {
      ERROR(err, 8, "string builder overflow");
      return 0;
   }

   deltaSize = deltaSize < MIN_BUFFER_ALLOCATION ? MIN_BUFFER_ALLOCATION : deltaSize;
   internalResize(builder, deltaSize, err);
   return builder->capacity - builder->length;
}

size_t
pfmStringBuilderAppendString(StringBuilder *builder, char const *cStr, Error **err)
{
   if (builder == NULL)
   {
      REPORT_NULL_FAULT(builder, err);
      return 0;
   }

   // Seems like a reasonable upper limit
   size_t length = pfmASCIIStringLength(cStr, MAX_CAPACITY - builder->capacity);
   size_t freeSpace = autoResize(builder, length, err);
   if (err != NULL)
   {
      return freeSpace;
   }
   memcpy(&builder->buffer[builder->length], cStr, length);
   builder->length += length;
   return builder->capacity - builder->length;
}

void
pfmStringBuilderAppendChar(StringBuilder *builder, char c, Error **err)
{
   if (builder == NULL)
   {
      REPORT_NULL_FAULT(builder, err);
      return;
   }

   size_t freeSpace = autoResize(builder, c, err);
   if (err != NULL)
   {
      return;
   }
   builder->buffer[builder->length] = c;
   builder->length += 1;
}

size_t
pfmStringBuilderAppendBuffer(StringBuilder *builder, char const *buffer, size_t size, Error **err)
{
   return 0;
}

/**
 *
 * @param builder
 * @param err
 * @return
 */
const char *
pfmStringBuilderToString(StringBuilder *builder, Error **err)
{
   size_t len = builder->length;
   char *returnBuffer = malloc(len + 1);
   if (returnBuffer == NULL)
   {
      REPORT_NULL_FAULT(returnBuffer, err);
   }
   memcpy(returnBuffer, builder->buffer, len);
   returnBuffer[len + 1] = '\0';
   return returnBuffer;
}

/////////////////////////////