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

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

// typedef for the function pointer to the original malloc function
typedef void* (*a_malloc)(size_t size);

__attribute__((constructor)) void shim_init()
{
    printf("Shim Init!\n");
}

__attribute__((destructor)) void shim_destroy()
{
    printf("Shim Done!\n");
}

void* malloc(size_t size)
{
    // get the original malloc function
    a_malloc original_malloc = dlsym(RTLD_NEXT, "malloc");
    if (original_malloc == NULL) {
      fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
      exit(EXIT_FAILURE);
    }

    // call the original malloc function
    void* ptr = original_malloc(size);

    // print the pointer and size
    printf("malloc(%zu) = %p\n", size, ptr);

    // return the pointer
    return ptr;
}
