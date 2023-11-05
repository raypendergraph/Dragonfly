#ifndef PLATFORM_TEXT_H
#define PLATFORM_TEXT_H

#include <stddef.h>

//////////////////////
// String
/////////////////////
#ifndef PFM_TEXT_STRING_MAX_SIZE
#define PFM_TEXT_STRING_MAX_SIZE (1024UL * 1024)
#endif
typedef struct String String;
typedef struct Error Error;

String *
pfmStringNew(char const *cString, Error **err);

size_t
pfmStringLength(String const *cString);

bool
pfmStringEquals(String const *str, String const *anotherString);

void
pfmStringDestroy(String *str);

String *
pfmStringNewFromBuffer(char const *buffer, size_t size, Error **err);

String *
pfmStringCreateWithRawString(char const *cString, Error **err);
////////////////////////////
// ASCII string functions
////////////////////////////

size_t
pfmASCIIStringLength(char const *s, size_t max);

bool
pfmASCIIStringInsensitiveCompare(const char *a, const char *b, size_t len);

bool
pfmASCIIStringParseFloat(const char *value, float *outValue, Error **err);

bool
pfmASCIIStringParseULong(const char *value, unsigned long *outValue, Error **err);

/////////////////////////
// StringBuilder
/////////////////////////

typedef struct StringBuilder StringBuilder;

StringBuilder *
pfmStringBuilderNew(Error **err);

void
stringBuilderDestroy(StringBuilder *builder);

size_t
pfmStringBuilderAddCapacity(StringBuilder *builder, size_t deltaSize, Error **err);

void
pfmStringBuilderAppendChar(StringBuilder *builder, char c, Error **err);

size_t
pfmStringBuilderAppendString(StringBuilder *builder, char const *raw, Error **err);

size_t
pfmStringBuilderAppendBuffer(StringBuilder *builder, char const *buffer, size_t size, Error **err);

size_t
pfmStringBuilderGetAvailable(StringBuilder *builder, Error **err);

size_t
pfmStringBuilderGetCapacity(StringBuilder *builder, Error **err);

size_t
pfmStringBuilderGetLength(StringBuilder *builder, Error **err);

const char *
pfmStringBuilderToString(StringBuilder *builder, Error **err);

#endif