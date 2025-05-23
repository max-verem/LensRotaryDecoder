#include <stdint.h>
#include <stdio.h>

#include "main.h"
#include "tim.h"
#include "usart.h"
#include "dma.h"

#include "app.h"

#define ROT1_SHIFT 0	/* PA0, PA1 */
#define ROT2_SHIFT 2	/* PA2, PA3 */
#define ROT3_SHIFT 4	/* PA4, PA5 */

static struct
{
	uint8_t s1, s2;
	int32_t value0, value1, value2;
	uint16_t prev;
} instance;

static inline void instance_to_hid()
{
	int values[3] = {instance.value0, instance.value1, instance.value2};
#ifdef SERIAL_OUTPUT_TEXT
	int r;
	static char text_report[128];

	r = snprintf(text_report, sizeof(text_report), "%8d |%8d |%8d |\r\n",
		values[0], values[1], values[2]);

	// HAL_UART_Transmit_DMA(&huart6, text_report, r);
	r = HAL_UART_Transmit_DMA(&huart1, (uint8_t*)text_report, r);
#else
	static uint8_t buf[32];
	uint32_t cs = 0;

	// STH
	buf[0] = 0xDE;
	buf[1] = 0xC0;

	cs += buf[4] = instance.s1;
	cs += buf[5] = instance.s2;

	cs += buf[ 6] = values[0] >> 16;
	cs += buf[ 7] = values[0] >>  8;
	cs += buf[ 8] = values[0] >>  0;

	cs += buf[ 9] = values[1] >> 16;
	cs += buf[10] = values[1] >>  8;
	cs += buf[11] = values[1] >>  0;

	cs += buf[12] = values[2] >> 16;
	cs += buf[13] = values[2] >>  8;
	cs += buf[14] = values[2] >>  0;

	// control sum
	buf[2] = (cs >> 8) ^ 0xFF;
	buf[3] = (cs >> 0) ^ 0xFF;

	/* send to UART 16 bytes */
	HAL_UART_Transmit_DMA(&huart1, buf, 16);
#endif
}

static int32_t rotary_table[1 << 12][3];

static inline void rotary_dec_table(uint8_t *buf, int len)
{
	int32_t prev;
    int32_t a = 0, b = 0, c = 0;

	HAL_GPIO_WritePin(TP0_GPIO_Port, TP0_Pin, GPIO_PIN_SET);

    prev = instance.prev;

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

	HAL_GPIO_WritePin(TP0_GPIO_Port, TP0_Pin, GPIO_PIN_RESET);
}

static uint8_t dma_buf_data[2 * DMA_BUF_SIZE];

static void dma_cb_half(DMA_HandleTypeDef *hdma)
{
	HAL_GPIO_WritePin(TP1_GPIO_Port, TP1_Pin, GPIO_PIN_RESET);

	instance.s2++;

	rotary_dec_table(dma_buf_data, DMA_BUF_SIZE);
}

static void dma_cb_full(DMA_HandleTypeDef *hdma)
{
	HAL_GPIO_WritePin(TP1_GPIO_Port, TP1_Pin, GPIO_PIN_SET);

	instance.s2++;

	rotary_dec_table(dma_buf_data + DMA_BUF_SIZE, DMA_BUF_SIZE);
}

static void dma_cb_abort(DMA_HandleTypeDef *hdma)
{
	instance.s1++;
}

static void dma_cb_error(DMA_HandleTypeDef *hdma)
{
	instance.s1++;
}

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

int app_init()
{
	return 0;
};

volatile static uint32_t app_100Hz_cnt = 0;

static void app_100Hz_proc(TIM_HandleTypeDef *htim)
{
	app_100Hz_cnt++;
};

int app_begin()
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	rotary_gen_table();
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

	HAL_DMA_RegisterCallback(htim1.hdma[TIM_DMA_ID_UPDATE], HAL_DMA_XFER_CPLT_CB_ID, dma_cb_full);
	HAL_DMA_RegisterCallback(htim1.hdma[TIM_DMA_ID_UPDATE], HAL_DMA_XFER_HALFCPLT_CB_ID, dma_cb_half);
	HAL_DMA_RegisterCallback(htim1.hdma[TIM_DMA_ID_UPDATE], HAL_DMA_XFER_ERROR_CB_ID, dma_cb_error);
	HAL_DMA_RegisterCallback(htim1.hdma[TIM_DMA_ID_UPDATE], HAL_DMA_XFER_ABORT_CB_ID, dma_cb_abort);

	HAL_DMA_Start_IT(htim1.hdma[TIM_DMA_ID_UPDATE], (uint32_t)&(GPIOA->IDR), (uint32_t)dma_buf_data, 2 * DMA_BUF_SIZE);
	__HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_UPDATE ); //Enable the TIM Update DMA request
	__HAL_TIM_ENABLE(&htim1);                 //Enable the Peripheral

	// start generic timer
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_PERIOD_ELAPSED_CB_ID, app_100Hz_proc);
	HAL_TIM_Base_Start_IT(&htim2);

	return 0;
};

int app_loop()
{
	uint32_t app_100Hz_cnt_curr = 0, app_100Hz_cnt_prev = 0;

	while(1)
	{
		/* wait for timer tick */
		app_100Hz_cnt_curr = app_100Hz_cnt;
		if(app_100Hz_cnt_curr == app_100Hz_cnt_prev)
			continue;
		app_100Hz_cnt_prev = app_100Hz_cnt_curr;

		/* report send */
		instance_to_hid();

		// toggle led
		if(!(app_100Hz_cnt_prev % 10))
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	};

	return 0;
};

