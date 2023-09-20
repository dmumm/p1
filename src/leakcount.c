/***********************************************************************************

        Author: Dylan Mumm
        Student ID: C18070517
        Project Start Date: 2023-09-11
        Project End Date: TODO

        Professor Name: Jacob Sorber
        Semester: Fall 2023
        Class ID: CPSC 3220
        Class Title: Operating Systems
        Section ID: 001

        Project Name: Project #1: Tracing, System Calls, and Processes

***********************************************************************************/
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct userRequest {
    int argc;
    char** argv;
    char* thisProgramPath;
    char* currentWorkingDirectory;
    char* testProgramPath;
    char* testProgramPathResolved;
    char* testProgramCall;
};
typedef struct userRequest UserRequest;

UserRequest processArgs(int, char**);

void printUsage();

char* cmd_realpath(const char*);

int main(int argc, char** argv)
{
    UserRequest request = processArgs(argc, argv);

    fprintf(stdout, "Testing following program call: %s\n", request.testProgramCall); // TODO: delete debug

    FILE* testCommand = popen(request.testProgramCall, "r");
    if (testCommand == NULL) {
        perror("Failed to open program to test");
        fprintf(stderr, "Failed to open program to test\n");
        return EXIT_FAILURE;
    }
    pclose(testCommand);
    return EXIT_SUCCESS;
}

UserRequest processArgs(int argc, char** argv)
{
    const size_t NULL_TERMINATOR_SIZE = 1;
    const size_t SPACE_SIZE = 1;

    UserRequest newRequest;

    if (argc == 1) {
        fprintf(stderr, "ERROR: No arguments given.\n");
        printUsage();
        exit(EXIT_FAILURE);
    }
    newRequest.argc = argc;
    newRequest.argv = argv;

    // ### Determining File Paths ### //

    // Get path to this program
    newRequest.thisProgramPath = realpath(argv[0], NULL);
    if (! newRequest.thisProgramPath) {
        perror("ERROR: Failed to get real path for argv[0]");
        printUsage();
        exit(EXIT_FAILURE);
    }

    // Get path to current working directory
    const int MAX_PATH_SIZE = 4096;
    char rawCurrentWorkingDirectory[MAX_PATH_SIZE];
    char* getcwdResult = getcwd(rawCurrentWorkingDirectory, sizeof(rawCurrentWorkingDirectory));
    if (! getcwdResult || strlen(rawCurrentWorkingDirectory) == 0) {
        perror("ERROR: getcwd() error");
        exit(EXIT_FAILURE);
    }

    // Process real path to current working directory
    newRequest.currentWorkingDirectory = realpath(rawCurrentWorkingDirectory, NULL);
    if (! newRequest.currentWorkingDirectory) {
        perror("ERROR: Failed to get real path for current directory");
        printUsage();
        exit(EXIT_FAILURE);
    }

    // Derive path to program to test
    newRequest.testProgramPath = malloc(sizeof(char) * MAX_PATH_SIZE);
    if (! newRequest.testProgramPath) {
        perror("ERROR: Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    snprintf(newRequest.testProgramPath, MAX_PATH_SIZE, "%s/%s", newRequest.currentWorkingDirectory, argv[1]);
    size_t length = strlen(newRequest.testProgramPath);
    if (length == 0) {
        fprintf(stderr, "Path to requested program has length 0: %s\n", newRequest.testProgramPath);
        printUsage();
        exit(EXIT_FAILURE);
    }

    // Resolve optimal path to program to test
    newRequest.testProgramPathResolved = cmd_realpath(newRequest.testProgramPath);
    if (! newRequest.testProgramPathResolved) {
        fprintf(stderr, "WARNING: Failed to derive real path to program to test\n");
        fprintf(stderr, "         Falling back to requested path: %s\n", newRequest.testProgramPath);
        newRequest.testProgramPathResolved = newRequest.testProgramPath;
    }
    if (strlen(newRequest.testProgramPathResolved) == 0) {
        fprintf(stderr, "ERROR: Failed to resolve path to program to test\n");
        printUsage();
        exit(EXIT_FAILURE);
    }

    // ### Determining Program Call ### //
    newRequest.testProgramCall = malloc(strlen(newRequest.testProgramPathResolved) + NULL_TERMINATOR_SIZE);
    strcpy(newRequest.testProgramCall, newRequest.testProgramPathResolved);

    // If no arguments given for requested program, can return before processing
    if (argc == 2) {
        return newRequest;
    }

    // If arguments given for requested program, append them to program call
    for (int i = 2; i < argc; ++i) {

        size_t newLength = strlen(newRequest.testProgramCall) + strlen(argv[i]) + SPACE_SIZE + NULL_TERMINATOR_SIZE;
        newRequest.testProgramCall = realloc(newRequest.testProgramCall, newLength);
        if (! newRequest.testProgramCall) {
            perror("ERROR: Memory allocation failed");
            printUsage();
            exit(EXIT_FAILURE);
        }

        strncat(newRequest.testProgramCall, " ", SPACE_SIZE);
        strncat(newRequest.testProgramCall, argv[i], strlen(argv[i]));
    }

    fprintf(stdout, "Processed following program call: %s\n", newRequest.testProgramCall); // TODO: delete debug
    return newRequest;
}

void printUsage()
{
    fprintf(stderr, "Usage: <path/to/leakcount> <path/to/program> [args for program]\n");
    fprintf(stderr, "Usage example: ./leakcount ./my_test_program\n");
    fprintf(stderr, "Usage example: ./leakcount ./my_test_program arg1 arg2\n");
}

char* cmd_realpath(const char* path)
{
    const int MAX_PATH_SIZE = 4096;

    char command[MAX_PATH_SIZE];
    snprintf(command, sizeof(command), "realpath \"%s\"", path);

    FILE* realpathPipe = popen(command, "r");
    if (! realpathPipe) {
        perror("Failed to run command");
        return NULL;
    }

    char resolvedPath[MAX_PATH_SIZE];
    fgets(resolvedPath, MAX_PATH_SIZE, realpathPipe);
    assert(ferror(realpathPipe) == 0);
    if (feof(realpathPipe) || ferror(realpathPipe)) {
        perror("Failed to read command output");
        fprintf(stderr, "Failed to read command output\n");
        pclose(realpathPipe);
        return NULL;
    }
    pclose(realpathPipe);

    size_t length = strlen(resolvedPath);
    if (length == 0) {
        fprintf(stderr, "Resolved path has length 0\n");
        return NULL;
    }
    if (resolvedPath[length - 1] == '\n') {
        resolvedPath[length - 1] = '\0';
    }

    char* result = malloc(length);
    if (! result) {
        perror("Failed to allocate memory for resolved path");
        return NULL;
    }
    strcpy(result, resolvedPath);

    return result;
}
