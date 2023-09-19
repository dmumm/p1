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

// from Sorber's tokenize.c

int main()
{
    char* message = "When 900 years,,, old you, reach, look as good, you will not.";
    char* buf = malloc(1024);
    memset(buf, 0, 1024);
    strcpy(buf, message);

    char* token = strtok(buf, ",");
    while (NULL != token) {
        printf("token: %s\n", token);
        token = strtok(NULL, ",");
    }
}
