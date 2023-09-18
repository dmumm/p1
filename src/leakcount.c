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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
   if (argc == 1) {
      fprintf(stderr, "ERROR: No arguments given.\n");
      fprintf(stderr, "Usage: <path/to/leakcount> <path/to/program> [args for program]\n");
      fprintf(stderr, "Usage example: ./leakcount ./my_test_program\n");
      fprintf(stderr, "Usage example: ./leakcount ./my_test_program arg1 arg2\n");
      return EXIT_FAILURE;
   }

   #define THIS_PROGRAM_PATH argv[0]
   #define TEST_PROGRAM_PATH argv[1]

   char* invokePath = malloc(strlen(THIS_PROGRAM_PATH) + 1);
   strcpy(invokePath, THIS_PROGRAM_PATH);
   strcat(invokePath, " ");

   char* programCall = malloc(strlen(invokePath) + 2);
   strcpy(programCall, invokePath);

   char* testProgramCall = malloc(strlen(TEST_PROGRAM_PATH) + 2);
   for(int i = 1; i < argc; ++i) {

      realloc(testProgramCall, strlen(testProgramCall) + strlen(argv[i]) + 2);
      strcat(testProgramCall, argv[i]);

      if (i != argc - 1) {
         strcat(testProgramCall, " ");
      }

   }

      //TODO: delete debug prints below
   for(int i = 0; i < argc; i++) {
      printf("DEBUG: argv[%d]: %s\n", i, argv[i]);
   }

   printf("Testing following program call: %s\n", testProgramCall);

   FILE* pipe = popen(testProgramCall, "r");
   if (pipe == NULL) {
      perror("Failed to open program to test");
      fprintf(stderr, "Failed to open program to test\n");
      return EXIT_FAILURE;
   }


   return EXIT_SUCCESS;

}