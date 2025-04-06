/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define Lo(PIN) (PIN << 16)
#define Hi(PIN) (PIN <<  0)

static uint32_t seq1[4] = {
	Lo(ROT_A1_Pin) | Lo(ROT_B1_Pin),
	Hi(ROT_A1_Pin) | Lo(ROT_B1_Pin),
	Hi(ROT_A1_Pin) | Hi(ROT_B1_Pin),
	Lo(ROT_A1_Pin) | Hi(ROT_B1_Pin)
};

static uint32_t seq2[4] = {
	Lo(ROT_A2_Pin) | Lo(ROT_B2_Pin),
	Hi(ROT_A2_Pin) | Lo(ROT_B2_Pin),
	Hi(ROT_A2_Pin) | Hi(ROT_B2_Pin),
	Lo(ROT_A2_Pin) | Hi(ROT_B2_Pin)
};

static uint32_t seq3[4] = {
	Lo(ROT_A3_Pin) | Lo(ROT_B3_Pin),
	Hi(ROT_A3_Pin) | Lo(ROT_B3_Pin),
	Hi(ROT_A3_Pin) | Hi(ROT_B3_Pin),
	Lo(ROT_A3_Pin) | Hi(ROT_B3_Pin)
};

static void rotary_seq_out(TIM_HandleTypeDef* tim, GPIO_TypeDef *gpio, uint32_t *buf, int size)
{
	HAL_DMA_Start_IT(
		tim->hdma[TIM_DMA_ID_UPDATE],
		(uint32_t)buf,
		(uint32_t)&(gpio->BSRR), size);
	__HAL_TIM_ENABLE_DMA(tim, TIM_DMA_UPDATE ); 	//Enable the TIM Update DMA request
	__HAL_TIM_ENABLE(tim);                 		//Enable the Peripheral
}

#define CNT_MAX 3
#define CNT_PREV 0
#define CNT_CURR 1
volatile int cnt[CNT_MAX][2];
static uint32_t *seqs[CNT_MAX] = {seq1, seq2, seq3};

#define MAX_ROT_COUNT 128
static uint32_t rotary_seq_buf[2][MAX_ROT_COUNT];
volatile int rotary_seq_idx = 0, rotary_seq_size = 0;

static void timer_2ms(TIM_HandleTypeDef *htim)
{
	int i, j;
	uint32_t *rotary_seq_data = rotary_seq_buf[rotary_seq_idx];

	HAL_GPIO_WritePin(TP1_GPIO_Port, TP1_Pin, rotary_seq_idx ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(TP2_GPIO_Port, TP2_Pin, GPIO_PIN_SET);

	/* send rotary sequence if present */
	if(rotary_seq_size)
	{
		/* send */
		rotary_seq_out(&htim1, ROT_A3_GPIO_Port, rotary_seq_data, rotary_seq_size);

		/* rotate buffer */
		rotary_seq_size = 0;
		rotary_seq_idx ^= 1;
		rotary_seq_data = rotary_seq_buf[rotary_seq_idx];
	}

	for(i = 0; i < MAX_ROT_COUNT; i++)
	{
		uint32_t v = 0, m = 0;

		for(j = 0; j < CNT_MAX; j++)
		{
			int step;

			/* skip cycle if no counter value changed */
			if(cnt[j][CNT_CURR] == cnt[j][CNT_PREV])
				continue;

			/* mark this counter has updated */
			m |= 1 << j;

			/* calc next value */
			if(cnt[j][CNT_PREV] < cnt[j][CNT_CURR])
				cnt[j][CNT_PREV]++;
			else
				cnt[j][CNT_PREV]--;

			/* calc step */
			step = cnt[j][CNT_PREV] % 4;
			if(step < 0) step += 4;

			/* store value */
			v |= seqs[j][step];
		};

		/* check if any number written */
		if(!m)
			break;

		/* store to rot array */
		rotary_seq_data[i] = v;
	}
	rotary_seq_size = i;

	HAL_GPIO_WritePin(TP2_GPIO_Port, TP2_Pin, GPIO_PIN_RESET);
};


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	uint8_t j = 0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  // start generic timer
  HAL_TIM_RegisterCallback(&htim3, HAL_TIM_PERIOD_ELAPSED_CB_ID, timer_2ms);
  HAL_TIM_Base_Start_IT(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	/* wait for key press */
	while(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_SET);

	/* fake next counters */
	__disable_irq();
	cnt[0][CNT_CURR] += 200; // + rotary_seq_idx;
	__enable_irq();
	HAL_Delay(1);
	__disable_irq();
	cnt[1][CNT_CURR] -= 200; // + rotary_seq_idx;
	__enable_irq();
	HAL_Delay(1);
	__disable_irq();
	cnt[2][CNT_CURR] += j ? 100 : -100;
	__enable_irq();

	j ^= 1;

    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

	/* sleep for contact bounces */
	HAL_Delay(200);

	/* wait for key release */
	while(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET);

	/* sleep for contact bounces */
	HAL_Delay(200);

    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
