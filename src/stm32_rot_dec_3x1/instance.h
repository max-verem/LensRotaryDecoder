#ifndef INSTANCE_H
#define INSTANCE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <pthread.h>
#include <stdint.h>
#include <limits.h>

#include "stm32_rot_dec_3x1.git.h"
#include "common/instance.h"

#define PRODUCT_NAME "stm32_rot_dec_3x1"
#define PRODUCT_STRING PRODUCT_NAME "-" GITVERSION ", Copyright by Maksym Veremeyenko (c) 2022"

#define WS_LISTEN_PORT          8070
#define MAX_TRANSFER_CNT        2048

enum
{
    LENS_PARAM_ZOOM = 0,
    LENS_PARAM_FOCUS,
    LENS_PARAM_IRIS,
    LENS_PARAM_LAST
};


typedef struct instance_desc
{
    /* lock */
    pthread_mutex_t lock;

    struct
    {
        int port;
        pthread_t th;
        void* ctx;
//        ws_queue_t main;
        char tmp[PATH_MAX];
    } ws;

    int32_t values[LENS_PARAM_LAST];

    DEFAULT_INSTANCE_PARAMS
} instance_t;

int instance_init(instance_t** instance);
int instance_release(instance_t** instance);
int instance_opts(instance_t* instance, int argc, char** argv);
int instance_usage(instance_t* instance);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* INSTANCE_H */
