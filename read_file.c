#include <stdlib.h>
#include <stdio.h>

size_t read_file_to_str(const char *file_path, char **dst) {
    FILE *file = fopen(file_path, "r");
    if (!file) goto error;

    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    if (!len) goto error;
    *dst = (char*)malloc(len+1);
    fseek(file, 0, SEEK_SET);

    size_t read = fread(*dst, sizeof(char), len, file);
    fclose(file);
    if (read != len) goto cleanup;

    (*dst)[len] = '\0';

    return len;

cleanup:
    free(*dst);
    *dst = NULL;
error:
    fprintf(stderr, "Failed to read file %s\n", file_path);
    return 0;
}
