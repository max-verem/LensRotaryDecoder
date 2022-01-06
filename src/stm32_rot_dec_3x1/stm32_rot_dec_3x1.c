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

#include "stm32_rot_dec.h"

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
    int res;
    instance_t* instance = (instance_t*)p;
    char str[MAX_STR];
    wchar_t wstr[MAX_STR];
    unsigned char buf[MAX_BUF_READ];

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
            wchar_to_char(cur_dev->serial_number, str, sizeof(str));

            logger_printf(0, "%s: found device [%s] with serial [%s] {%d}", __FUNCTION__, cur_dev->path, str, idx);

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
                logger_printf(1, "%s: usb device VID=0x%04X, PID=0x%04X, serial=[%s] not found",
                    __FUNCTION__, vendor_id, product_id, serial);
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
            else if(res > 0 && (recv[0] != buf[0] || recv[1] != buf[1] || recv[2] != buf[2] || recv[3] != buf[3] || recv[4] != buf[4] || recv[5] != buf[5]))
            {
                int v = 0;

                v |= recv[2];
                v <<= 8;
                v |= recv[3];
                v <<= 8;
                v |= recv[4];
                v <<= 8;
                v |= recv[5];

                pthread_mutex_lock(&instance->lock);
                instance->values[lp] = v;
                pthread_mutex_unlock(&instance->lock);

                memcpy(buf, recv, MAX_BUF_READ);
            };
        };

        // Close the device
        hid_close(handle);
    };


    logger_printf(0, "%s: Bye...", __FUNCTION__);

    return NULL;
};
