#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

#include <libusb-1.0/libusb.h>

#define TRANSFER_SIZE   512
#define TRANSFER_CNT    512
#define TRANSFER_TMO    5000

#define VID 0x1d50
#define PID 0x608c

/* Protocol commands */
#define CMD_GET_FW_VERSION              0xb0
#define CMD_START                       0xb1
#define CMD_GET_REVID_VERSION           0xb2
#define CMD_START_FLAGS_WIDE_POS        5
#define CMD_START_FLAGS_CLK_SRC_POS     6
#define CMD_START_FLAGS_SAMPLE_8BIT     (0 << CMD_START_FLAGS_WIDE_POS)
#define CMD_START_FLAGS_SAMPLE_16BIT    (1 << CMD_START_FLAGS_WIDE_POS)
#define CMD_START_FLAGS_CLK_30MHZ       (0 << CMD_START_FLAGS_CLK_SRC_POS)
#define CMD_START_FLAGS_CLK_48MHZ       (1 << CMD_START_FLAGS_CLK_SRC_POS)

#define SR_MHZ(n)   ((n) * (uint64_t)(1000000ULL))
#define MAX_SAMPLE_DELAY   (6 * 256)

#pragma pack(push, 1)
struct version_info {
    uint8_t major;
    uint8_t minor;
};
struct cmd_start_acquisition {
    uint8_t flags;
    uint8_t sample_delay_h;
    uint8_t sample_delay_l;
};
#pragma pack(pop)

#include "instance.h"
#include "common/logger.h"

static int command_get_fw_version(libusb_device_handle *devhdl, struct version_info *vi)
{
    int ret;

    ret = libusb_control_transfer(devhdl, LIBUSB_REQUEST_TYPE_VENDOR |
        LIBUSB_ENDPOINT_IN, CMD_GET_FW_VERSION, 0x0000, 0x0000,
        (unsigned char *)vi, sizeof(struct version_info), 100);

    if (ret < 0)
    {
        logger_printf(1, "%s: Unable to get version info: %s.", __FUNCTION__, libusb_error_name(ret));
        return ret;
    }

    return 0;
}

static int command_get_revid_version(libusb_device_handle *devhdl, uint8_t *revid)
{
    int ret;

    ret = libusb_control_transfer(devhdl, LIBUSB_REQUEST_TYPE_VENDOR |
        LIBUSB_ENDPOINT_IN, CMD_GET_REVID_VERSION, 0x0000, 0x0000,
        revid, 1, 100);

    if (ret < 0)
    {
        logger_printf(1, "%s: Unable to get REVID: %s.", __FUNCTION__, libusb_error_name(ret));
        return ret;
    }

    return 0;
}

static int command_start_acquisition(libusb_device_handle *devhdl,
                      uint64_t samplerate, int sample_wide)
{
    int delay = 0, ret;
    struct cmd_start_acquisition cmd = {0};

    /* Compute the sample rate. */
    if ((SR_MHZ(48) % samplerate) == 0)
    {
        cmd.flags = CMD_START_FLAGS_CLK_48MHZ;
        delay = SR_MHZ(48) / samplerate - 1;
        if (delay > MAX_SAMPLE_DELAY)
            delay = 0;
    }
    if (delay == 0 && (SR_MHZ(30) % samplerate) == 0)
    {
        cmd.flags = CMD_START_FLAGS_CLK_30MHZ;
        delay = SR_MHZ(30) / samplerate - 1;
    }
    logger_printf(1, "%s: GPIF delay = %d, clocksource = %sMHz.",
        __FUNCTION__, delay, (cmd.flags & CMD_START_FLAGS_CLK_48MHZ) ? "48" : "30");
    if (delay <= 0 || delay > MAX_SAMPLE_DELAY)
    {
        logger_printf(1, "%s: Unable to sample at %" PRIu64 "Hz.", __FUNCTION__, samplerate);
        return -EINVAL;
    }
    cmd.sample_delay_h = (delay >> 8) & 0xff;
    cmd.sample_delay_l = delay & 0xff;

    /* Select the sampling width. */
    cmd.flags |= sample_wide ? CMD_START_FLAGS_SAMPLE_16BIT : CMD_START_FLAGS_SAMPLE_8BIT;

    /* Send the control message. */
    ret = libusb_control_transfer(devhdl, LIBUSB_REQUEST_TYPE_VENDOR |
            LIBUSB_ENDPOINT_OUT, CMD_START, 0x0000, 0x0000,
            (unsigned char *)&cmd, sizeof(cmd), 100);
    if (ret < 0)
    {
        logger_printf(1, "%s: Unable to send start command: %s.", __FUNCTION__, libusb_error_name(ret));
        return ret;
    }

    return 0;
}

