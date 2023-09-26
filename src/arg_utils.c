
#include "arg_utils.h"

#include "common_libs.h"
#include "utils.h"

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

void standardizeSubjectArgs(Request * const request, int const argc, char const *** const argv_pointers, char const * const argv_data)
{

    static int subjectArgc = 0;
    char const ** subjectArgv = NULL;
    size_t const SUBJECT_START_INDEX = 1;
    char const * argIterator = (*argv_pointers)[SUBJECT_START_INDEX];
    while (*argIterator && subjectArgc < argc) {
        argIterator = parseArg(&subjectArgv, argIterator, &subjectArgc);
    }

    request->subjectArgs = subjectArgv;
    request->subjectArgc = subjectArgc;
}

char const * parseArg(char const *** const pSubjectArgv, char const * charIterator, int * const subjectArgc)
{
    int const NULL_TERMINATOR_SIZE = 1;
    char const * const rawArg = charIterator;

    bool inQuotePairs = false;

    // Skip leading whitespace
    while (isspace(*charIterator)) {
        charIterator++;
    }

    while (*charIterator) {
        if (*charIterator == '"') {
            inQuotePairs = ! inQuotePairs;
        }
        else if (isspace(*charIterator) && ! inQuotePairs) {
            break;
        }
        charIterator++;
    }
    int argLength = charIterator - rawArg;

    char * const arg = (char *)malloc(argLength + NULL_TERMINATOR_SIZE);
    if (! arg) {
        perror("Failed to allocate memory for argument");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < argLength; i++) {
        if (rawArg[i] == '"') {
            i--;
            argLength--;
            continue;
        }
        arg[i] = rawArg[i];
    }
    arg[argLength] = '\0';

    appendArg(pSubjectArgv, subjectArgc, arg);

    if (*charIterator == '\0') {
        return charIterator + 1;
    }
    return charIterator;
}

void appendArg(char const *** const pSubjectArgv, int * const subjectArgc, char const * const arg)
{
    // size_t newLength = strlen(arg) + 1;
    size_t toAllocate = ((*subjectArgc) + 1) * sizeof(char *);
    *pSubjectArgv = (char const **)realloc(*pSubjectArgv, toAllocate);
    if (! *pSubjectArgv) {
        perror("Failed to allocate memory for argument");
        exit(EXIT_FAILURE);
    }
    (*pSubjectArgv)[*subjectArgc] = arg;
    (*subjectArgc)++;
}

void deriveSubjectPath(Request * pRequest, char const * inputPath)
{
    int const MAX_PATH_SIZE = 4096;

    // Getting real path to current working directory //
    char * rawCWD = get_current_dir_name();
    char * resolvedCWD = realpath(rawCWD, NULL);
    if (! resolvedCWD) {
        printUsage("WARNING: Failed to get real path for current directory\nAttemping fallback path\n");
        resolvedCWD = rawCWD;
    }

    // Getting real path to subject program //

    char * rawPath = (char *)malloc(MAX_PATH_SIZE);

    // Determine if path given is absolute, relative, or just a program name

    switch (inputPath[0]) {
    case '/': // Absolute path given
        snprintf(rawPath, MAX_PATH_SIZE, "%s", inputPath);
        fprintf(stderr, "Warning: Absolute path given for program to test: %s\n", rawPath);
        break;

    case '.': // Relative path given
        snprintf(rawPath, MAX_PATH_SIZE, "%s/%s", resolvedCWD, inputPath);
        break;

    default: // Program name given
        fprintf(stderr, "Warning: No path given for program to test. Assuming '%s' is in PATH", inputPath);
        char command[MAX_PATH_SIZE];
        snprintf(command, MAX_PATH_SIZE, "which %s", inputPath);
        FILE * whichPipe = popen(command, "r");
        if (! whichPipe) {
            perror("Failed to run which command on program name");
            printUsage("");
            exit(EXIT_FAILURE);
        }

        //./bin/leakcount ls .
        fgets(rawPath, MAX_PATH_SIZE, whichPipe);
        rawPath[strcspn(rawPath, "\r\n")] = 0;
        assert(ferror(whichPipe) == 0);
        if (/*! feof(whichPipe) ||*/ ferror(whichPipe)) {
            fprintf(stderr, "Failed to find %s. Program may not exist in PATH\n", inputPath);
            printUsage("");
            pclose(whichPipe);
            exit(EXIT_FAILURE);
        }
        pclose(whichPipe);

        break;
    }

    if (strlen(rawPath) == 0) {
        fprintf(stderr, "Raw path to requested program has length 0: %s\n", rawPath);
        printUsage("");
        exit(EXIT_FAILURE);
    }

    // Resolve optimal path to program to test

    char * resolvedAbsolutePath = realpathCMD(rawPath);
    if (! resolvedAbsolutePath) {
        fprintf(stderr, "WARNING: Failed to derive real path to program to test\n");
        fprintf(stderr, "         Falling back to requested path: %s\n", rawPath);
        free(resolvedAbsolutePath);
        resolvedAbsolutePath = rawPath;
    }
    if (strlen(resolvedAbsolutePath) == 0) {
        printUsage("ERROR: Failed to resolve path to program to test\n");
        free(resolvedAbsolutePath);
        free(resolvedCWD);
        exit(EXIT_FAILURE);
    }

    free(rawPath);
    free(rawCWD);
    pRequest->currentWorkingDirectory = resolvedCWD;
    pRequest->subjectPath = resolvedAbsolutePath;
}
