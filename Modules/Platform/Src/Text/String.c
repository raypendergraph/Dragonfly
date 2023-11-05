#include <assert.h>
#include <errno.h>
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
pfmASCIIStringLength(char const *s, size_t max)
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

bool
pfmASCIIStringParseFloat(const char *value, float *outValue, Error **err)
{
    if (value == NULL)
    {
        REPORT_NULL_FAULT(value, err);
        return false;
    }

    if (outValue == NULL)
    {
        REPORT_NULL_FAULT(outValue, NULL);
        return false;
    }

    char *end;

    errno = 0;
    float f = strtof(value, &end);
    if (end == value)
    {
        ERRORF(err, 27, "could not convert [%s] to a float", value);
        return false;
    }

    *outValue = f;
    return true;
}

bool
pfmASCIIStringParseULong(const char *value, unsigned long *outValue, Error **err)
{
    if (value == NULL)
    {
        REPORT_NULL_FAULT(value, err);
        return false;
    }

    if (outValue == NULL)
    {
        REPORT_NULL_FAULT(outValue, NULL);
        return false;
    }

    char *end;

    errno = 0;
    unsigned long l = strtoul(value, &end, 10);
    if (end == value)
    {
        ERRORF(err, 27, "could not convert [%s] to an unsigned long", value);
        return false;
    }

    *outValue = l;

    return true;
}

bool
pfmASCIIStringInsensitiveCompare(const char *a, const char *b, size_t n)
{
    if (a == b)
    {
        return true;
    }
    if (a == NULL || b == NULL)
    {
        return false;
    }

    for (size_t i = 0; i < n; a++, b++)
    {
        // Both strings are done and they are equal
        if (!*a && !*b)
        {
            return true;
        }

        // a is not a letter
        if ((*a < 'A' || *a > 'Z') && (*a < 'a' || *a > 'z'))
        {
            if (*a == *b)
            {
                continue;
            }
            return false;
        }

        // ASCII is written so that:
        // Uppercase 'A' is 65 in decimal, or 0100 0001 in binary.
        // Lowercase 'a' is 97 in decimal, or 0110 0001 in binary
        // So performing an or of 0010 0000 on the character makes it insensitive.
        if ((*a | 0x20) != (*b | 0x20))
        {
            return false;
        }
    }
    return false;
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
    size_t size = pfmASCIIStringLength(cString, PFM_TEXT_STRING_MAX_SIZE);
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