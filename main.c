
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
    #include <dlfcn.h>


int main(int argc, char const ** const argv)
{
    void *handle = dlopen("./memory_shim.so", RTLD_NOW);

    static Request request;

    processArgs(&request, argc, argv);

    // fprintf(stderr, "Testing following program call: %s\n", request.subjectCall); // TODO: delete debug
    FILE * testCommand = popen(request.subjectCall, "r");
    if (testCommand == NULL) {
        perror("Failed to open program to test");
        // printUsage("Failed to open program to test\n");
        return EXIT_FAILURE;
    }
    pclose(testCommand);

    freeRequest(&request);
    dlclose(handle);

    return EXIT_SUCCESS;
}
