#ifndef PLATFORM_CORE_H
#define PLATFORM_CORE_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#ifdef _MSC_VER
/* Microsoft Visual Studio */
#include <intrin.h>
#define BREAKPOINT() __debugbreak();
#elif defined(__GNUC__)
/* GCC family of compilers (also works for Clang) */
#if defined(__x86_64__) || defined(__i386__)
/* x86/x86-64 */
#define BREAKPOINT() __asm__ volatile("int $0x03");
#else
/* Other architectures (e.g., ARM) */
#define BREAKPOINT() __builtin_trap();
#endif
#elif defined(__clang__)
/* Clang compiler */
#define BREAKPOINT() __builtin_debugtrap();
#else
#include <assert.h>
#define BREAKPOINT() assert(0); /* Fallback to assert(0) for unknown compilers */
#endif

typedef enum
{
   PFM_ERR_NONE = 0,
   PFM_ERR_ALLOC_FAILED = 1,
   PFM_ERR_NULL_FAULT = 2,
   PFM_ERR_IO_FAULT = 3,
   PFM_ERR_INVALID_KIND = 4
} PlatformErrorType;

typedef struct ErrorArgs
{
   int code;
   char const *message;
   char const *file;
   uint16_t line;
} ErrorArgs;

typedef struct Error Error;

#define FREE(ptr) if (ptr != NULL) free(ptr)

#define REPORT_NULL_FAULT(arg, err) \
do{ \
    if (err == NULL) \
    { \
        break; \
    } \
    ErrorArgs args = { \
        .code=PFM_ERR_NULL_FAULT, \
        .message="'%s' is NULL and should not be", \
        .file=__FILE__,  \
        .line=__LINE__}; \
    pfmErrorSetReference((err), args, #arg); \
}while(true)

#define ERRORF(ref, errCode, msg, ...) pfmErrorSetReference(ref, (ErrorArgs){.code = errCode, .message = msg, .file = __FILE__, .line = __LINE__}, __VA_ARGS__);
#define ERROR(ref, errCode, msg) pfmErrorSetReference(ref, (ErrorArgs){.code = errCode, .message = msg, .file = __FILE__, .line = __LINE__});


void
pfmErrorSetReference(Error **err, ErrorArgs args, ...);

Error *
pfmErrorNew(ErrorArgs args, ...);

void
pfmErrorDestroy(Error *err);

const char *
pfmErrorGetFile(Error const *err);

const char *
pfmErrorGetMessage(Error const *error);

int
pfmErrorGetCode(Error const *error);

const char *
pfmErrorToString(Error const *err);


//////////////////
// Logger
//////////////////

#define LOGLEVEL_STR_ERROR "ERROR"
#define LOGLEVEL_STR_WARNING "WARNING"
#define LOGLEVEL_STR_INFO "INFO"
#define LOGLEVEL_STR_DEBUG "DEBUG"

typedef struct Logger Logger;

typedef enum
{
   LOGLEVEL_ERROR = 0,
   LOGLEVEL_WARNING = 1,
   LOGLEVEL_INFO = 2,
   LOGLEVEL_DEBUG = 3,
   LOGLEVEL_COUNT = 4
} LogLevelType;

typedef struct LoggerConfig LoggerConfig;

Logger *
pfmLoggerInit(char const *name, LogLevelType level, LoggerConfig config);

void
pfmLoggerSetLevel(Logger *logger, LogLevelType level);

enum LogLevel
pfmLoggerGetLogLevel(Logger const *logger);

void
pfmLoggerWrite(Logger *logger, LogLevelType level, char const *file, size_t line, char const *fmt, ...);

#ifdef DEBUG
#define LOG_DEBUG(logger, ...) pfmLoggerWrite(logger, LOGLEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_DEBUG(logger, ...) ((void)0)
#endif
#define LOG_INFO(logger, ...) pfmLoggerWrite(logger, LOGLEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(logger, ...) pfmLoggerWrite(logger, LOGLEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(logger, ...) pfmLoggerWrite(logger, LOGLEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)


double
clamp(double value, double lower, double upper);

unsigned long long
clampUnsigned(unsigned long long value, unsigned long long lower, unsigned long long upper);

#endif


