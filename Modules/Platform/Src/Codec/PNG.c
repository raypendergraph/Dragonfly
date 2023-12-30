#include <Platform/IO.h>
#include <spng.h>
#include <stdbool.h>
#include <Platform/Codec.h>
#include <Platform/Core.h>
#include <assert.h>

typedef struct
{
   CodecDecodedPNG public;
   size_t dataSize;
   uint8_t data[];
} PrivateCodecDecodedPNG;

bool
codecDecodedPNGGetData(CodecDecodedPNG const *png, uint8_t **outData, size_t *outSize, Error **err)
{
   if (png == NULL)
   {
      REPORT_NULL_FAULT(png, err);
      return false;
   }
   if (outData == NULL)
   {
      REPORT_NULL_FAULT(outData, err);
      return false;
   }
   if (outSize == NULL)
   {
      REPORT_NULL_FAULT(outSize, err);
      return false;
   }

   *outData = ((PrivateCodecDecodedPNG *) png)->data;
   *outSize = ((PrivateCodecDecodedPNG *) png)->dataSize;
   return true;
}

CodecDecodedPNG *
codecDecodedPNGNew(IOStaticBuffer const *buffer, Error **err)
{
   if (buffer == NULL)
   {
      REPORT_NULL_FAULT(buffer, err);
      return NULL;
   }
   PrivateCodecDecodedPNG *out = NULL;
   spng_ctx *ctx = spng_ctx_new(0);
   if (ctx == NULL)
   {
      ERROR(err, 0, "could not create a png context");
      goto onFailure;
   }

   int spngErr = spng_set_png_buffer(ctx, buffer->data, buffer->length);
   if (spngErr)
   {
      ERRORF(err, 0, "error setting png buffer: %d", spngErr);
      goto onFailure;
   }

   size_t imageSize;
   spngErr = spng_decoded_image_size(ctx, SPNG_FMT_RAW, &imageSize);
   if (spngErr)
   {
      ERRORF(err, 0, "error determining decoded PNG size: %d", spngErr);
      goto onFailure;
   }

   out = malloc(sizeof(PrivateCodecDecodedPNG) + imageSize);
   if (out == NULL)
   {
      ERROR(err, PFM_ERR_ALLOC_FAILED, "malloc failed")
      goto onFailure;
   }
   struct spng_ihdr header;
   spngErr = spng_get_ihdr(ctx, &header);
   if (spngErr)
   {
      ERROR(err, 0, "could not query the PNG header");
      goto onFailure;
   }

   out->public = (CodecDecodedPNG) {
      .color = header.color_type,
      .depth = header.bit_depth,
      .interlace = header.interlace_method,
      .width = header.width,
      .height = header.height
   };

   out->dataSize = imageSize;
   spngErr = spng_decode_image(ctx, &out->data, imageSize, SPNG_FMT_RAW, 0);
   if (spngErr)
   {
      ERROR(err, 0, "could not decode the PNG");
      goto onFailure;
   }

   spng_ctx_free(ctx);
   return (CodecDecodedPNG *) out;

   onFailure:
   if (ctx)
   {
      spng_ctx_free(ctx);
   }
   if (out)
   {
      free(out);
   }
   return NULL;
}