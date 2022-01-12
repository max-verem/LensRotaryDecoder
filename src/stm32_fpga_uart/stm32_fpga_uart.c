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

#include <hidapi.h>

#include "common/strl.h"

#include "instance.h"
#include "common/logger.h"

#include "stm32_fpga_uart.h"

#define MAX_STR 1024
#define MAX_BUF_READ 64
#define VID 0x05df
#define PID 0x16c0
#define PACKET_STH      0x5a
#define PACKET_LEN      11

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

const unsigned int
    vendor_id = VID,
    product_id = PID;

void* stm32_fpga_uart_proc(void* p)
{
    int res, packet_len = 0, i, r;
    char str[MAX_STR];
    wchar_t wstr[MAX_STR];
    unsigned char packet_buf[MAX_BUF_READ];
    instance_t* instance = (instance_t*)p;

    logger_printf(0, "%s: Entering....", __FUNCTION__);

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

            /* notify */
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
                logger_printf(1, "%s: usb device VID=0x%04X, PID=0x%04X, not found",
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

        for(res = 0; res >= 0 && !(*instance->p_exit);)
        {
            unsigned char recv[MAX_BUF_READ];
            res = hid_read(handle, recv, sizeof(recv));
            if (res == 0)
                usleep(1000);
            else for(i = 0; i < res; i++)
            {
                int32_t v[3];
                uint8_t cs, b = recv[i];

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
                if(packet_len != PACKET_LEN ||  PACKET_STH != packet_buf[0])
                    continue;

                /* calc checksum */
                for(cs = 0, r = 0; r < (PACKET_LEN - 1); r++)
                    cs += packet_buf[r];
                if(packet_buf[PACKET_LEN - 1] != cs)
                    continue;

                v[0] = 0;
                v[0] |= packet_buf[3]; v[0] <<= 8;
                v[0] |= packet_buf[2]; v[0] <<= 8;
                v[0] |= packet_buf[1];
                if(v[0] & 0x00800000) v[0] |= 0xFF000000;

                v[1] = 0;
                v[1] |= packet_buf[6]; v[1] <<= 8;
                v[1] |= packet_buf[5]; v[1] <<= 8;
                v[1] |= packet_buf[4];
                if(v[1] & 0x00800000) v[1] |= 0xFF000000;

                v[2] = 0;
                v[2] |= packet_buf[9]; v[2] <<= 8;
                v[2] |= packet_buf[8]; v[2] <<= 8;
                v[2] |= packet_buf[7];
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
        };

        // Close the device
        hid_close(handle);
    };

    logger_printf(0, "%s: Bye...", __FUNCTION__);

    return NULL;
};
