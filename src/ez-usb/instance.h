#ifndef INSTANCE_H
#define INSTANCE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <pthread.h>
#include <stdint.h>
#include <limits.h>

#include "ez_usb_rot_dec.git.h"
#include "common/instance.h"

#define PRODUCT_NAME "ez_usb_rot_dec"
#define PRODUCT_STRING PRODUCT_NAME "-" GITVERSION ", Copyright by Maksym Veremeyenko (c) 2021"

#define WS_LISTEN_PORT          8070

#define MAX_TRANSFER_CNT        2048

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

    struct
    {
        pthread_cond_t cond;
        int queue1_count, queue2_count, errors;
        void *queue1_data[MAX_TRANSFER_CNT], *queue2_data[MAX_TRANSFER_CNT];
    } xfr;

    struct
    {
        int64_t samples, counters[3];
        uint8_t prev[3];
    } decoder;

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
