#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <Platform/Core.h>
#include <Platform/Text.h>
//#include "xxHash.h"

struct String
{
  size_t size;
  uint32_t hash;
  unsigned char buffer[];

};

typedef struct Error Error;

size_t
pfmRawStringLength(char const *s, size_t max)
{

    assert(s != NULL);
    size_t len = 0;
    if (s == NULL)
    {
        return len;
    }
    while (len <= max && s[len] != '\0')
    {
        len++;
    }

    return len;
}


String *
pfmStringNewFromBuffer(char const *buffer, size_t size, Error **err)
{
    String *str = calloc(sizeof(*str) + size, sizeof(char));
    if (str == NULL)
    {
        ERROR(err, 8, "string allocation has failed");
        return NULL;
    }
    str->size = size;
    //TODO set hash
    memcpy(str->buffer, buffer, size);

    return str;
}

String *
pfmStringCreateWithRawString(char const *cString, Error **err)
{
    size_t size = pfmRawStringLength(cString, PFM_TEXT_STRING_MAX_SIZE);
    return pfmStringNewFromBuffer(cString, size, err);
}

inline void
pfmStringDestroy(String *str)
{
    free(str);
}


inline size_t
pfmStringLength(String const *str)
{
    assert(str != NULL);
    //size is length for now with 8-bit characters.
    return str == NULL ? 0 : str->size;
}

inline bool
pfmStringEquals(String const *string, String const *anotherString)
{
    if (string == anotherString) // This works if both are NULL also 
    {
        return true;
    }
    if (string == NULL || anotherString == NULL)
    {
        return false;
    }
    return string->hash == anotherString->hash;
}