static void callbackUSBTransferComplete(struct libusb_transfer *xfr)
{
    int i, ret;
    instance_t* instance = (instance_t*)xfr->user_data;

    if(xfr->status == LIBUSB_TRANSFER_COMPLETED)
    {
        /* continue ? */
        if(!(*instance->p_exit))
        {
            pthread_mutex_lock(&instance->lock);

            /* enqueu transfer for processing */
            instance->xfr.queue1_data[instance->xfr.queue1_count++] = xfr;

            /* enqueu buffers for transfers */
            for(i = 0; i < instance->xfr.queue2_count; i++)
            {
                /* enqueue transfer request */
                ret = libusb_submit_transfer(instance->xfr.queue2_data[i]);
                if(ret < 0)
                {
                    // Error
                    logger_printf(1, "%s: USB resubmit transfer error: %s",
                        __FUNCTION__, libusb_error_name(ret));
                    instance->xfr.errors++;
                }
            };

            instance->xfr.queue2_count = 0;

            pthread_cond_broadcast(&instance->cond);

            pthread_mutex_unlock(&instance->lock);
        };
    }
    else
    {
        const char* msg = "undefined";

        instance->xfr.errors++;

        switch(xfr->status)
        {
            case LIBUSB_TRANSFER_CANCELLED: msg = "CANCELLED"; break;
            case LIBUSB_TRANSFER_NO_DEVICE: msg = "NO_DEVICE"; break;
            case LIBUSB_TRANSFER_TIMED_OUT: msg = "TIMED_OUT"; break;
            case LIBUSB_TRANSFER_ERROR:     msg = "ERROR";     break;
            case LIBUSB_TRANSFER_STALL:     msg = "STALL";     break;
            case LIBUSB_TRANSFER_OVERFLOW:  msg = "OVERFLOW";  break;
            case LIBUSB_TRANSFER_COMPLETED: msg = "COMPLETED"; break;
        };

        logger_printf(1, "%s: USB transfer error: %s", __FUNCTION__, msg);
    };
}

static void* counter_proc(void* p)
{
    int i;
    instance_t* instance = (instance_t*)p;

    logger_printf(0, "%s: Entering...", __FUNCTION__);

    while(!instance->xfr.errors)
    {
        int addon[3];
        struct timespec to;
        uint8_t prev[3];
        struct libusb_transfer *xfr = NULL;

        /* wait for buffer */
        pthread_mutex_lock(&instance->lock);
        clock_gettime(CLOCK_REALTIME, &to);
        to.tv_sec += 1;
        while(!instance->xfr.queue1_count && !instance->xfr.errors && !(*instance->p_exit))
            pthread_cond_timedwait(&instance->cond, &instance->lock, &to);
        if(!instance->xfr.errors)
        {
            xfr = instance->xfr.queue1_data[0];
            instance->xfr.queue1_count--;
            memmove(&instance->xfr.queue1_data[0], &instance->xfr.queue1_data[1], sizeof(void*) * instance->xfr.queue1_count);
        };
        pthread_mutex_unlock(&instance->lock);

        /* process here */
        if(!xfr)
            continue;

        prev[0] = instance->decoder.prev[0];
        prev[1] = instance->decoder.prev[1];
        prev[2] = instance->decoder.prev[2];
        addon[0] = addon[1] = addon[2] = 0;

        for(i = 0; i < xfr->actual_length; i++)
        {
            unsigned char v = xfr->buffer[i];
            static const int8_t r[16] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };

            prev[0] <<= 2; prev[0] |= v & 0x03; v >>= 2;
            addon[0] += r[prev[0] & 0x0f];

            prev[1] <<= 2; prev[1] |= v & 0x03; v >>= 2;
            addon[1] += r[prev[1] & 0x0f];

            prev[2] <<= 2; prev[2] |= v & 0x03; v >>= 2;
            addon[2] += r[prev[2] & 0x0f];
        };

        instance->decoder.counters[0] += addon[0];
        instance->decoder.counters[1] += addon[1];
        instance->decoder.counters[2] += addon[2];

        instance->decoder.prev[0] = prev[0];
        instance->decoder.prev[1] = prev[1];
        instance->decoder.prev[2] = prev[2];

        instance->decoder.samples += xfr->actual_length;

        /* enqueue processed buffers */
        pthread_mutex_lock(&instance->lock);
        instance->xfr.queue2_data[instance->xfr.queue2_count++] = xfr;
        pthread_mutex_unlock(&instance->lock);
    };

    logger_printf(0, "%s: Exiting...", __FUNCTION__);

    return NULL;
};

