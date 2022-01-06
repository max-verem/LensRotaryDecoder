/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_customhid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim3_ch4_up;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern USBD_HandleTypeDef hUsbDeviceFS;

static struct
{
	uint8_t s1, s2;
	int32_t value0, value1, value2;
	uint16_t prev;
} instance;

static inline void instance_to_hid()
{
	uint8_t buf[32];

	buf[0] = instance.s1;
	buf[1] = instance.s2;

	buf[2] = instance.value0 >> 24;
	buf[3] = instance.value0 >> 16;
	buf[4] = instance.value0 >>  8;
    buf[5] = instance.value0 >>  0;

	buf[6] = instance.value1 >> 24;
	buf[7] = instance.value1 >> 16;
	buf[8] = instance.value1 >>  8;
	buf[9] = instance.value1 >>  0;

	buf[10] = instance.value2 >> 24;
	buf[11] = instance.value2 >> 16;
	buf[12] = instance.value2 >>  8;
	buf[13] = instance.value2 >>  0;

	USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buf, 14);
}

static inline void rotary_dec_buf(uint8_t *buf, int len)
{
	int i;
    int32_t a = 0, b = 0, c = 0;

#define ROT1_SHIFT 1	/* PA1, PA2 */
#define ROT2_SHIFT 3	/* PA3, PA4 */
#define ROT3_SHIFT 5	/* PA5, PA6 */

	for(i = 0; i < len; i++)
	{
	    instance.prev <<= 8;
	    instance.prev |= buf[i];

		// counter 1
#ifdef ROT1_SHIFT
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
#endif

		// counter 2
#ifdef ROT2_SHIFT
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
#endif

		// counter 3
#ifdef ROT3_SHIFT
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
#endif

	  }

	  instance.value0 += a;
	  instance.value1 += b;
	  instance.value2 += c;
}

int8_t CUSTOM_HID_OutEvent_FS_main(uint8_t* buf)
{
	instance.s1 = buf[0];
	instance.s2 = buf[1];

	return (USBD_OK);
};

#define DMA_BUF_SIZE 512
uint32_t dma_buf_size = DMA_BUF_SIZE;
uint8_t dma_buf_data[DMA_BUF_SIZE];

static void dma_cb_half(DMA_HandleTypeDef *hdma)
{
	instance.s2++;
}

static void dma_cb_full(DMA_HandleTypeDef *hdma)
{
	instance.s1++;
}


#define BUF1_SIZE	1024
uint8_t buf1[BUF1_SIZE];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
    for(int j = 0; j < BUF1_SIZE; j += 8)
    {
    	const static uint8_t cs[8] =
    	{
   			1,  // 01
   			0,  // 00
   			0,  // 00
   			2,  // 10
   			2,  // 10
   			3,  // 11
   			3,  // 11
   			1,  // 01
    	};

    	const static uint8_t ccs[8] =
    	{
   			2,  // 10
   			0,  // 00
   			0,  // 00
   			1,  // 01
   			1,  // 01
   			3,  // 11
   			3,  // 11
   			2,  // 10
    	};

    	for(int k = 0; k < 8; k ++)
    		buf1[j + k] =
    			cs[k] << ROT1_SHIFT |
				ccs[k] << ROT2_SHIFT |
				cs[k] << ROT3_SHIFT;
    }
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_DMA_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  /*
   * htim3.hdma[TIM_DMA_ID_UPDATE] or hdma_tim3_ch1_trig ?
   *
   */
  HAL_DMA_RegisterCallback(htim3.hdma[TIM_DMA_ID_UPDATE], HAL_DMA_XFER_CPLT_CB_ID, dma_cb_full);
  HAL_DMA_RegisterCallback(htim3.hdma[TIM_DMA_ID_UPDATE], HAL_DMA_XFER_HALFCPLT_CB_ID, dma_cb_half);
  HAL_DMA_Start_IT(htim3.hdma[TIM_DMA_ID_UPDATE], (uint32_t)&GPIOA->IDR, (uint32_t)dma_buf_data, dma_buf_size);
  //  ?
  __HAL_TIM_ENABLE_DMA(&htim3, TIM_DMA_UPDATE);
  __HAL_TIM_ENABLE(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // toggle led
	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

	  /* test sequence send */
	  instance_to_hid();

	  /* test processing */
	  int i;
	  for(i = 0; i < 8000; i++)
		  rotary_dec_buf(buf1, BUF1_SIZE);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 96;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 10;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA2 PA3 PA4
                           PA5 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

