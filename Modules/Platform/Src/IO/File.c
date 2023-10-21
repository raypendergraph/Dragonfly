#include <Platform/Core.h>
#include <stdio.h>
#include <stdlib.h>

const char *
pfmLoadContentAtPathAsString(const char *path, Error **err)
{
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