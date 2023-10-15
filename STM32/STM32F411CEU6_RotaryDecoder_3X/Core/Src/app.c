#include <stdint.h>

#include <stm32f4xx_hal.h>

#include "main.h"
#include "app.h"
#include "usb_device.h"
#include "usbd_customhid.h"

extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_up;

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;

#define ROT1_SHIFT 0	/* PA0, PA1 */
#define ROT2_SHIFT 2	/* PA2, PA3 */
#define ROT3_SHIFT 4	/* PA4, PA5 */

extern USBD_HandleTypeDef hUsbDeviceFS;

static struct
{
	uint8_t s1, s2;
	int32_t value0, value1, value2;
	uint16_t prev;
} instance;

static uint8_t buf[32];

static inline void instance_to_hid()
{
	uint32_t cs = 0;

	// STH
	buf[0] = 0xDE;
	buf[1] = 0xC0;

	cs += buf[4] = instance.s1;
	cs += buf[5] = instance.s2;

	cs += buf[ 6] = instance.value0 >> 16;
	cs += buf[ 7] = instance.value0 >>  8;
	cs += buf[ 8] = instance.value0 >>  0;

	cs += buf[ 9] = instance.value1 >> 16;
	cs += buf[10] = instance.value1 >>  8;
	cs += buf[11] = instance.value1 >>  0;

	cs += buf[12] = instance.value2 >> 16;
	cs += buf[13] = instance.value2 >>  8;
	cs += buf[14] = instance.value2 >>  0;

	// control sum
	buf[2] = (cs >> 8) ^ 0xFF;
	buf[3] = (cs >> 0) ^ 0xFF;

	/* send to USB */
	USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buf, 16);

	/* send to UART 16 */
	HAL_UART_Transmit_DMA(&huart1, buf, 16);
}

#ifdef ROTARY_TABLE
static int32_t rotary_table[1 << 12][3];

static inline void rotary_dec_table(uint8_t *buf, int len)
{
	int32_t prev = instance.prev;
    int32_t a = 0, b = 0, c = 0;

	for(int i = 0; i < len; i++)
	{
		int32_t curr = buf[i] & 0x3F;

		if(curr != prev)
		{
			prev <<= 6;
			prev |= curr;
			a += rotary_table[prev][0];
			b += rotary_table[prev][1];
			c += rotary_table[prev][2];
			prev &= 0x3F;
		}
	};

	instance.value0 += a;
	instance.value1 += b;
	instance.value2 += c;

	instance.prev = prev;
}
#else
static inline void rotary_dec_buf(uint8_t *buf, int len)
{
	int i;
    int32_t a = 0, b = 0, c = 0;

	for(i = 0; i < len; i++)
	{
	    instance.prev <<= 8;
	    instance.prev |= buf[i];

		// counter 1
		switch(instance.prev & (0x0303 << ROT1_SHIFT))
		{
/*
* CW
*
* 00	10 | ______00	______10
* 01	00 | ______01	______00
* 10	11 | ______10	______11
* 11	01 | ______11	______01
*
*/
		    case 0x0002 << ROT1_SHIFT:
		  	case 0x0100 << ROT1_SHIFT:
		  	case 0x0203 << ROT1_SHIFT:
		  	case 0x0301 << ROT1_SHIFT:
		  		a++;
		  	    break;
/*
* CCW
*
* 00	01 | ______00	______01
* 01	11 | ______01	______11
* 10	00 | ______10	______00
* 11	10 | ______11	______10
*/
		  	case 0x0001 << ROT1_SHIFT:
		  	case 0x0103 << ROT1_SHIFT:
		  	case 0x0200 << ROT1_SHIFT:
		  	case 0x0302 << ROT1_SHIFT:
		  	    a--;
		  		break;
		  }

		// counter 2
		switch(instance.prev & (0x0303 << ROT2_SHIFT))
		{
/*
* CW
*
* 00	10 | ______00	______10
* 01	00 | ______01	______00
* 10	11 | ______10	______11
* 11	01 | ______11	______01
*
*/
		    case 0x0002 << ROT2_SHIFT:
		  	case 0x0100 << ROT2_SHIFT:
		  	case 0x0203 << ROT2_SHIFT:
		  	case 0x0301 << ROT2_SHIFT:
		  		b++;
		  	    break;
/*
* CCW
*
* 00	01 | ______00	______01
* 01	11 | ______01	______11
* 10	00 | ______10	______00
* 11	10 | ______11	______10
*/
		  	case 0x0001 << ROT2_SHIFT:
		  	case 0x0103 << ROT2_SHIFT:
		  	case 0x0200 << ROT2_SHIFT:
		  	case 0x0302 << ROT2_SHIFT:
		  	    b--;
		  		break;
		}

		// counter 3
		switch(instance.prev & (0x0303 << ROT3_SHIFT))
		{
/*
* CW
*
* 00	10 | ______00	______10
* 01	00 | ______01	______00
* 10	11 | ______10	______11
* 11	01 | ______11	______01
*
*/
		    case 0x0002 << ROT3_SHIFT:
		  	case 0x0100 << ROT3_SHIFT:
		  	case 0x0203 << ROT3_SHIFT:
		  	case 0x0301 << ROT3_SHIFT:
		  		c++;
		  	    break;
/*
* CCW
*
* 00	01 | ______00	______01
* 01	11 | ______01	______11
* 10	00 | ______10	______00
* 11	10 | ______11	______10
*/
		  	case 0x0001 << ROT3_SHIFT:
		  	case 0x0103 << ROT3_SHIFT:
		  	case 0x0200 << ROT3_SHIFT:
		  	case 0x0302 << ROT3_SHIFT:
		  	    c--;
		  		break;
		  }
	  }

	  instance.value0 += a;
	  instance.value1 += b;
	  instance.value2 += c;
}
#endif

