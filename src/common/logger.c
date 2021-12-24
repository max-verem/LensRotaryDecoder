#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#define INVALID_HANDLE_VALUE NULL
#else /* __linux__ */
#include <windows.h>
#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib") 
#endif /* __linux__ */
#include "logger.h"
#include "strl.h"

#define LOCK_TIMEOUT            10000           /**< lock ackqure timeout, miliseconds */
#define MAX_MSG_LEN             128000          /**< maximal message length */
#define MAX_MSG_CNT             500000          /**< maximum messages in queue */

static char MSG[MAX_MSG_LEN + 32];
static int f_exit = 0;                          /**< queue thread finish flag */
#ifdef __linux__
static pthread_mutex_t lock_static = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t* lock = &lock_static;
static pthread_t* th_h = NULL;
#else /* __linux__ */
static HANDLE lock = INVALID_HANDLE_VALUE;      /**< module lock */
static HANDLE th_h = INVALID_HANDLE_VALUE;      /**< thread handle */
static unsigned long th_id = 0;                 /**< thread id */
#endif /* __linux__ */
static char** msg_q_list = NULL;                /**< module messages queue */
static int msg_q_tail;                          /**< message tail pointer */
static time_t rot_last = 0;                     /**< last log file rotation process */
static time_t rot_period;
static char log_file_name[PATH_MAX];
static int dup_to_stderr = 1;

#ifdef __linux__
#define LOCK_LOGGER ((lock)?pthread_mutex_lock(lock):-1)
#define UNLOCK_LOGGER pthread_mutex_unlock(lock)
#define Sleep(VAR) usleep(1000*VAR)
#define _vsnprintf vsnprintf
#define _snprintf snprintf
#define MoveFile rename
static unsigned long long timeGetTime()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return
        ((unsigned long long)tv.tv_sec)*1000LL
        +
        ((unsigned long long)tv.tv_usec)/1000LL;
};
#else
#define LOCK_LOGGER WaitForSingleObject(lock, INFINITE)
#define UNLOCK_LOGGER ReleaseMutex(lock)
#endif /* __linux__ */

static char* fetch_msg()
{
    int r;
    char* buf = NULL;

    /* lock struct */
    r = LOCK_LOGGER;

    /* check if lock is success */
    if(0 == r)
    {
        /* check if head is present */
        if(NULL != msg_q_list[0])
        {
            /* save string pointer to file */
            buf = msg_q_list[0];
            msg_q_tail--;

            /* move momory */
            memmove
            (
                &msg_q_list[0],
                &msg_q_list[1],
                sizeof(char*) * MAX_MSG_CNT
            );

            /* clear last element */
            msg_q_list[MAX_MSG_CNT] = NULL;
        };

        /* realese mutex */
        UNLOCK_LOGGER;
    };

    return buf;
};

static void output_msg(char* buf)
{
    struct stat st;
    static char *templ = "-%Y%m%d_%H%M%S";
    char* file_name = NULL;
    time_t ltime;
    struct tm *rtime;
    FILE* f;

    /* output to console */
    if(0 != dup_to_stderr)
        fputs(buf, stderr);

    /* check if target file is empty */
    if(!log_file_name[0])
        return;

    /* check if log file should be rotated */
    time( &ltime );
    if(rot_last + rot_period < ltime)
    {
        /* allocate file name */
        file_name = (char*)malloc(PATH_MAX + 1);

        /* compose file name */
        strlcpy(file_name, log_file_name, PATH_MAX);

        /* append date to filename */
        rtime = localtime( &ltime );
        strftime
        (
            file_name + strlen(file_name),
            PATH_MAX - strlen(file_name),
            templ,
            rtime
        );    /* append time */

        /* rename */
        MoveFile(log_file_name, file_name);

        /* free file name */
        free(file_name);

        /* assign new time */
        rot_last = ltime;
    };

    /* try to create or append file */
    if(stat(log_file_name, &st) < 0)
    {
        char *p = strdup(log_file_name), *t;

        for(t = p;;)
        {
            t = strchr(t, '/');

            if(!t)
                break;

            *t = 0;

            mkdir(p, 0777);

            *t = '/';

            t++;
        };

        free(p);
    };
    if(NULL != (f = fopen(log_file_name, "a")))
    {
        fputs(buf, f);
        fclose(f);
    };
};

static void process_queue()
{
    char* buf;

    while(NULL != (buf = fetch_msg()))
    {
#if 1
        /* output msg */
        output_msg(buf);
#else
        fputs(buf, stderr);
#endif

        /* free mem of message */
        free(buf);
    };
};

#ifdef __linux__
static void* logger_queue_proc(void* desc_)
#else /* __linux__ */
static unsigned long WINAPI logger_queue_proc(void* desc_)
#endif /* __linux__ */
{
    while(0 == f_exit)
    {
        process_queue();
        Sleep(1);
    };

    /* ensure all messages sent */
    process_queue();

    return 0;
};

int logger_dup(int f)
{
    return (dup_to_stderr = f);
};

