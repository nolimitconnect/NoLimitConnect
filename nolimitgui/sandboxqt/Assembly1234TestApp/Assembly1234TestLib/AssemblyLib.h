#ifndef ASSEMBLYLIB_H
#define ASSEMBLYLIB_H

#include <stdint.h>

#ifdef __cplusplus
const char* getCppText();
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

const char* getCText();

int64_t get_assembly_test_num();
int64_t get_assembly_reverse_num();

#ifdef __cplusplus
}
#endif


#endif // ASSEMBLYLIB_H
