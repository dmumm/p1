
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

// ***********************************************************************************/

#include "common_libs.h"
#include "request.h"
#include "utils.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


// #include <dlfcn.h>

int main(int argc, char const ** const argv)
{
    // void * handle = dlopen("./memory_shim.so", RTLD_NOW);

    static Request request;

    processArgs(&request, argc, argv);

    // // fprintf(stderr, "Testing following program call: %s\n", request.subjectCall); // TODO: delete debug
    // FILE * testCommand = popen(request.subjectCall, "r");
    // if (testCommand == NULL) {
    //     perror("Failed to open program to test");
    //     // printUsage("Failed to open program to test\n");
    //     return EXIT_FAILURE;
    // }
    // pclose(testCommand);

    pid_t pid = fork(); // Fork the current process

    if (pid == -1) {
        perror("Failed to fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) { // Child process
        setenv("LD_PRELOAD", "./memory_shim.so", 1); // Set LD_PRELOAD environment variable

        char * args[] = { "/bin/sh", "-c", request.subjectCall, NULL }; // Assuming request.subjectCall is the command you want to run
        execvp(args[0], args); // Replace the child process image with the new one

        perror("Failed to exec");
        exit(EXIT_FAILURE); // Ensure the child process exits if exec fails
    }
    else { // Parent process
        int status;
        waitpid(pid, &status, 0); // Wait for the child process to complete. Remove this line if you don't want to wait.
    }

    freeRequest(&request);

    return EXIT_SUCCESS;
}
