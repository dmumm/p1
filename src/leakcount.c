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
#include <limits.h>
#include <unistd.h>

#define PATH_MAX 4096

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

int main(int argc, char** argv) {

   UserRequest request = processArgs(argc, argv);

   // TODO: delete debug prints below
   fprintf(stdout, "Testing following program call: %s\n", request.testProgramCall);

   FILE* testCommand = popen(request.testProgramCall, "r");
   if (testCommand == NULL) {
      perror("Failed to open program to test");
      fprintf(stderr, "Failed to open program to test\n");
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;

}

UserRequest processArgs(int argc, char** argv) {

   const size_t NULL_TERMINATOR_SIZE = 1;
   const size_t SPACE_SIZE = 1;

   UserRequest new;

   if (argc == 1) {
      fprintf(stderr, "ERROR: No arguments given.\n");
      printUsage();
      exit(EXIT_FAILURE);
   }
   new.argc = argc;
   new.argv = argv;

   // ### Determining File Paths ### //

   // Get path to this program
   new.thisProgramPath = realpath(argv[0], NULL);
   if (NULL == new.thisProgramPath) {
      perror("ERROR: Failed to get real path for argv[0]");
      printUsage();
      exit(EXIT_FAILURE);
   }

   // Get path to current working directory
   char cwd[PATH_MAX];
   if (getcwd(cwd, sizeof(cwd)) == NULL) {
      perror("ERROR: getcwd() error");
      exit(EXIT_FAILURE);
   }

   // Process real path to current working directory
   new.currentWorkingDirectory = realpath(cwd, NULL);
   if (NULL == new.currentWorkingDirectory) {
      perror("ERROR: Failed to get real path for current directory");
      printUsage();
      exit(EXIT_FAILURE);
   }

   // Derive path to program to test
   new.testProgramPath = malloc(sizeof(char) * PATH_MAX);
   if (NULL == new.testProgramPath) {
      perror("ERROR: Memory allocation failed");
      exit(EXIT_FAILURE);
   }
   snprintf(new.testProgramPath, PATH_MAX, "%s/%s", new.currentWorkingDirectory, argv[1]);
   size_t len = strlen(new.testProgramPath);
   if (len == 0) {
      fprintf(stderr, "Path to requested program has length 0: %s\n", new.testProgramPath);
      printUsage();
      exit(EXIT_FAILURE);
   }

   // Resolve optimal path to program to test
   new.testProgramPathResolved = cmd_realpath(new.testProgramPath);
   if (NULL == new.testProgramPathResolved) {
      fprintf(stderr, "WARNING: Failed to derive real path to program to test\n");
      fprintf(stderr, "         Falling back to requested path: %s\n", new.testProgramPath);
      new.testProgramPathResolved = new.testProgramPath;
   }
   if (strlen(new.testProgramPathResolved) == 0) {
      fprintf(stderr, "ERROR: Failed to resolve path to program to test\n");
      printUsage();
      exit(EXIT_FAILURE);
   }

   // // Remove newline from resolved path
   // char* lastChar = &new.testProgramPathResolved[len - 1];
   // if (*lastChar == '\n') {
   //    *lastChar = '\0';
   // }

   // ### Determining Program Call ### //
   new.testProgramCall = malloc(strlen(new.testProgramPathResolved) + NULL_TERMINATOR_SIZE);
   strcpy(new.testProgramCall, new.testProgramPathResolved);

   // If no arguments given for requested program, can return before processing
   if(argc == 2) {
      fprintf(stdout, "Processed following program call: %s\n", new.testProgramCall);// TODO: delete debug print below
      return new;
   }

   // If arguments given for requested program, append them to program call
   for(int i = 2; i < argc; ++i) {

      size_t newLength = strlen(new.testProgramCall) + strlen(argv[i]) + SPACE_SIZE + NULL_TERMINATOR_SIZE;
      new.testProgramCall = realloc(new.testProgramCall, newLength);
      if (NULL == new.testProgramCall) {
         perror("ERROR: Memory allocation failed");
         printUsage();
         exit(EXIT_FAILURE);
      }

      strncat(new.testProgramCall, " ", SPACE_SIZE);
      strncat(new.testProgramCall, argv[i], strlen(argv[i]));

   }

   fprintf(stdout, "Processed following program call: %s\n", new.testProgramCall); // TODO: delete debug
   return new;

}

void printUsage() {

   fprintf(stderr, "Usage: <path/to/leakcount> <path/to/program> [args for program]\n");
   fprintf(stderr, "Usage example: ./leakcount ./my_test_program\n");
   fprintf(stderr, "Usage example: ./leakcount ./my_test_program arg1 arg2\n");

}


char* cmd_realpath(const char* path) {

   char command[PATH_MAX];
   snprintf(command, sizeof(command), "realpath \"%s\"", path);

   FILE* fp = popen(command, "r");
   if (fp == NULL) {
      perror("Failed to run command");
      return NULL;
   }

    static char resolvedPath[PATH_MAX];
    fgets(resolvedPath, sizeof(resolvedPath), fp);
    if (resolvedPath == NULL) {
      perror("Failed to read command output");
      fprintf(stderr, "Failed to read command output\n");
        pclose(fp);
        return NULL;
    }

   size_t len = strlen(resolvedPath);
   if (len == 0) {
      fprintf(stderr, "Resolved path has length 0\n");
      pclose(fp);
      return NULL;
    }

    if (resolvedPath[len-1] == '\n') {
      resolvedPath[len-1] = '\0';
    }

    pclose(fp);
    return resolvedPath;
}
