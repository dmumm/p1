#ifndef REQUEST_H
#define REQUEST_H

typedef struct request {
    char * currentWorkingDirectory;
    // char * thisProgramPath;
    char * subjectPath;
    char const ** subjectArgs;
    int subjectArgc;
    char * subjectCall;
} Request;

void freeRequest(Request * const pRequest);

void processArgs(Request * const pRequest, int const argc, char const ** const argv);

char * simpleArgv(char const *** argv_pointers, int * newArgc, int argc, char const ** const argv);


#endif // REQUEST_H
