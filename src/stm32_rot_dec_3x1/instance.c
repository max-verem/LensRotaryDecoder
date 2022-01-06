#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

#include "instance.h"
#include "common/strl.h"

int instance_init(instance_t** pinstance)
{
    instance_t* instance;

    if(!pinstance)
        return -EINVAL;

    instance = (instance_t*)malloc(sizeof(instance_t));

    if(!instance)
        return -ENOMEM;

    memset(instance, 0, sizeof(instance_t));

    *pinstance = instance;

    return 0;
};

int instance_release(instance_t** pinstance)
{
    instance_t* instance;

    if(!pinstance)
        return -EINVAL;

    instance = *pinstance;
    *pinstance = NULL;

    if(!instance)
        return -ENOMEM;

    free(instance);

    return 0;
};

#include "common/opts.h"

static const char short_options [] = "h";
static const struct option long_options [] =
{
    { "ws-port",                        required_argument, NULL, 174},

    DEFAULT_INSTANCE_LONG_OPTS
};

OPTS_FROM_FILE_CB(instance_opts);

int instance_opts(instance_t* instance, int argc, char** argv)
{
    int c, index = 0;

    /* reset datas */
    optind = 0; opterr = 0; optopt = 0;

    /* presetup some defaults */
    if(!instance->log_file_name[0])
        strlcpy(instance->log_file_name, PRODUCT_NAME ".log", PATH_MAX);
    if(!instance->log_rotate_interval)
        instance->log_rotate_interval = 3600* 24;
    if(!instance->ws.port)
        instance->ws.port = WS_LISTEN_PORT;

    while(1)
    {
        c = getopt_long(argc, argv, short_options, long_options, &index);

        if((-1) == c) break;

        switch(c)
        {
            case 174:   /* --ws-port */
                ST_INT(ws.port);

            DEFAULT_INSTANCE_SET_OPTS
        };
    };

    OPTS_FROM_FILE_PROCESS(instance);

    return 0;
};


int instance_usage(instance_t* instance)
{
    fprintf
    (
        stderr,
        PRODUCT_STRING "\n"
        "Usage:\n"
        "    "PRODUCT_NAME" [args]\n"
        "Where args:\n"
        "    --ws-port=<N>                          websocket port\n"
        "    --serial-zoom=<STR>                    serial number of ZOOM usb dongle\n"
        "    --serial-focus=<STR>                   serial number of FOCUS usb dongle\n"
        "    --serial-iris=<STR>                    serial number of IRIS usb dongle\n"
        DEFAULT_INSTANCE_USAGE
    );

    return 0;
};
