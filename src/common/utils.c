#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/statvfs.h>
#include <time.h>

#include "logger.h"
#include "utils.h"
#include "strl.h"

int64_t utils_timeval_diff_us(struct timeval *B, struct timeval *A)
{
    int64_t r;

    r = B->tv_sec - A->tv_sec;
    r *= 1000000LL;
    r += B->tv_usec - A->tv_usec;

    return r;
};

int utils_mkdir_p(char* path, int mode)
{
    int r;
    char* p;

    /* check empty string */
    if(0 == *path)
        return 0;

    for(r = 0, p = path + 1; (0 != (*p)) ; p++)
        if('/' == (*p))
        {
            *p = 0;

            r = mkdir(path, mode);

            *p = '/';
        };

    r = mkdir(path,mode);

    return ((0 == r) || (EEXIST == errno))?0:-errno;
};

int utils_rmdir_r(char* path)
{
    int r;
    DIR* dir = opendir(path);
    struct dirent *dir_ent;

    if(!dir)
    {
        r = -errno;
        logger_printf(1, "utils_rmdir_r: opendir(%s) failed, errno=%d (%s)", path, r, strerror(-r));
        return r;
    };

    /* read directoty content */
    while((dir_ent = readdir(dir)))
    {
        struct stat st;
        char* f;

        /* skip dirs */
        if(!strcmp(dir_ent->d_name, ".") || !strcmp(dir_ent->d_name, ".."))
            continue;

        /* build full filename */
        f = malloc(PATH_MAX);
        snprintf(f, PATH_MAX, "%s/%s", path, dir_ent->d_name);

        /* stat file */
        if(-1 == stat(f, &st))
        {
            r = -errno;
            logger_printf(1, "utils_rmdir_r: stat(%s) failed, errno=%d", f, r);
            free(f);
            continue;
        };

        if(S_ISDIR(st.st_mode))
            utils_rmdir_r(f);
        else
        {
            r = remove(f);
            if(r)
            {
                r = -errno;
                logger_printf(1, "utils_rmdir_r: remove(%s) failed, errno=%d (%s)", f, r, strerror(-r));
            }
            else
                logger_printf(1, "utils_rmdir_r: remove(%s)", f);

        };

        free(f);
    };

    closedir(dir);

    r = rmdir(path);
    if(r)
    {
        r = -errno;
        logger_printf(1, "utils_rmdir_r: rmdir(%s) failed, errno=%d (%s)", path, r, strerror(-r));
    }
    else
        logger_printf(1, "utils_rmdir_r: rmdir(%s)", path);

    return r;
};

int utils_load_file(char* filename, void** pbuf, int *size)
{
    int r;
    FILE* f;
    void* buf;
    struct stat st;

    /* stat file */
    if(-1 == stat(filename, &st))
    {
        r = errno;
        logger_printf(1, "utils_load_file: stat(%s) failed, r=%d: %s",
            filename, r, strerror(r));
        return -r;
    };

    /* check if it regular file */
    if(!S_ISREG(st.st_mode))
        return -ENOENT;

    /* open that file */
    f = fopen(filename, "rb");
    if(!f)
    {
        int e = errno;
        logger_printf(1, "utils_load_file: failed to open '%s'", filename);
        return -e;
    };

    /* allocate buffer */
    buf = malloc(st.st_size + 1);
    if(!buf)
        r = -ENOMEM;
    else
    {
        memset(buf, 0, st.st_size + 1);
        fread(buf, 1, st.st_size, f);
    };

    fclose(f);

    *pbuf = buf;
    *size = st.st_size;

    return 0;
};

int utils_free_space(char* path, uint64_t* free_space, uint64_t* total_space)
{
    int r;
    uint64_t s;
    struct statvfs vfs;

    r = statvfs(path, &vfs);

    if(r) return -errno;

    s = vfs.f_bavail;
    s *= vfs.f_bsize;
    *free_space = s;

    s = vfs.f_blocks;
    s *= vfs.f_frsize;
    *total_space = s;

    return 0;
};

char* utils_canonize_filename(char* src, char* dst_origin)
{
    int i, r, l;
    char* dst;
    const static char* tokens[] =
    {
        "/../", "/",
        "/./",  "/",
        "\\",   "",
        "\"",   "",
        "<",    "",
        ">",    "",
        "&",    "",
        "//",   "/",
        NULL
    };

    /* prep destination */
    if(dst_origin)
        dst = dst_origin;
    else
        dst = (char*)malloc(PATH_MAX);
    strlcpy(dst, src, PATH_MAX - 1);

    /* add starting slash */
    if(dst[0] != '/')
    {
        l = strlen(dst);
        memmove(dst + 1, dst, l + 1);
        dst[0] = '/';
    };

    /* drop tokens */
    for(i = 0; tokens[i]; i += 2)
    {
        char* p;

        while((p = strstr(dst, tokens[i])))
        {
            l = strlen(tokens[i]);
            r = strlen(tokens[i + 1]);
            memmove(p + r, p + l, strlen(p) - l + 1);
            memcpy(p, tokens[i + 1], r);
        };
    };

    return dst;
};

char* utils_find_local_file(char* dst, int len, char* file, ...)
{
    va_list ap;
    char path[PATH_MAX + 1], *tmp;

    memset(path, 0, sizeof(path));
    readlink("/proc/self/exe", path, PATH_MAX);

    tmp = strrchr(path, '/');
    if(tmp)
        *(tmp + 1) = 0;
    else
        path[0] = 0;


    va_start(ap, file);
    while(1)
    {
        struct stat s;
        char* rel = va_arg(ap, char *);

        if(!rel)
        {
            strlcpy(dst, file, len);
            break;
        };

        snprintf(dst, len, "%s%s/%s", path, rel, file);
        if(!stat(dst, &s))
            break;
    };
    va_end(ap);

    return dst;
};
