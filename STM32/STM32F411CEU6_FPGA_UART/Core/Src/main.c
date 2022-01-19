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
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c1_tx;
DMA_HandleTypeDef hdma_i2c2_tx;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern USBD_HandleTypeDef hUsbDeviceFS;

#define PACKET_REQ	20

static uint8_t rx_buffers[2][PACKET_REQ];
static int rx_buf_idx = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t* rx_buffer = rx_buffers[rx_buf_idx];

	/* enqueue receive more */
	rx_buf_idx = 1 - rx_buf_idx;
	HAL_UART_Receive_IT(&huart1, rx_buffers[rx_buf_idx], PACKET_REQ);

	/* send to PC */
	USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, rx_buffer, PACKET_REQ);
};


int8_t CUSTOM_HID_OutEvent_FS_main(uint8_t* buf)
{
	return (USBD_OK);
};

#define FONT_WIDTH 8
#define FONT_HEIGHT 8
static uint8_t font[] =
{
#include "08UKRSTD.h"
		0
};

#include "SSD1306.h"

#define OLED1_I2C_ADDR 0x3C
#define OLED1_SCREEN_WIDTH 128
#define OLED1_SCREEN_HEIGHT 64
SSD1306_DEF(hi2c1, oled1, OLED1_I2C_ADDR, OLED1_SCREEN_WIDTH, OLED1_SCREEN_HEIGHT, FONT_WIDTH, FONT_HEIGHT, font);

#define OLED2_I2C_ADDR 0x3C
#define OLED2_SCREEN_WIDTH 128
#define OLED2_SCREEN_HEIGHT 32
SSD1306_DEF(hi2c2, oled2, OLED2_I2C_ADDR, OLED2_SCREEN_WIDTH, OLED2_SCREEN_HEIGHT, FONT_WIDTH, FONT_HEIGHT, font);

#define TXT_PAD "                          "

#include "int2text.h"

// -----------------------------------------------------------------------

#include "socket.h"
#include "dhcp.h"

static void W5500_Select(void) {
	HAL_GPIO_WritePin(W5500_SCS_GPIO_Port, W5500_SCS_Pin,  GPIO_PIN_RESET);
}

static void W5500_Unselect(void) {
    HAL_GPIO_WritePin(W5500_SCS_GPIO_Port, W5500_SCS_Pin,  GPIO_PIN_SET);
}

static void W5500_ReadBuff(uint8_t* buff, uint16_t len) {
    HAL_SPI_Receive(&hspi1, buff, len, HAL_MAX_DELAY);
}

static void W5500_WriteBuff(uint8_t* buff, uint16_t len) {
    HAL_SPI_Transmit(&hspi1, buff, len, HAL_MAX_DELAY);
}

static uint8_t W5500_ReadByte(void) {
    uint8_t byte = 0xFF;
    W5500_ReadBuff(&byte, sizeof(byte));
    return byte;
}

static void W5500_WriteByte(uint8_t byte) {
    W5500_WriteBuff(&byte, sizeof(byte));
}

volatile int W5500_ip_assigned = 0;
volatile int W5500_dhcp_retry = 0;
volatile int W5500_init_done = 0;

volatile wiz_NetInfo *p_net_info;
static void W5500_cb_IPAssigned(void) {
    W5500_ip_assigned = 1;
    getIPfromDHCP((uint8_t*)p_net_info->ip);
    getGWfromDHCP((uint8_t*)p_net_info->gw);
    getSNfromDHCP((uint8_t*)p_net_info->sn);
}

static void W5500_cb_IPConflict(void) {
	W5500_ip_assigned = 0;
}

static uint8_t W5500_dhcp_buffer[2048];

#define W5500_DHCP_SOCKET     0
#define W5500_UDP_SOCKET      1

// ------------------------------------------------

#include "W5500_custom.h"

volatile int cs = 0;

// ------------------------------------------------

