#ifndef STRL_H
#define STRL_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

size_t strlcpy(char *dst, const char *src, size_t size);
size_t strlcat(char *dst, const char *src, size_t size);
char* utils_cleanup_mediaid(char* src);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
