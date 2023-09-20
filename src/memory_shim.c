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
#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

typedef void* (*a_malloc)(size_t size);
typedef void* (*a_free)(void* ptr);

struct allocation {
    void* address;
    size_t size;
    int isFreed;
    struct allocation* nextAllocation;
};
typedef struct allocation Allocation;
static Allocation* allocationHead = NULL;

static a_malloc original_malloc = NULL;
static a_free original_free = NULL;

static void init_originals(void);
void* malloc(size_t);
void free(void* ptr);
static void record_free(void* memoryBlock);
static void record_allocation(void* memoryBlock, size_t size);
static void print_allocations(void);

__attribute__((constructor)) void shim_init()
{
    static int initializing = 0;
    assert(initializing == 0);
    if (initializing) {
        fprintf(stderr, "Tried to shim_init() while initializing");
        exit(EXIT_FAILURE);
    }

    assert(original_malloc == NULL);
    assert(original_free == NULL);
    if ((original_malloc == NULL || original_free == NULL)) {
        initializing = 1;
        init_originals();
        initializing = 0;
    }

    printf("Shim Init!\n");
}

__attribute__((destructor)) void shim_destroy()
{
    assert(original_malloc != NULL);
    assert(original_free != NULL);
    printf("Shim Done!\n");
}

static void init_originals(void)
{

    original_malloc = dlsym(RTLD_NEXT, "malloc");
    original_free = dlsym(RTLD_NEXT, "free");

    if (original_malloc == NULL || original_free == NULL) {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
}

void* malloc(size_t size)
{
    assert(original_malloc != NULL);
    assert(original_free != NULL);

    // static int initializing = 0;
    // if (initializing) {
    //     fprintf(stderr, "Tried to malloc while initializing");
    //     exit(EXIT_FAILURE);
    // }

    // if ((original_malloc == NULL || original_free == NULL)) {
    //     initializing = 1;
    //     init_originals();
    //     initializing = 0;
    // }

    void* requestedMemoryBlock = original_malloc(size);
    if (! requestedMemoryBlock) {
        fprintf(stderr, "Error in `original_malloc`: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }



    return requestedMemoryBlock;
}

static void record_allocation(void* memoryBlock, size_t size)
{
    Allocation* newAllocation = original_malloc(sizeof(Allocation));
    if (! newAllocation) {
        fprintf(stderr, "Error in `original_malloc, failed to record memory leak`: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    newAllocation->address = memoryBlock;
    newAllocation->size = size;
    newAllocation->isFreed = 0;
    newAllocation->nextAllocation = allocationHead;

    allocationHead = newAllocation;

}

void free(void* requestedMemoryBlock)
{
    assert(original_malloc != NULL);
    assert(original_free != NULL);

    // static int initializing = 0;
    // if (initializing) {
    //     fprintf(stderr, "Tried to free while initializing");
    //     exit(EXIT_FAILURE);
    // }

    // if ((original_malloc == NULL || original_free == NULL)) {
    //     initializing = 1;
    //     init_originals();
    //     initializing = 0;
    // }
    record_free(requestedMemoryBlock);
    original_free(requestedMemoryBlock);
}

static void record_free(void* memoryBlock)
{
    Allocation* currentAllocation = allocationHead;
    while (! currentAllocation) {
        if (currentAllocation->address == memoryBlock) {
            currentAllocation->isFreed = 1;
            return;
        }
        currentAllocation = currentAllocation->nextAllocation;
    }

}

static void print_allocations(void)
{
    Allocation* currentAllocation = allocationHead;
    int leakCount = 0;
    while (! currentAllocation) {
        if (currentAllocation->isFreed) {
            leakCount++;
            fprintf("stderr","LEAK\t%zu\n",currentAllocation->size);
        }
        currentAllocation = currentAllocation->nextAllocation;
    }
    fprintf("stderr","TOTAL\t%d\t%zu\n",leakCount, currentAllocation->size);
}