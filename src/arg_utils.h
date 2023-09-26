#ifndef ARG_UTILS_H
#define ARG_UTILS_H

#include "request.h"

void standardizeSubjectArgs(Request * const request, int const argc, char const *** const argv_pointers, char const * const argv_data);

char const * parseArg(char const *** const pSubjectArgv, char const * pArg, int * const pSubjectArgc);

void appendArg(char const *** const subjectArgv, int * const subjectArgc, char const * const arg);

void deriveSubjectPath(Request *, char const * inputPath);

#endif // ARG_UTILS_H
