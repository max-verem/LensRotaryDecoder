#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iconv.h>

#include <hidapi.h>

#include "common/logger.h"
#include "common/strl.h"
#include "instance.h"
#include "pipeline.h"

static void* indiemark_param_reader_proc(instance_t* instance, int lp);
static void* indiemark_zoom_reader_proc(void* p){ return indiemark_param_reader_proc(p, LENS_PARAM_ZOOM); };
static void* indiemark_focus_reader_proc(void* p){ return indiemark_param_reader_proc(p, LENS_PARAM_FOCUS); };

void* indiemark_reader_proc(void* p)
{
    pthread_t th_zoom, th_focus;

    // Initialize the hidapi library
    hid_init();

    /* run zoom reader */
    pthread_create(&th_zoom, 0, indiemark_zoom_reader_proc, p);

    /* run focus reader */
    pthread_create(&th_focus, 0, indiemark_focus_reader_proc, p);

    /* wait for threads */
    pthread_join(th_zoom, NULL);
    pthread_join(th_focus, NULL);

    // Finalize the hidapi library
    hid_exit();

    return NULL;
}

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
#define MAX_BUF_WRITE 64

const unsigned int
    vendor_id = 0x04B4,
    product_id = 0x8051;

static void* indiemark_param_reader_proc(instance_t* instance, int lp)
{
    int res;
    char str[MAX_STR], serial[PATH_MAX];
    wchar_t wstr[MAX_STR];
    unsigned char buf[MAX_BUF_WRITE];

    logger_printf(0, "%s[%d]: Entering....", __FUNCTION__, lp);


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

            /* save current serial */
            strlcpy(serial, instance->state.lens[lp].serial, PATH_MAX);

            /* check if serial number is ours */
            if(strcmp(str, instance->state.lens[lp].serial))
            {
                logger_printf(0, "%s[%d]: found device [%s] with serial [%s] is not ours [%s]",
                    __FUNCTION__, lp, cur_dev->path, str, serial);
                cur_dev = cur_dev->next;
                continue;
            };

            logger_printf(0, "%s[%d]: found device [%s] with serial [%s]", __FUNCTION__, lp, cur_dev->path, str);

            /* try to open */
            handle = hid_open_path(cur_dev->path);
            if(!handle)
            {
                logger_printf(1, "%s[%d]: hid_open_path(%s) failed",
                    __FUNCTION__, lp, cur_dev->path);
            };

            break;
        };
        hid_free_enumeration(devs);

        /* check if device found */
        if(!handle)
        {
            logger_printf(1, "%s[%d]: usb device VID=0x%04X, PID=0x%04X, serial=[%s] not found",
                __FUNCTION__, lp, vendor_id, product_id, serial);
            for(res = 0; res < 5000 && !(*instance->p_exit); res++)
                usleep(1000);
            continue;
        };

        // Read the Manufacturer String
        res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
        logger_printf(0, "%s[%d]: Manufacturer String: %s", __FUNCTION__, lp, wchar_to_char(wstr, str, sizeof(str)));

        // Read the Product String
        res = hid_get_product_string(handle, wstr, MAX_STR);
        logger_printf(0, "%s[%d]: Product String: %s", __FUNCTION__, lp, wchar_to_char(wstr, str, sizeof(str)));

        // Read the Serial Number String
        res = hid_get_serial_number_string(handle, wstr, MAX_STR);
        logger_printf(0, "%s[%d]: Serial Number String: %s", __FUNCTION__, lp, wchar_to_char(wstr, str, sizeof(str)));

        // Read Indexed String 1
        res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
        logger_printf(0, "%s[%d]: Indexed String 1: %s", __FUNCTION__, lp, wchar_to_char(wstr, str, sizeof(str)));

        // set params
        buf[0] = 0x41;
        buf[1] = 0x00
            | 0x80 // flashLED
//        | 0x40 // reset counter
//        | 0x10 // Blackout
        ;
        buf[2] = 0x40; //0x12;
        buf[3] = 0x01; //0x34;
        hid_write(handle, buf, 4);

        // Set the hid_read() function to be non-blocking.
        hid_set_nonblocking(handle, 1);

        memset(buf, 0, sizeof(buf));
        for(res = 0; res >= 0 && !(*instance->p_exit);)
        {
            unsigned char recv[MAX_BUF_WRITE];
            res = hid_read(handle, recv, sizeof(recv));
            if (res == 0)
                usleep(1000);
            else if(res > 0 && (recv[0] != buf[0] || recv[1] != buf[1] || recv[2] != buf[2] || recv[3] != buf[3]))
            {
                double ds, dd;
                unsigned int v;

                v = recv[2];
                v <<= 8;
                v |= recv[3];

                pthread_mutex_lock(&instance->lock);
                instance->lens[lp].curr[LENS_PARAM_RAW] = v;
                ds = instance->lens[lp].src_range[1] - instance->lens[lp].src_range[0];
                dd = instance->state.lens[lp].dst_range[1] - instance->state.lens[lp].dst_range[0];
                if(ds == 0.0)
                    instance->lens[lp].curr[LENS_PARAM_SCALED] = 0.0;
                else
                    instance->lens[lp].curr[LENS_PARAM_SCALED] = instance->state.lens[lp].dst_range[0]
                        + (instance->lens[lp].curr[LENS_PARAM_RAW] - instance->lens[lp].src_range[0]) * dd / ds;
                pthread_mutex_unlock(&instance->lock);

                memcpy(buf, recv, MAX_BUF_WRITE);
            };


            /* check if serial number changed */
            if(strcmp(serial, instance->state.lens[lp].serial))
            {
                instance->lens[lp].curr[LENS_PARAM_RAW] =
                instance->lens[lp].curr[LENS_PARAM_SCALED] = 0.0;
                break;
            };
        };

        // Close the device
        hid_close(handle);
    };


    logger_printf(0, "%s[%d]: Bye...", __FUNCTION__, lp);

    return NULL;
};