int8_t CUSTOM_HID_OutEvent_FS_main(uint8_t* buf)
{
	return (USBD_OK);
};

static uint8_t dma_buf_data[2 * DMA_BUF_SIZE];
volatile static uint32_t dma_half_cnt = 0, dma_full_cnt = 0;

static void dma_cb_half(DMA_HandleTypeDef *hdma)
{
	instance.s2++;
	dma_half_cnt++;
}

static void dma_cb_full(DMA_HandleTypeDef *hdma)
{
	instance.s2++;
	dma_full_cnt++;
}

static void dma_cb_abort(DMA_HandleTypeDef *hdma)
{
	instance.s1++;
}

static void dma_cb_error(DMA_HandleTypeDef *hdma)
{
	instance.s1++;
}

#ifdef ROTARY_TABLE
static void rotary_gen_table()
{
	int i;

	// generate tables
	for(i = 0; i < 1 << 12; i++)
	{
		rotary_table[i][0] =
		rotary_table[i][1] =
		rotary_table[i][2] = 0;
		/*
		 *      2     1     0     2     1     0
		 *   |=====|=====||=====|=====||=====|=====|
		 *   |11|10| 9| 8|| 7| 6| 5| 4|| 3| 2| 1| 0|
		 *
		 *   MASK
		 *   |           || 1| 1| X| X|| Z| Z| 1| 1|  0xC3
		 *
		 *                  8  4  2  1   8  4  2  1
 		 *   CW
		 *   |           || 0| 0| X| X|| Z| Z| 1| 0|  0x02
		 *   |           || 0| 1| X| X|| Z| Z| 0| 0|  0x40
		 *   |           || 1| 0| X| X|| Z| Z| 1| 1|  0x83
		 *   |           || 1| 1| X| X|| Z| Z| 0| 1|  0xC1
		 *
		 *   CCW
		 *   |           || 0| 0| X| X|| Z| Z| 0| 1|  0x01
		 *   |           || 0| 1| X| X|| Z| Z| 1| 1|  0x43
		 *   |           || 1| 0| X| X|| Z| Z| 0| 0|  0x80
		 *   |           || 1| 1| X| X|| Z| Z| 1| 0|  0xC2
		 *
		 */
		switch(i & (0x00C3 << ROT1_SHIFT))
		{
	    	case 0x0002 << ROT1_SHIFT:
	    	case 0x0040 << ROT1_SHIFT:
	    	case 0x0083 << ROT1_SHIFT:
	    	case 0x00C1 << ROT1_SHIFT:
				rotary_table[i][0] = 1;
	  	    	break;

	    	case 0x0001 << ROT1_SHIFT:
	    	case 0x0043 << ROT1_SHIFT:
	    	case 0x0080 << ROT1_SHIFT:
	    	case 0x00C2 << ROT1_SHIFT:
				rotary_table[i][0] = -1;
	  			break;
		}

		switch(i & (0x00C3 << ROT2_SHIFT))
		{
	    	case 0x0002 << ROT2_SHIFT:
	    	case 0x0040 << ROT2_SHIFT:
	    	case 0x0083 << ROT2_SHIFT:
	    	case 0x00C1 << ROT2_SHIFT:
				rotary_table[i][1] = 1;
	  	    	break;

	    	case 0x0001 << ROT2_SHIFT:
	    	case 0x0043 << ROT2_SHIFT:
	    	case 0x0080 << ROT2_SHIFT:
	    	case 0x00C2 << ROT2_SHIFT:
				rotary_table[i][1] = -1;
	  			break;
		}

		switch(i & (0x00C3 << ROT3_SHIFT))
		{
			case 0x0002 << ROT3_SHIFT:
			case 0x0040 << ROT3_SHIFT:
			case 0x0083 << ROT3_SHIFT:
			case 0x00C1 << ROT3_SHIFT:
				rotary_table[i][2] = 1;
	  	    	break;

			case 0x0001 << ROT3_SHIFT:
			case 0x0043 << ROT3_SHIFT:
			case 0x0080 << ROT3_SHIFT:
			case 0x00C2 << ROT3_SHIFT:
				rotary_table[i][2] = -1;
	  			break;
		}
	}
}
#endif

