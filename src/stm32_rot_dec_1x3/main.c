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

#include "stm32_rot_dec_1x3.h"

static int f_exit = 0;

static void sighandler (int sig)
{
    /* notify */
    logger_printf(1, "%s: [%d] catched %d", __FUNCTION__, getpid(), sig);
    f_exit++;
};

static void sighandler_ignore (int sig)
{
    /* notify */
    logger_printf(1, "%s: [%d] catched %d", __FUNCTION__, getpid(), sig);
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
    pthread_t th;

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

        /* run */
        pthread_create(&th, 0, stm32_rot_dec_proc, instance);

        /* start ws */
//        ws_server_run(instance);

        /* main loop */
        while(!f_exit)
        {
            if(!instance->f_demonize)
                fprintf(stderr, "| %10d | %10d | %10d |\r",
                    instance->values[0], instance->values[1], instance->values[2]);

            usleep(100000);
        };

        logger_printf(0, "%s: Finishing...", __FUNCTION__);

        /* stop ws */
//        ws_server_finish(instance);

        /* wait for vrpn threads */
        pthread_join(th, NULL);

        pthread_mutex_destroy(&instance->lock);

        instance_release(&instance);

        /* exit notify */
        logger_printf(0, "Bye!");

        logger_release();
    };

    instance_release(&instance);

    return r;
};

