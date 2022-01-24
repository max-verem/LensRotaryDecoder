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

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>

#include <hidapi.h>

#include "common/strl.h"

#include "instance.h"
#include "common/logger.h"

#include "stm32_rot_dec_3x1.h"

#include "common/FreeD.h"

#define VID 0x05df
#define PID 0x16c0

static char* wchar_to_char
(
    const wchar_t* src,
    char *dst, int dst_lim
)
{
    iconv_t i;
    size_t r, l = wcslen(src);

    char *buf, *inbuf, *outbuf;
    size_t inbytesleft, outbytesleft;

    // const char *tocode, const char *fromcode
    i = iconv_open("UTF8", "WCHAR_T");
    if(i == (iconv_t) -1)
        return NULL;

    // prep
    inbytesleft = sizeof(wchar_t) * (l + 1);
    inbuf = (char*)src;

    outbytesleft = dst_lim;
    outbuf = buf = dst;
    memset(buf, 0, outbytesleft);

    // conv
    r = iconv(i, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    if(r == (size_t) -1)
        fprintf(stderr, "iconv failed, inbytesleft=%d, outbytesleft=%d\n", (int)inbytesleft, (int)outbytesleft);

    iconv_close(i);

    return buf;
};

#define MAX_STR 1024
#define MAX_BUF_READ 64

const unsigned int
    vendor_id = VID,
    product_id = PID;

void* stm32_rot_dec_proc(void* p)
{
    int res, i, freed_socket, freed_socket_broadcast = 1;
    instance_t* instance = (instance_t*)p;
    char str[MAX_STR];
    wchar_t wstr[MAX_STR];
    unsigned char buf[MAX_BUF_READ];
    FreeD_D1_t freed;
    unsigned char buf_packet[FREE_D_D1_PACKET_SIZE];
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
    for(i = 0; i < instance->freed.cnt; i++)
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

    while(!(*instance->p_exit))
    {
        hid_device *handle = NULL;
        struct hid_device_info *devs, *cur_dev;

        /* try to find a device we need */
        cur_dev = devs = hid_enumerate(vendor_id, product_id);
        while(cur_dev)
        {
            /* convert serial */
            if(!cur_dev->serial_number)
                break;
            wchar_to_char(cur_dev->serial_number, str, sizeof(str));

            logger_printf(0, "%s: found device [%s] with serial [%s]", __FUNCTION__, cur_dev->path, str);

            /* try to open */
            handle = hid_open_path(cur_dev->path);
            if(!handle)
            {
                logger_printf(1, "%s: hid_open_path(%s) failed",
                    __FUNCTION__, cur_dev->path);
            };

            break;
        };
        hid_free_enumeration(devs);

        /* check if device found */
        if(!handle)
        {
            if(instance->f_debug)
                logger_printf(1, "%s: usb device VID=0x%04X, PID=0x%04X",
                    __FUNCTION__, vendor_id, product_id);
            for(res = 0; res < 5000 && !(*instance->p_exit); res++)
                usleep(1000);
            continue;
        };

        // Read the Manufacturer String
        res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
        logger_printf(0, "%s: Manufacturer String: %s", __FUNCTION__, wchar_to_char(wstr, str, sizeof(str)));

        // Read the Product String
        res = hid_get_product_string(handle, wstr, MAX_STR);
        logger_printf(0, "%s: Product String: %s", __FUNCTION__, wchar_to_char(wstr, str, sizeof(str)));

        // Read the Serial Number String
        res = hid_get_serial_number_string(handle, wstr, MAX_STR);
        logger_printf(0, "%s: Serial Number String: %s", __FUNCTION__, wchar_to_char(wstr, str, sizeof(str)));

        // Read Indexed String 1
        res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
        logger_printf(0, "%s: Indexed String 1: %s", __FUNCTION__, wchar_to_char(wstr, str, sizeof(str)));

        // Set the hid_read() function to be non-blocking.
        hid_set_nonblocking(handle, 1);

        memset(buf, 0, sizeof(buf));
        for(res = 0; res >= 0 && !(*instance->p_exit);)
        {
            unsigned char recv[MAX_BUF_READ];
            res = hid_read(handle, recv, sizeof(recv));
            if (res == 0)
                usleep(1000);
            else if(res > 0)
            {
                int32_t v0 = 0, v1 = 0, v2 = 0;

                if(instance->f_debug)
                    logger_printf
                    (
                        1, "%s: [%4d] | %02x %02x | %02x %02x | %02x %02x | %02x %02x %02x | %02x %02x %02x | %02x %02x %02x |",
                        __FUNCTION__, res,
                        recv[ 0], recv[ 1],             // STH
                        recv[ 2], recv[ 3],             // CS
                        recv[ 4], recv[ 5],             // s1, s2
                        recv[ 6], recv[ 7], recv[ 8],   // ZOOM
                        recv[ 9], recv[10], recv[11],   // FOCUS
                        recv[12], recv[13], recv[14]    // IRIS
                    );

                v0 |= recv[6];
                v0 <<= 8;
                v0 |= recv[7];
                v0 <<= 8;
                v0 |= recv[8];
                v0 <<= 8;
                v0 >>= 8;

                v1 |= recv[9];
                v1 <<= 8;
                v1 |= recv[10];
                v1 <<= 8;
                v1 |= recv[11];
                v1 <<= 8;
                v1 >>= 8;

                v2 |= recv[12];
                v2 <<= 8;
                v2 |= recv[13];
                v2 <<= 8;
                v2 |= recv[14];
                v2 <<= 8;
                v2 >>= 8;

                pthread_mutex_lock(&instance->lock);
                instance->values[LENS_PARAM_ZOOM] = v0;
                instance->values[LENS_PARAM_FOCUS] = v1;
                instance->values[LENS_PARAM_IRIS] = v2;
                instance->stat[0] = recv[4];
                instance->stat[1] = recv[5];
                pthread_mutex_unlock(&instance->lock);

                memcpy(buf, recv, MAX_BUF_READ);

                /* inc freed divider */
                instance->freed.cnt++;

                /* check if packet should be sent */
                if(!(instance->freed.cnt % instance->freed.div))
                {
                    int Iris;

                    /* prepare struct */
                    Iris = instance->values[LENS_PARAM_IRIS];
                    freed.Zoom = instance->values[LENS_PARAM_ZOOM];
                    freed.Focus = instance->values[LENS_PARAM_FOCUS];
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

                        sendto
                        (
                            freed_socket,               /* Socket to send result */
                            (char*)buf_packet,          /* The datagram buffer */
                            sizeof(buf_packet),         /* The datagram lngth */
                            0,                          /* Flags: no options */
                            (struct sockaddr *)addr ,   /* addr */
                            sizeof(struct sockaddr_in)  /* Server address length */
                        );
                    };
                };
            };
        };

        // Close the device
        hid_close(handle);
    };


    logger_printf(0, "%s: Bye...", __FUNCTION__);

    return NULL;
};
