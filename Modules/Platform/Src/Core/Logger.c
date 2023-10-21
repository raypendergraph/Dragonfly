#include "Platform/Core.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static const char *LOG_LEVELS[] = {
   "ERROR",
   "WARNING",
   "INFO",
   "DEBUG"
};

struct LoggerConfig
{
  FILE *out;
  FILE *error;
};

struct Logger
{
  FILE *out;
  FILE *error;
  LogLevelType currentLevel;
};

Logger *
pfmLoggerNew(char const *name, LogLevelType level, LoggerConfig config)
{
    Logger *l = (Logger *) malloc(sizeof *l);
    assert(l);
    // TODO
    //return Logger{

    //}

}

void
pfmLoggerWrite(Logger *logger, LogLevelType level, char const *file, size_t const line, char const *fmt, ...)
{
    if (level > logger->currentLevel)
    {
        return;
    }

    printf("%s: %lu: ", file, line);
    va_list argp;
    va_start(argp, fmt);
    printf(fmt, argp);
    va_end(argp);
    printf("\n");
}