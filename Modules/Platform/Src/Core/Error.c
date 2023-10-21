#include "Platform/Core.h"
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

const char *EMPTY_STRING = "";

struct Error
{
  int code;
  errno_t errnoValue;
  uint16_t line;
  const char *file;
  char *message;
};

static Error *
internalErrorCreate(ErrorArgs args, va_list varArgs)
{
    errno_t capturedErrno = errno;
    errno = 0;
    va_list varArgsCopy;
    Error *error = NULL;

    BREAKPOINT();
    assert(args.message != NULL);
    assert(args.file != NULL);
    va_copy(varArgsCopy, varArgs);

    int size = vsnprintf(NULL, 0, args.message, varArgs);
    assert(size > 0);

    char *buffer = (char *) malloc(size + 1);
    assert(buffer);
    if (buffer == NULL)
    {
        goto cleanup;
    }

    error = (Error *) malloc(sizeof(Error));
    assert(error);
    if (error == NULL)
    {
        goto cleanup;
    }

    int copyRet = vsnprintf(buffer, size + 1, args.message, varArgsCopy);
    assert(copyRet > 0);

    error->errnoValue = capturedErrno;
    error->file = args.file;
    error->message = buffer;
    error->code = args.code;
    error->line = args.line;

    cleanup:
    if (buffer && !error)
    {
        free(buffer);
    }
    va_end(varArgs);
    va_end(varArgsCopy);

    return error;
}

void
pfmErrorDestroy(Error *err)
{
    assert(err);
    if (err == NULL)
    {
        return;
    }
    free(err->message);
    free(err);
}

Error *pfmErrorNew(ErrorArgs args, ...)
{
    va_list varArgs;
    va_start(varArgs, args);
    Error *err = internalErrorCreate(args, varArgs);
    va_end(varArgs);
    return err;
}

const char *
pfmErrorGetFile(Error const *err)
{
    assert(err != NULL);
    return err ? err->file : EMPTY_STRING;
}

const char *
pfmErrorGetMessage(Error const *error)
{
    assert(error != NULL);
    return error ? error->message : EMPTY_STRING;
}

int
pfmErrorGetCode(Error const *error)
{
    assert(error != NULL);
    return error ? error->code : INT_MIN;
}

const char *
pfmErrorToString(Error const *err)
{
    assert(err != NULL);
    if (err == NULL)
    {
        return EMPTY_STRING;
    }
    return err->message;
}

void
pfmErrorSetReference(Error **err, ErrorArgs args, ...)
{
    va_list varArgs;
    va_start(varArgs, args);
    *err = internalErrorCreate(args, varArgs);
    va_end(varArgs);
}
