
#ifndef PLATFORM_IO_H
#define PLATFORM_IO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct String String;
typedef struct Error Error;

/**
 * IOStaticBuffer is a simple memory buffer that is allocated and passed around. It is not changed once
 * allocated.
 */
typedef struct IOStaticBuffer
{
   size_t length;
   uint8_t data[];
} IOStaticBuffer;

char *
pfmLoadContentAtPathAsString(const char *path, Error **err);

bool
ioFileReportSize(FILE *stream, size_t *outSize, Error **err);

IOStaticBuffer *
ioStaticBufferNewFromFileContents(FILE *stream, Error **err);

#endif
