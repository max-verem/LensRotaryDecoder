#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdint.h>

int utils_mkdir_p(char* path, int mode);
int utils_rmdir_r(char* path);
int utils_load_file(char* filename, void** pbuf, int *size);
int utils_free_space(char* path, uint64_t* free_space, uint64_t* total_space);
char* utils_canonize_filename(char* src, char* dst);
char* utils_find_local_file(char* dst, int len, char* file, ...);
int64_t utils_timeval_diff_us(struct timeval *B, struct timeval *A);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
