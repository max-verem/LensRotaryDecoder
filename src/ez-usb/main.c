#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>

#include "instance.h"

#include "common/logger.h"
#include "common/demonize.h"

#include "ez_usb_rot_dec.h"
#include "freed_sender.h"

static int f_exit = 0;

static void sighandler (int sig)
{
    /* notify */
    fprintf(stderr, "%s: [%d] catched %d\n", __FUNCTION__, getpid(), sig);
    f_exit++;
};

static void sighandler_ignore (int sig)
{
    /* notify */
    fprintf(stderr, "%s: [%d] catched %d\n", __FUNCTION__, getpid(), sig);
};

static void signals()
{
    /* setup handlers */
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGPIPE, sighandler_ignore);
    signal(SIGHUP, sighandler);
    signal(SIGALRM, sighandler);
};


///
int main(int argc, char** argv)
{
    int r;
    instance_t *instance;
    pthread_t th_ez_usb;
    pthread_t th_freed_sender;

    /* init instance */
    instance_init(&instance);
    instance->p_exit = &f_exit;

    /* load arguments */
    r = instance_opts(instance, argc, argv);

    if(!r)
    {
        /* demonize */
        if(instance->f_demonize && !instance->f_nodemonize)
        {
            logger_dup(0);
            demonize();
        };

        /* init logger */
        logger_init(instance->log_file_name, instance->log_rotate_interval);

        logger_printf(0, "%s", PRODUCT_STRING);

        /* setup signals */
        signals();

        /* init lock */
        pthread_mutex_init(&instance->lock, NULL);
        {
            pthread_mutexattr_t mta;

            pthread_mutexattr_init(&mta);
            pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE );
            pthread_mutex_init(&instance->lock, &mta);
            pthread_mutexattr_destroy(&mta);
        };

        /* init conditional variable */
        pthread_cond_init(&instance->cond, NULL);

        /* run ez_usb */
        pthread_create(&th_ez_usb, 0, ez_usb_reader_proc, instance);

        /* run freed_sender */
        pthread_create(&th_freed_sender, 0, freed_sender_proc, instance);

        /* start ws */
//        ws_server_run(instance);

        /* main loop */
        while(!f_exit)
        {
            fprintf(stderr, "| %10" PRId64 " | %10" PRId64 " | %10" PRId64 "| [%15" PRIu64  "  ]\r",
                instance->decoder.counters[0], instance->decoder.counters[1],
                instance->decoder.counters[2], instance->decoder.samples);

            usleep(100000);
        };

        logger_printf(0, "%s: Finishing...", __FUNCTION__);

        /* stop ws */
//        ws_server_finish(instance);

        /* wait for ez-usb reader finished */
        pthread_join(th_ez_usb, NULL);

        /* wait for freed_sender */
        pthread_join(th_freed_sender, NULL);

        pthread_mutex_destroy(&instance->lock);

        pthread_cond_destroy(&instance->cond);

        instance_release(&instance);

        /* exit notify */
        logger_printf(0, "Bye!");

        logger_release();
    };

    instance_release(&instance);

    return r;
};

