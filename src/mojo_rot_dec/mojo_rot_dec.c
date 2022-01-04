#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <iconv.h>

#include "common/strl.h"

#include "instance.h"
#include "common/logger.h"

#include "mojo_rot_dec.h"

#define PACKET_STH      0x5a
#define PACKET_LEN      10

void* mojo_rot_dec_proc(void* p)
{
    int fd, r, packet_len = 0;
    instance_t* instance = (instance_t*)
    uint8_t packet_buf[PACKET_LEN];

    /* serial port & buffer operation */
    fd_set fds;
    struct timeval tv;
    struct termios newtio;

    /* open serial port */
open_dev:
    logger_printf(0, "%s: opening [%s]", __FUNCTION__, instance->mojo);
    fd = open(ctx->instance->serial[ctx->idx], O_RDWR | O_NOCTTY );

    /* check if port opened */
    if(fd < 0)
    {
        r = -errno;
        logger_printf(0, "%s: failed to open [%s], r=%d",
            __FUNCTION__, instance->mojo, r);

        if(*instance->p_exit)
            return NULL;

        logger_printf(0, "%s: will retry in 3 seconds", __FUNCTION__);

        sleep(2);

        goto open_dev;
    };

    /* configure serial device */
    memset(&newtio, 0, sizeof(struct termios));     /* clear struct for new port settings */
    newtio.c_iflag = IGNPAR;                        /* ignore bytes with parity errors */
    newtio.c_oflag = 0;                             /* Raw output */
    newtio.c_lflag = 0;                             /* enable canonical input */
    newtio.c_cc[VTIME] = 1;                         /* inter-character timer unused */
    newtio.c_cc[VMIN] = 1;                          /* blocking read until 5 chars received */
    newtio.c_cflag =
        B38400 |
        PARENB |
        PARODD |
        CS8 |
        CREAD |
        CLOCAL;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);

    /* discard all data in buffers */
    tcflush(fd, TCIOFLUSH);

    while(!(*ctx->instance->p_exit))
    {
        /* prepare to check if data present in buffer */
        FD_ZERO (&fds);
        FD_SET (fd, &fds);
        tv.tv_sec = 0; /* Timeout 'sec'. */
        tv.tv_usec = 100 * 1000; /* Timeout 'usec'. */

        /* check */
        r = select(fd + 1, &fds, NULL, NULL, &tv);
        if(r > 0)
        {
            uint8_t b;

            /* read data */
            r = read(fd, &b, 1);
            if (r > 0)
            {
                int32_t v[3];
                uint8_t cs;

                /* store byte */
                if(packet_len < PACKET_LEN)
                {
                    packet_buf[packet_len] = b;
                    packet_len++;
                }
                else
                {
                    memmove(packet_buf, packet_buf + 1, PACKET_LEN - 1);
                    packet_buf[PACKET_LEN - 1] = b;
                };

                /* increment byte counter */
                instance->recv[0]++;

                /* check packet size and STH */
                for(cs = 0, r = 0; r < (PACKET_LEN - 1); r++)
                    cs += packet_buf[r];
                if(packet_buf[PACKET_LEN - 1] != cs)
                    continue;

                v[0] = 0;
                v[0] |= packet_buf[1]; v[0] <<= 8;
                v[0] |= packet_buf[2]; v[0] <<= 8;
                v[0] |= packet_buf[3];
                if(v[0] & 0x00800000) v[0] |= 0xFF000000;

                v[1] = 0;
                v[1] |= packet_buf[4]; v[1] <<= 8;
                v[1] |= packet_buf[5]; v[1] <<= 8;
                v[1] |= packet_buf[6];
                if(v[1] & 0x00800000) v[1] |= 0xFF000000;

                v[2] = 0;
                v[2] |= packet_buf[7]; v[2] <<= 8;
                v[2] |= packet_buf[8]; v[2] <<= 8;
                v[2] |= packet_buf[9];
                if(v[2] & 0x00800000) v[2] |= 0xFF000000;


                pthread_mutex_lock(&instance->lock);
                instance->values[0] = v[0];
                instance->values[1] = v[1];
                instance->values[2] = v[2];
                pthread_mutex_unlock(&instance->lock);

                /* increment packet counter */
                instance->recv[1]++;

                /* reset packet len */
                packet_len = 0;
            };
        }
        else
        {
            /* check if error in select */
            if(r < 0)
            {
                r = -errno;
                logger_printf(1, "%s: select failed, errno=%d", __FUNCTION__, r);
                break;
            };
        };

    };

    close(fd);

    if(!(*instance->p_exit))
        goto open_dev;

    logger_printf(0, "%s: exiting", __FUNCTION__);

    return NULL;
};
