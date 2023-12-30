#include <Platform/Core.h>
#include <Platform/IO.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

char *
pfmLoadContentAtPathAsString(const char *path, Error **err)
{
   //TODO this should use pfmLoadContentAtPath
   char *buffer = NULL;

   FILE *file = fopen(path, "r");
   if (file == NULL)
   {
      ERRORF(err, PFM_ERR_IO_FAULT, "failed to open file at %s", path);
      goto cleanup;
   }

   fseek(file, 0, SEEK_END);
   long length = ftell(file);
   rewind(file);

   size_t rawSize = length * sizeof(char);
   buffer = (char *) malloc(1 + rawSize);
   if (buffer == NULL)
   {
      REPORT_NULL_FAULT(buffer, err);
      goto cleanup;
   }

   buffer[rawSize] = '\0';
   size_t n = fread(buffer, sizeof(char), length, file);
   int fe = ferror(file);
   if (fe)
   {
      // if the read fails free the buffer here.
      free(buffer);
      buffer = NULL;
      ERRORF(err, PFM_ERR_IO_FAULT, "could not read %s", path);
      goto cleanup;
   }

   cleanup:
   if (file)
   {
      fclose(file);
   }
   return buffer;
}

bool
ioFileReportSize(FILE *stream, size_t *outSize, Error **err)
{
   // NULL stream will be caught by EBADF

   if (outSize == NULL)
   {
      REPORT_NULL_FAULT(outSize, err);
      return false;
   }

   errno = 0;
   if (fseek(stream, 0, SEEK_END) == -1)
   {
      ERROR(err, 0, "call fseek() has failed");
      return false;
   }

   errno = 0;
   if ((*outSize = ftell(stream)) == -1)
   {
      ERROR(err, 0, "call ftell() has failed");
      return false;
   }

   errno = 0;
   rewind(stream);
   if (errno != 0)
   {
      ERROR(err, 0, "call rewind() has failed");
      return false;
   }

   return true;
}

IOStaticBuffer *
ioStaticBufferNewFromFileContents(FILE *stream, Error **err)
{
   size_t fileSize;
   if (!ioFileReportSize(stream, &fileSize, err))
   {
      return NULL;
   }

   IOStaticBuffer *b = malloc(sizeof(IOStaticBuffer) + fileSize);
   if (b == NULL)
   {
      ERRORF(err, PFM_ERR_ALLOC_FAILED, "could not malloc static buffer of size %d", fileSize);
      return NULL;
   }
   b->length = fileSize;
   if (fread(b->data, sizeof(uint8_t), fileSize, stream) != fileSize)
   {
      ERRORF(err, PFM_ERR_IO_FAULT, "fread() on stream %d returned %d", fileno(stream), ferror(stream));
      free(b);
      return NULL;
   }

   return b;
}