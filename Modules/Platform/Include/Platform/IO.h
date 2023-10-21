
#ifndef PLATFORM_IO_H
#define PLATFORM_IO_H

typedef struct String String;
typedef struct Error Error;

const char *
pfmLoadContentAtPathAsString(const char *path, Error **err);

#endif
