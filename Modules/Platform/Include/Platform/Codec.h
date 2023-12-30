

#ifndef LEARNING_CODEC_H
#define LEARNING_CODEC_H

#include <stddef.h>
#include <stdint.h>
#include <Platform/Core.h>
#include <Platform/IO.h>

/*
 * PNG
 */
typedef enum
{
   CODEC_PNG_INTERLACE_NONE = 0,
   CODEC_PNG_INTERLACE_ADAM7 = 1,
   CODEC_PNG_INTERLACE_FORCE32 = 0x7FFFFFFF
} CodecPNGInterlaceType;

typedef enum
{
   CODEC_PNG_COLOR_TYPE_GRAYSCALE = 0x0,
   CODEC_PNG_COLOR_TYPE_TRUECOLOR = 0x2,
   CODEC_PNG_COLOR_TYPE_INDEXED = 0x3,
   CODEC_PNG_COLOR_TYPE_GRAYSCALE_ALPHA = 0x4,
   CODEC_PNG_COLOR_TYPE_TRUECOLOR_ALPHA = 0x6,
   CODEC_PNG_COLOR_TYPE_FORCE32 = 0x7FFFFFFF
} CodecPNGColorType;

//typedef enum
//{
//   CODEC_PNG_FILTER_DISABLE = 0,
//   CODEC_PNG_FILTER_NONE = 8,
//   CODEC_PNG_FILTER_SUB = 16,
//   CODEC_PNG_FILTER_UP = 32,
//   CODEC_PNG_FILTER_AVG = 64,
//   CODEC_PNG_FILTER_PAETH = 128,
//   CODEC_PNG_FILTER_ALL = (8 | 16 | 32 | 64 | 128)
//} CodecPNGFilterOption;

typedef struct CodecDecodedPNGData
{
   uint32_t width;
   uint32_t height;
   uint8_t depth;
   CodecPNGColorType color;
   CodecPNGInterlaceType interlace;
} CodecDecodedPNG;

enum CodecType
{
   CodecTypeInvalid,
   CodecTypePNG
};

typedef enum uint16_t
{
   CodecImageFormatInvalid,
   CodecImageFormatPNG,
   CodecImageFormatCount
} CodecImageType;

CodecDecodedPNG *
codecDecodedPNGNew(IOStaticBuffer const *buffer, Error **err);

bool
codecDecodedPNGGetData(CodecDecodedPNG const *png, uint8_t **outData, size_t *outSize, Error **err);

#endif //LEARNING_CODEC_H
