#include "request.h"

#include "arg_utils.h"
#include "common_libs.h"
#include "utils.h"

#include <string.h>

void freeRequest(Request * const pRequest)
{
    for (int i = 0; i < pRequest->subjectArgc; i++) {
        free((char *)pRequest->subjectArgs[i]);
    }
    free(pRequest->subjectArgs);
    free(pRequest->currentWorkingDirectory);
    // free(pRequest->thisProgramPath);
    free(pRequest->subjectCall);
    free(pRequest->subjectPath);
}

size_t my_strlcpy(char * const dest, char const * const src, size_t size)
{
    size_t length = 0;
    while (src[length] != '\0') {
        length++;
    }
    if (size == 0) {
        return length;
    }
    size_t const endIndex = size - 1;
    for (size_t i = 0; (i < endIndex) && (src[i]); i++) {
        dest[i] = src[i];
    }
    dest[endIndex] = '\0';
    return length;
}

char * simpleArgv(char const *** argv_pointers, int * newArgc, int argc, char const ** const argv)
{
    // ### seperated in memory ### //

    // size_t const NULL_TERMINATOR_SIZE = 1;
    // int i, j;
    // char ** newArgv = (char **)malloc(argc * sizeof(char *));
    // if (! newArgv) {
    //     perror("Failed to allocate memory for newArgv");
    //     exit(EXIT_FAILURE);
    // }
    // for (i = 0; i < argc; i++) {
    //     size_t iLength = strlen(argv[i]) + NULL_TERMINATOR_SIZE;
    //     newArgv[i] = (char * const)malloc(iLength * sizeof(char));
    //     if (! newArgv[i]) {
    //         perror("Failed to allocate memory for newArgv[i]");
    //         exit(EXIT_FAILURE);
    //     }
    //     for (j = 0; j < iLength; j++) {
    //         newArgv[i][j] = argv[i][j];
    //     }
    //     newArgv[i][j] = '\0';
    // }
    // return newArgv;

    // ## VLA ## //

    // // find length of longest arg
    // int maxLength = 0;
    // for (int i = 0; i < argc; i++) {
    //     int iLength = strlen(argv[i]);
    //     if (iLength > maxLength) {
    //         maxLength = iLength;
    //     }
    // }
    // size_t const NULL_TERMINATOR_SIZE = 1;
    // char newArgv[argc][maxLength + NULL_TERMINATOR_SIZE];

    // for (int i = 0; i < argc; i++) {
    //     my_strlcpy(newArgv[i], argv[i], maxLength + NULL_TERMINATOR_SIZE);
    // }

    // return (char**)newArgv;

    // ### dynamically allocated null terminator seperated single memory block ### //
    size_t const NULL_TERMINATOR_SIZE = 1;
    size_t totalLength = 0;
    int i, j;

    // Calculate total length required
    for (i = 0; i < argc; i++) {
        totalLength += strlen(argv[i]) + NULL_TERMINATOR_SIZE;
    }

    // Allocate memory for concatenated strings
    size_t dataSize = totalLength * sizeof(char);
    if (dataSize == 0) {
        perror("Failed to allocate memory for newArgv");
        exit(EXIT_FAILURE);
    }
    char * newArgv = malloc(dataSize);
    if (! newArgv) {
        perror("Failed to allocate memory for newArgv");
        exit(EXIT_FAILURE);
    }

    size_t indexesSize = argc * sizeof(char *);
    if (indexesSize == 0) {
        perror("Failed to allocate memory for argv_pointers");
        exit(EXIT_FAILURE);
    }
    *argv_pointers = malloc(indexesSize);
    if (! *argv_pointers) {
        perror("Failed to allocate memory for argv_pointers");
        exit(EXIT_FAILURE);
    }

    // Copy strings into newArgv
    char * charIterator = newArgv;
    for (i = 0; i < argc; i++) {
        (*argv_pointers)[i] = charIterator;
        for (j = 0; argv[i][j] != '\0'; j++) {
            *charIterator = argv[i][j];
            charIterator++;
        }
        *charIterator = '\0';
        charIterator++;
        (*newArgc)++;
    }

    return newArgv;
}

void processArgs(Request * const pRequest, int const argc, char const ** const argv)
{

    size_t const NULL_TERMINATOR_SIZE = 1;
    size_t const SPACE_SIZE = 1;
    size_t const MAX_PATH_SIZE = 4096;

    if (argc == 1) {
        printUsage("ERROR: No arguments given.\n");
        exit(EXIT_FAILURE);
    }

    // ## Restructuring subject program call ### //

    char const ** argv_pointers = NULL;
    int newArgc = 0;
    char * argv_data = simpleArgv(&argv_pointers, &newArgc, argc, argv);
    assert(argc == newArgc);
    standardizeSubjectArgs(pRequest, newArgc, &argv_pointers, argv_data);
    free(argv_data);
    free(argv_pointers);

    // // ### Determining File Paths ### //

    // char* rawCWD = argv_pointers[0];
    // char* rawSubjectPath = pRequest->subjectArgs[0];

    deriveSubjectPath(pRequest, pRequest->subjectArgs[0]);

    // ### Determining Program Call ### //

    pRequest->subjectCall = (char *)malloc(MAX_PATH_SIZE);
    snprintf(pRequest->subjectCall, MAX_PATH_SIZE, "%s", pRequest->subjectPath);


    // If no arguments given for requested program, can return before processing
    if (pRequest->subjectArgc == 1) {
        return;
    }

    // If arguments given for requested program, append them to program call
    for (int i = 1; i < pRequest->subjectArgc; ++i) {

        size_t newLength = strlen(pRequest->subjectCall) + strlen(pRequest->subjectArgs[i]) + SPACE_SIZE + NULL_TERMINATOR_SIZE;
        pRequest->subjectCall = (char *)realloc(pRequest->subjectCall, newLength);

        if (! pRequest->subjectCall) {
            perror("ERROR: Memory allocation failed");
            printUsage("");
            exit(EXIT_FAILURE);
        }

        strncat(pRequest->subjectCall, " ", SPACE_SIZE);
        strncat(pRequest->subjectCall, pRequest->subjectArgs[i], strlen(pRequest->subjectArgs[i]));
    }


    return;
}