void* ez_usb_reader_proc(void* p)
{
    int ret;
    instance_t* instance = (instance_t*)p;
    struct libusb_device_descriptor desc;
    libusb_device_handle *dev_handle = NULL;
    libusb_context *context = NULL;
    libusb_device **list;
    size_t count, i;

retr:

    logger_printf(0, "%s: Searching for %.4X:%.4X", __FUNCTION__, VID, PID);

    // Initialize library
    libusb_init(&context);

    // Get list of USB devices currently connected
    count = libusb_get_device_list(context, &list);

    for(i = 0; i < count; i++)
    {
        libusb_get_device_descriptor(list[i], &desc);

        // Is our device?
        if(desc.idVendor == VID && desc.idProduct == PID)
        {
            // Open USB device and get handle
            if(libusb_open(list[i], &dev_handle))
                logger_printf(1, "%s: Failed to open device %.4X:%.4X", __FUNCTION__, VID, PID);
            break;
        }
    }

    libusb_free_device_list(list, count);

    if(dev_handle)
    {
        libusb_device *dev_instance;
        struct libusb_config_descriptor *dev_config = NULL;

        dev_instance = libusb_get_device(dev_handle);

        if(!dev_instance)
            logger_printf(1, "Failed to libusb_get_device");
        else
        {
            if(libusb_get_active_config_descriptor(dev_instance, &dev_config))
                logger_printf(1, "Failed to libusb_get_active_config_descriptor");
            else
            {
                uint8_t revid;
                struct version_info vi;
                int endpoint = dev_config->interface[0].altsetting[0].endpoint[0].bEndpointAddress;

                logger_printf(1, "%s: endpoint=%d (0x%.2X)", __FUNCTION__, endpoint, endpoint);

                if(!command_get_fw_version(dev_handle, &vi) && !command_get_revid_version(dev_handle, &revid))
                {
                    struct libusb_transfer *xfrs[MAX_TRANSFER_CNT];
                    unsigned char *xfrs_buffers[MAX_TRANSFER_CNT];

                    logger_printf(1, "%s: fw_version Major=%.2X, Minor=%.2X, revision %.2X", __FUNCTION__, vi.major, vi.minor, revid);

                    /* enqueue transfers */
                    for(ret = 0, i = 0; i < TRANSFER_CNT; i++)
                    {
                        struct libusb_transfer *xfr;
                        unsigned char *data;

                        data = malloc(TRANSFER_SIZE);
                        xfr = libusb_alloc_transfer(0);

                        libusb_fill_bulk_transfer(xfr, dev_handle, endpoint, data, TRANSFER_SIZE, callbackUSBTransferComplete, instance, TRANSFER_TMO);

                        ret = libusb_submit_transfer(xfr);
                        if(ret < 0)
                        {
                            logger_printf(1, "%s: libusb_submit_transfer failed: %s", __FUNCTION__, libusb_error_name(ret));
                            break;
                        };
                        xfrs[i] = xfr;
                        xfrs_buffers[i] = data;
                    };

                    if(!ret)
                    {
                        pthread_t th;

                        instance->xfr.errors =
                        instance->xfr.queue1_count =
                        instance->xfr.queue2_count = 0;

                        pthread_create(&th, 0, counter_proc, instance);

                        if(!command_start_acquisition(dev_handle, SR_MHZ(4), 0))
                        {
                            /* poll ? */
                            while(!(*instance->p_exit) && !instance->xfr.errors)
                            {
                                struct timeval tv;

                                tv.tv_sec = 1;
                                tv.tv_usec = 0;

                                ret = libusb_handle_events_timeout(context, &tv);
                                if(ret < 0)
                                {
                                    logger_printf(1, "%s: libusb_handle_events_timeout failed: %s", __FUNCTION__, libusb_error_name(ret));
                                    break;
                                };
                            };
                        };

                        logger_printf(0, "%s: waiting for counter_proc...", __FUNCTION__);
                        instance->xfr.errors++;
                        pthread_join(th, NULL);
                    };

                    logger_printf(0, "%s: canceling bulk transfers...", __FUNCTION__);

                    for(i = 0; i < TRANSFER_CNT; i++)
                        libusb_cancel_transfer(xfrs[i]);

                    logger_printf(0, "%s: awaiting bulk transfers...", __FUNCTION__);

                    while(1)
                    {
                        struct timeval tv;
                        int completed = 0;

                        tv.tv_sec = 1;
                        tv.tv_usec = 0;

                        ret = libusb_handle_events_timeout_completed(context, &tv, &completed);

                        logger_printf(0, "%s: libusb_handle_events_timeout_completed: ret=%d, completed=%d",
                            __FUNCTION__, ret, completed);

                        if(ret < 0)
                            break;

                        if(!completed)
                            break;
                    };

                    logger_printf(0, "%s: freeing bulk transfers...", __FUNCTION__);

                    for(i = 0; i < TRANSFER_CNT; i++)
                    {
                        libusb_free_transfer(xfrs[i]);
                        free(xfrs_buffers[i]);
                    };
                };

                libusb_free_config_descriptor(dev_config);
            };
        };
    }
    else
        logger_printf(1, "%s: Device %.4X:%.4X not found", __FUNCTION__, VID, PID);

    libusb_exit(context);

    if(!(*instance->p_exit))
    {
        sleep(1);
        goto retr;
    };

    logger_printf(1, "%s: bye", __FUNCTION__);

    return 0;
}