volatile int timer_1k_cnt = 0, W5500_phy = 0, W5500_id = 0;
volatile int demo_c1 = 0, demo_c2 = 0;
volatile uint8_t demo_c3 = 0, demo_c4;
volatile uint8_t trig_udp = 0;
static void timer_1k_cb(TIM_HandleTypeDef *htim)
{
	timer_1k_cnt++;

	if(timer_1k_cnt % 100)
		trig_udp++;

	if(W5500_init_done)
	{
		if(!(timer_1k_cnt % 100))
			DHCP_time_handler();
	}

	if(timer_1k_cnt % 500)
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

	if(timer_1k_cnt % 500)
	{
		// PHY/DEV state
	    SSD1306_text_put_at(&oled2, 0,  4, (W5500_phy & 0x04) ? "FULL" : "HALF");
		SSD1306_text_put_at(&oled2, 0,  9, (W5500_phy & 0x02) ? "100" : "10 ");
		SSD1306_text_put_at(&oled2, 0, 13, (W5500_phy & 0x01) ? " UP" : "DWN");
	    SSD1306_text_put_at(&oled2, 0, 0, W5500_id == 0x04 ? "WIZ" : "???");
	}

	if(timer_1k_cnt % 201)
	{
		int len;
		char tmp[32];

		len = dec_signed_to_str(demo_c1--, tmp);
		SSD1306_text_put_at(&oled1, 2, 7, "" TXT_PAD);
		SSD1306_text_put_at(&oled1, 2, 16 - len, tmp);

		len = dec_signed_to_str(demo_c2++, tmp);
		SSD1306_text_put_at(&oled1, 3, 7, "" TXT_PAD);
		SSD1306_text_put_at(&oled1, 3, 16 - len, tmp);

		len = dec_unsigned_to_str(demo_c3--, tmp);
		SSD1306_text_put_at(&oled2, 3, 0, "" TXT_PAD);
		SSD1306_text_put_at(&oled2, 3, 8 - len, tmp);

		len = dec_unsigned_to_str(demo_c4++, tmp);
		SSD1306_text_put_at(&oled2, 3, 8, "" TXT_PAD);
		SSD1306_text_put_at(&oled2, 3, 16 - len, tmp);
	};
}

// ---------------------------------------------------

#ifdef DEBUG

#include <stdio.h>
#include <errno.h>
//#include  <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO

int _write(int file, char *data, int len)
{
   // arbitrary timeout 1000
   HAL_StatusTypeDef status =
      HAL_UART_Transmit(&huart1, (uint8_t*)data, len, 1000);

   // return # of bytes written - as best we can tell
   return (status == HAL_OK ? len : 0);
}

#endif

// ---------------------------------------------------

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  int i, len, p;
  char tmp[32];
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
  MX_USB_DEVICE_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  W5500_RST_LOW;

  // run oleds
  SSD1306_oled1_init();
  SSD1306_run(&oled1);
  SSD1306_oled2_init();
  SSD1306_run(&oled2);

  // OLED WELCOME
  SSD1306_text_cls(&oled1);
  SSD1306_text_put_at(&oled1, 1, 1, "Lens" TXT_PAD);
  SSD1306_text_put_at(&oled1, 3, 1, "Rotary" TXT_PAD);
  SSD1306_text_put_at(&oled1, 4, 1, "Decoder" TXT_PAD);
  SSD1306_text_put_at(&oled1, 7, 1, "initializing #3" TXT_PAD);
  SSD1306_text_cls(&oled2);
  SSD1306_text_put_at(&oled2, 3, 1, "HELLO WORLD!!" TXT_PAD);

  HAL_Delay(100);

  W5500_RST_HIGH;

  // Registering W5500 callbacks...
  reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
  reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
