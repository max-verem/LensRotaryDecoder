#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>

#include "instance.h"
#include "common/logger.h"

#include "common/FreeD.h"

void* freed_sender_proc(void* p)
{
    FreeD_D1_t freed;
    int r, i, freed_socket, freed_socket_broadcast = 1;
    unsigned char buf_packet[FREE_D_D1_PACKET_SIZE];
    instance_t* instance = (instance_t*)p;
    struct sockaddr_in addrs[MAX_FREED_TARGETS];

    logger_printf(0, "%s: Entering....", __FUNCTION__);

    memset(addrs, 0, sizeof(addrs));
    memset(&freed, 0, sizeof(FreeD_D1_t));

    if(instance->freed.div < 1) instance->freed.div = 1;

    /* init socket */
    freed_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (freed_socket <= 0)
    {
        logger_printf(0, "%s: socket failed", __FUNCTION__);
        return NULL;
    };
    setsockopt(freed_socket, SOL_SOCKET, SO_BROADCAST, &freed_socket_broadcast, sizeof(freed_socket_broadcast));

    /* prepare addr struct */
    for(i = 0; i < instance->freed.trgs; i++)
    {
        char *port, *host;
        struct sockaddr_in *addr = &addrs[i];

        if(!instance->freed.targets[i][0])
            continue;

        /* reset addr */
        addr->sin_family = 0;

        /* parse target */
        host = strdup(instance->freed.targets[i]);
        logger_printf(1, "%s: new target [%s]", __FUNCTION__, host);
        port = strrchr(host, ':');
        if(port)
        {
            memset(addr, 0, sizeof(struct sockaddr_in));

            *port = 0; port++;

            /* prepare address */
            addr->sin_family = AF_INET;
            addr->sin_addr.s_addr = inet_addr(host);
            addr->sin_port = htons((unsigned short)atoi(port));
        };

        free(host);
    };

    /* main loop */
    while(!(*instance->p_exit))
    {
        int Iris;
        struct timespec to;

        /* wait for tick */
        pthread_mutex_lock(&instance->lock);
        clock_gettime(CLOCK_REALTIME, &to);
        to.tv_sec += 1;
        r = pthread_cond_timedwait(&instance->xfr.cond, &instance->lock, &to);
        pthread_mutex_unlock(&instance->lock);

        instance->freed.cnt++;

        if(instance->freed.cnt % instance->freed.div)
            continue;

        /* prepare struct */
        Iris = instance->decoder.counters[2];
        freed.Zoom = instance->decoder.counters[0];
        freed.Focus = instance->decoder.counters[1];
        freed.Spare[0] = (Iris >> 8) && 0x00FF;
        freed.Spare[1] = (Iris >> 0) && 0x00FF;
        freed.ID = instance->freed.id;

        /* build packet */
        FreeD_D1_pack(buf_packet, FREE_D_D1_PACKET_SIZE, &freed);

        /* send packet to targets */
        for(i = 0; i < MAX_FREED_TARGETS; i++)
        {
            struct sockaddr_in *addr = &addrs[i];

            if(!addr->sin_family)
                continue;

            r = sendto
            (
                freed_socket,               /* Socket to send result */
                (char*)buf_packet,          /* The datagram buffer */
                sizeof(buf_packet),         /* The datagram lngth */
                0,                          /* Flags: no options */
                (struct sockaddr *)addr ,   /* addr */
                sizeof(struct sockaddr_in)  /* Server address length */
            );
            if(r < 0)
            {
                r = errno;
                logger_printf(1, "%s: sendto(%s) failed, r=%d", __FUNCTION__, instance->freed.targets[i], r);
            };
        };
    };

    close(freed_socket);

    return NULL;
};
