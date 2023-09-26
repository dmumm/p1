#include "utils.h"

#include "common_libs.h"

#include <string.h>

void printUsage(char const * const optionalMessage)
{
    // fprintf(stderr, "%s", optionalMessage);
    // fprintf(stderr, "Usage: <path/to/leakcount> <path/to/program> [args for program]\n");
    // fprintf(stderr, "Usage example: ./leakcount ./my_test_program\n");
    // fprintf(stderr, "Usage example: ./leakcount ./my_test_program arg1 arg2\n");
}

char * realpathCMD(char const * const path)
{
    int const MAX_PATH_SIZE = 4096;
    int const NULL_TERMINATOR_SIZE = 1;

    char command[MAX_PATH_SIZE];
    snprintf(command, sizeof(command), "realpath -e \"%s\"", path);

    FILE * realpathPipe = popen(command, "r");
    if (! realpathPipe) {
        perror("Failed to run command");
        return NULL;
    }

    char resolvedPath[MAX_PATH_SIZE];
    fgets(resolvedPath, MAX_PATH_SIZE, realpathPipe);
    resolvedPath[strcspn(resolvedPath, "\r\n")] = 0;
    assert(ferror(realpathPipe) == 0);
    if (/*feof(realpathPipe) ||*/ ferror(realpathPipe)) {
        perror("Failed to read command output");
        // fprintf(stderr, "Failed to read command output\n");
        pclose(realpathPipe);
        return NULL;
    }
    pclose(realpathPipe);

    size_t length = strlen(resolvedPath) + NULL_TERMINATOR_SIZE;
    if (length == 0) {
        // fprintf(stderr, "Resolved path has length 0\n");
        return NULL;
    }
    // if (resolvedPath[length - 1] == '\n') {
    //     resolvedPath[length - 1] = '\0';
    // }

    char * result = (char *)malloc(length);
    if (! result) {
        perror("Failed to allocate memory for resolved path");
        return NULL;
    }
    snprintf(result, length, "%s", resolvedPath);
    // strcpy(result, resolvedPath);

    return result;
}