int logger_init(char* log_file, int rotate_interval)
{
    /* store arguments */
    rot_period = rotate_interval;
    strlcpy(log_file_name, log_file, PATH_MAX);

    /* create mutex */
#ifdef __linux__
#else /* __linux__ */
    lock = CreateMutex(NULL,FALSE,NULL);
#endif /* __linux__ */

    /* setup queue state */
    msg_q_tail = 0;
    msg_q_list = (char**)malloc(sizeof(char*) * (MAX_MSG_CNT + 1));
    memset(msg_q_list, 0, sizeof(char*) * (MAX_MSG_CNT + 1));

    /* create thread */
#ifdef __linux__
    th_h = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_create
    (
        th_h,
        NULL,
        logger_queue_proc,
        NULL
    );
#else /* __linux__ */
    th_h = CreateThread
    (
        NULL,                   /* LPSECURITY_ATTRIBUTES lpThreadAttributes,*/
        8096,                   /* SIZE_T dwStackSize,                        */
        logger_queue_proc,      /* LPTHREAD_START_ROUTINE lpStartAddress,    */
        NULL,                   /* LPVOID lpParameter,                        */
        0,                      /* DWORD dwCreationFlags,                    */
        &th_id                  /* LPDWORD lpThreadId                        */
    );
#endif /* __linux__ */

    return 0;
};

int logger_release()
{
    int r, i;

    /* check if initialized */
    if
    (
        (INVALID_HANDLE_VALUE == th_h)
        ||
        (INVALID_HANDLE_VALUE == lock)
        ||
        (NULL == msg_q_list)
    )
        return -1;

    /* notify thread to exit */
    f_exit = 1;

    /* wait for thread end */
#ifdef __linux__
    pthread_join(*th_h, (void**)&r);
    free(th_h);
#else /* __linux__ */
    WaitForSingleObject(th_h, INFINITE);
    CloseHandle(th_h);
#endif /* __linux__ */

    /* lock logger */
    r = LOCK_LOGGER;
    if(0 != r)
        return -2;

    /* destroy mutex */
#ifdef __linux__
#else /* __linux__ */
    CloseHandle(lock);
#endif /* __linux__ */

    /* destroy queue and messages */
    for(i = 0; i<MAX_MSG_CNT; i++)
        if(NULL != msg_q_list[i])
            free(msg_q_list[i]);
    free(msg_q_list);

    return (0 == r)?0:-1;
};

int logger_printf(int type, const char* message, ...)
{
    int r;
    char* buf;
    unsigned long mili_seconds;
    va_list args;
    static char *templ = "[%Y-%m-%d %H:%M:%S";
    static char *mili_seconds_template = ".%.03d] ";
    struct tm *rtime;
    time_t ltime;

    /* save time */
    mili_seconds = timeGetTime();
    time( &ltime );

    /* lock struct */
    r = LOCK_LOGGER;

    /* set space for buffer */
    buf = MSG;

    /* put error code */
    buf[0] = ' ';
    buf[1] = ' ';
    buf[2] = 0;
    switch(type)
    {
        case 1: buf[0] = '!'; break;
        case 2: buf[0] = '?'; break;
    };

    /* put date into buf */
    rtime = localtime( &ltime );                                /* local */
    strftime(buf + 2, MAX_MSG_LEN - 2, templ , rtime );         /* append time */

    /* append miliseconds */
    _snprintf
    (
        buf + strlen(buf),
        MAX_MSG_LEN - strlen(buf) - 10,
        mili_seconds_template,
        mili_seconds % 1000
    );

    /* init va arg */
    va_start( args, message);

    /* compose message */
    _vsnprintf
    (
        buf + strlen(buf),
        MAX_MSG_LEN - strlen(buf) - 10,
        message,
        args
    );

    /* add newline to message */
    strcat(buf, "\n");

    /* check if lock is success */
    if(0 == r)
    {
        if(msg_q_tail < MAX_MSG_LEN)
        {
            /* store current message in queue */
            msg_q_list[msg_q_tail] = strdup(buf);
            msg_q_tail++;
        }
        else
            /* notify about queue overflow */
            fprintf(stderr, "ERROR! message queue OVERFLOW!!!\n");
    }
    else
        fputs(buf, stderr);

    /* realese mutex */
    UNLOCK_LOGGER;

    /* init va arg */
    va_end(args);

    return 0;
};

static void logger_message_v(int level, const char* file, const int line, const char* function, const char *fmt, va_list ap)
{
    int r, l, m;
    char buf[64];
    struct timeval tv;
    struct tm result;

    /* prepare time */
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &result);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &result);

    /* lock struct */
    r = LOCK_LOGGER;

    /* create message body */
    l = snprintf(MSG, sizeof(MSG), "%s.%.3d [%s:%d:%s] ", buf, (int)(tv.tv_usec / 1000), file, line, function);
    m = vsnprintf(MSG + l, sizeof(MSG) - l, fmt, ap);
    MSG[l + m + 0] = '\n';
    MSG[l + m + 1] = 0;

    /* check if lock is success */
    if(0 == r)
    {
        if(msg_q_tail < MAX_MSG_LEN)
        {
            /* store current message in queue */
            msg_q_list[msg_q_tail] = strdup(MSG);
            msg_q_tail++;
        }
        else
            /* notify about queue overflow */
            fprintf(stderr, "ERROR! message queue OVERFLOW!!!\n");

    }
    else
        /* output to stderr */
        fputs(MSG, stderr);

    /* realese mutex */
    UNLOCK_LOGGER;
};

static int _log_level = 20;

void logger_message_level(int level)
{
    _log_level = level;
};

void logger_message_send(int level, const char* file, const int line, const char* function, const char *fmt, ...)
{
    va_list ap;

    if(level > _log_level)
        return;

    va_start(ap, fmt);

    logger_message_v(level, file, line, function, fmt, ap);

    va_end(ap);
};
