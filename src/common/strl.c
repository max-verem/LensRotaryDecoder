#include <string.h>

static void* memcpy_local(void* dest, const void* src, size_t count)
{
    char* dst8 = (char*)dest;
    char* src8 = (char*)src;

    while (count--) *dst8++ = *src8++;

    return dest;
};

// https://en.wikibooks.org/wiki/C_Programming/C_Reference/string.h/strlen
static size_t strlen_local(const char *str, size_t size)
{
    const char *s;
    for (s = str; *s && size - (s - str); ++s) {}
    return(s - str);
};

// http://stackoverflow.com/questions/18547251/when-i-use-strlcpy-function-in-c-the-compilor-give-me-an-error

size_t strlcat(char *dst, const char *src, size_t size)
{
    size_t srclen;
    size_t dstlen;

    dstlen = strlen_local(dst, size);
    size -= dstlen + 1;

    if(!size)
        return dstlen;

    srclen = strlen_local(src, size);

    if(srclen > size)
        srclen = size;

    memcpy_local(dst + dstlen, src, srclen);
    dst[dstlen + srclen] = '\0';

    return (dstlen + srclen);
}

size_t strlcpy(char *dst, const char *src, size_t size)
{
    size_t srclen;

    size --;

    srclen = strlen_local(src, size);

    if(srclen > size)
        srclen = size;

    memcpy_local(dst, src, srclen);
    dst[srclen] = '\0';

    return srclen;
};

char* utils_cleanup_mediaid(char* src)
{
    int i, o;

    for(i = 0, o = 0; ; i++)
    {
        src[o] = src[i];

        if(src[i] != '\t' && src[i] != '\n' && src[i] != '\r')
            o++;

        if(!src[i])
            break;
    };

    return src;
};