//  reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);

  // Calling wizchip_init
  uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};
  wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);

  // Calling DHCP_init()
  for(i = 0; i < 12; i++)
	  cs += *(uint8_t*)(UID_BASE + i);
  wiz_NetInfo net_info = {
#if 0
      .mac  = {0xEA, 0x11, 0x22, 0x33, 0x44, 0xEA},
#else
      .mac  = { 0xBA, 0xDE, 0xC0, 0xDE, cs & 0x00FF, 0 },
#endif
      .dhcp = NETINFO_DHCP
  };
  p_net_info = &net_info;
  setSHAR(net_info.mac);// set MAC address before using DHCP
  DHCP_init(W5500_DHCP_SOCKET, W5500_dhcp_buffer);
  reg_dhcp_cbfunc(W5500_cb_IPAssigned, W5500_cb_IPAssigned, W5500_cb_IPConflict);

  HAL_Delay(100);

  // initial LED pic
  SSD1306_text_cls(&oled1);
  SSD1306_text_put_at(&oled1, 0, 0, "LensRotaryDecoder" TXT_PAD);
  SSD1306_text_put_at(&oled1, 1, 0, "----------------" TXT_PAD);
  SSD1306_text_put_at(&oled1, 2, 0, " ZOOM:    122321" TXT_PAD);
  SSD1306_text_put_at(&oled1, 3, 0, "FOCUS:    323323" TXT_PAD);
  SSD1306_text_put_at(&oled1, 4, 0, " IRIS:   5676575" TXT_PAD);
  SSD1306_text_put_at(&oled1, 5, 0, "----------------" TXT_PAD);
  SSD1306_text_put_at(&oled1, 6, 0, "FOCUS:    323323" TXT_PAD);
  SSD1306_text_put_at(&oled1, 7, 0, " IRIS:   5676575" TXT_PAD);

  SSD1306_text_cls(&oled2);
  SSD1306_text_put_at(&oled2, 0, 0, "---:XXXX|XXX|XXX" TXT_PAD);
  SSD1306_text_put_at(&oled2, 1, 0, " --dhcp query-- ");

  /* show MAC address */
  for(p = 0, i = 0; i < 6; i++)
  {
	  static const char* hex = "0123456789ABCDEF";
	  tmp[p++] = hex[(net_info.mac[i] >> 4) & 0x0f];
	  tmp[p++] = hex[(net_info.mac[i] >> 0) & 0x0f];
	  tmp[p++] = ':';
  }
  tmp[p++] = 0;
  SSD1306_text_put_at(&oled2, 2, 0, tmp);

  SSD1306_text_put_at(&oled2, 3, 0, "................" TXT_PAD);

  // timer task
  HAL_TIM_RegisterCallback(&htim1, HAL_TIM_PERIOD_ELAPSED_CB_ID, timer_1k_cb);
  HAL_TIM_Base_Start_IT(&htim1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  W5500_init_done = 1;

	  /* wait until DHCP server find ip */
      while(!W5500_ip_assigned)
      {
  		W5500_phy = getPHYCFGR();
  		W5500_id = getVERSIONR();
        DHCP_run();
      }

      /* setup found IP address */
      wizchip_setnetinfo(&net_info);

	  /* update ip on screen */
	  {
		  p = 0;

		  SSD1306_text_put_at(&oled2, 1, p, "@"); p++;

		  len = dec_unsigned_to_str(net_info.ip[0], tmp);
		  SSD1306_text_put_at(&oled2, 1, p, tmp); p += len;
		  SSD1306_text_put_at(&oled2, 1, p, "."); p++;

		  len = dec_unsigned_to_str(net_info.ip[1], tmp);
		  SSD1306_text_put_at(&oled2, 1, p, tmp); p += len;
		  SSD1306_text_put_at(&oled2, 1, p, "."); p++;

		  len = dec_unsigned_to_str(net_info.ip[2], tmp);
		  SSD1306_text_put_at(&oled2, 1, p, tmp); p += len;
		  SSD1306_text_put_at(&oled2, 1, p, "."); p++;

		  len = dec_unsigned_to_str(net_info.ip[3], tmp);
		  SSD1306_text_put_at(&oled2, 1, p, tmp); p += len;
		  SSD1306_text_put_at(&oled2, 1, p, "        ");
	  };

	  while(1)
	  {
		  if(trig_udp)
		  {
			  trig_udp = 0;

			  W5500_phy = getPHYCFGR();
			  W5500_id = getVERSIONR();
		  };
	  }
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 96;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 230400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

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
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, W5500_SCS_Pin|W5500_RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : W5500_SCS_Pin W5500_RST_Pin */
  GPIO_InitStruct.Pin = W5500_SCS_Pin|W5500_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

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