int app_init()
{
	return 0;
};

int app_begin()
{
#ifdef ROTARY_TABLE
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	rotary_gen_table();
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
#endif

	HAL_DMA_RegisterCallback(htim1.hdma[TIM_DMA_ID_UPDATE], HAL_DMA_XFER_CPLT_CB_ID, dma_cb_full);
	HAL_DMA_RegisterCallback(htim1.hdma[TIM_DMA_ID_UPDATE], HAL_DMA_XFER_HALFCPLT_CB_ID, dma_cb_half);
	HAL_DMA_RegisterCallback(htim1.hdma[TIM_DMA_ID_UPDATE], HAL_DMA_XFER_ERROR_CB_ID, dma_cb_error);
	HAL_DMA_RegisterCallback(htim1.hdma[TIM_DMA_ID_UPDATE], HAL_DMA_XFER_ABORT_CB_ID, dma_cb_abort);

	HAL_DMA_Start_IT(htim1.hdma[TIM_DMA_ID_UPDATE], (uint32_t)&(GPIOA->IDR), (uint32_t)dma_buf_data, 2 * DMA_BUF_SIZE);
	__HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_UPDATE ); //Enable the TIM Update DMA request
	__HAL_TIM_ENABLE(&htim1);                 //Enable the Peripheral

	return 0;
};

int app_loop()
{
	int led_cnt = LED_CNT, sec_cnt = SEQ_CNT;

	dma_half_cnt = 0;
	dma_full_cnt = 0;

	while(1)
	{
		/* wait for any buffer */
		while(!dma_half_cnt && !dma_full_cnt);

		if(dma_half_cnt > 1 || dma_full_cnt > 1)
			instance.s1++;

		/* first part ready */
		if(dma_half_cnt)
		{
			dma_half_cnt = 0;
#ifdef ROTARY_TABLE
			rotary_dec_table(dma_buf_data, DMA_BUF_SIZE);
#else
			rotary_dec_buf(dma_buf_data, DMA_BUF_SIZE);
#endif
		}
		/* second part ready */
		else if(dma_full_cnt)
		{
			dma_full_cnt = 0;
#ifdef ROTARY_TABLE
			rotary_dec_table(dma_buf_data + DMA_BUF_SIZE, DMA_BUF_SIZE);
#else
			rotary_dec_buf(dma_buf_data + DMA_BUF_SIZE, DMA_BUF_SIZE);
#endif
		}

		// toggle led
		led_cnt--;
		if(!led_cnt)
		{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			led_cnt = LED_CNT;
		};

		/* report send */
		sec_cnt--;
		if(!sec_cnt)
		{
			instance_to_hid();
			sec_cnt = SEQ_CNT;
		};
	};

	return 0;
};

