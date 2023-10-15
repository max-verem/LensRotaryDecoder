#include <stdint.h>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_i2c.h>

#include "main.h"
#include "usb_device.h"

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart1;

#include "TextScreen.h"

TextScreen_t text_screen_oled1;
TextScreen_t text_screen_oled2;

#include "SSD1306.h"

SSD1306_DEF(oled1);
SSD1306_DEF(oled2);

// ---------------------------------

extern USBD_HandleTypeDef hUsbDeviceFS;

int8_t CUSTOM_HID_OutEvent_FS_main(uint8_t* buf)
{
	return (USBD_OK);
};

#define TXT_PAD "                          "

#include "int2text.h"

// -----------------------------------------------------------------------
#include "W5500_custom.h"

#define W5500_SPI hspi1

#include "socket.h"
#include "dhcp.h"

static void W5500_Select(void) {
	HAL_GPIO_WritePin(W5500_SCS_GPIO_Port, W5500_SCS_Pin,  GPIO_PIN_RESET);
}

static void W5500_Unselect(void) {
    HAL_GPIO_WritePin(W5500_SCS_GPIO_Port, W5500_SCS_Pin,  GPIO_PIN_SET);
}

static void W5500_ReadBuff(uint8_t* buff, uint16_t len) {
    HAL_SPI_Receive(&W5500_SPI, buff, len, HAL_MAX_DELAY);
}

static void W5500_WriteBuff(uint8_t* buff, uint16_t len) {
    HAL_SPI_Transmit(&W5500_SPI, buff, len, HAL_MAX_DELAY);
}

static uint8_t W5500_ReadByte(void) {
    uint8_t byte = 0xFF;
    W5500_ReadBuff(&byte, sizeof(byte));
    return byte;
}

static void W5500_WriteByte(uint8_t byte) {
    W5500_WriteBuff(&byte, sizeof(byte));
}

// ------------------------------------------------

volatile int CS  = 0;

volatile int W5500_ip_assigned = 0;
volatile int W5500_dhcp_retry = 0;
volatile int W5500_init_done = 0;

static wiz_NetInfo net_info = {
#if 0
	.mac  = {0xEA, 0x11, 0x22, 0x33, 0x44, 0xEA},
#else
    .mac  = { 0xBA, 0xDE, 0xC0, 0xDE, 0, 0 },
#endif
    .dhcp = NETINFO_DHCP
};

static wiz_NetInfo *p_net_info = &net_info;

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
#define W5500_FREED_SOCKET    1

// ------------------------------------------------

volatile static uint32_t cnt_uart = 0, cnt_pkt = 0, cnt_udp = 0;
volatile static int32_t rot1 = 100, rot2 = 100, rot3 = 100;

// ---------------------------------------------------

volatile int timer_1k_cnt = 0, W5500_phy = 0, W5500_id = 0;
volatile int demo_c1 = 0, demo_c2 = 0;
volatile uint8_t demo_c3 = 0, demo_c4;
volatile uint8_t trig_udp = 0;

#define TIMER_DIV(DIV) if(!(timer_1k_cnt % DIV))
#define TIMER_DIV_TRIG_UDP		10
#define TIMER_DIV_BLINK 		250
#define TIMER_DIV_PHY_STATUS	200
#define TIMER_DIV_COUNTERS		100
static void timer_1k_cb(TIM_HandleTypeDef *htim)
{
	timer_1k_cnt++;

	TIMER_DIV(TIMER_DIV_TRIG_UDP)
		trig_udp++;

	if(W5500_init_done)
	{
		if(!(timer_1k_cnt % 100))
			if(!W5500_ip_assigned)
				DHCP_time_handler();
	}

	TIMER_DIV(TIMER_DIV_BLINK)
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

	TIMER_DIV(TIMER_DIV_PHY_STATUS)
	{
		// PHY/DEV state
		TextScreen_put_at(&text_screen_oled2, 0,  4, (W5500_phy & 0x04) ? "FULL" : "HALF", 0);
		TextScreen_put_at(&text_screen_oled2, 0,  9, (W5500_phy & 0x02) ? "100" : "10 ", 0);
		TextScreen_put_at(&text_screen_oled2, 0, 13, (W5500_phy & 0x01) ? " UP" : "DWN", 0);
		TextScreen_put_at(&text_screen_oled2, 0, 0, W5500_id == 0x04 ? "WIZ" : "???", 0);
	}

	TIMER_DIV(TIMER_DIV_COUNTERS)
	{
		int len;
		char tmp[32];
		const static uint8_t running[] = "----------\\\\\\\\\\\\\\\\\\\\|||||||||||//////////";

		len = dec_signed_to_str(rot1, tmp);
		TextScreen_put_at(&text_screen_oled1, 2, 7, "" TXT_PAD, 0);
		TextScreen_put_at(&text_screen_oled1, 2, 16 - len, tmp, 0);

		len = dec_signed_to_str(rot2, tmp);
		TextScreen_put_at(&text_screen_oled1, 3, 7, "" TXT_PAD, 0);
		TextScreen_put_at(&text_screen_oled1, 3, 16 - len, tmp, 0);

		len = dec_signed_to_str(rot3, tmp);
		TextScreen_put_at(&text_screen_oled1, 4, 7, "" TXT_PAD, 0);
		TextScreen_put_at(&text_screen_oled1, 4, 16 - len, tmp, 0);

		len = dec_unsigned_to_str(cnt_uart, tmp);
		TextScreen_put_at(&text_screen_oled1, 6, 7, "" TXT_PAD, 0);
		TextScreen_put_at(&text_screen_oled1, 6, 16 - len, tmp, 0);

		len = dec_unsigned_to_str(cnt_pkt, tmp);
		TextScreen_put_at(&text_screen_oled1, 7, 7, "" TXT_PAD, 0);
		TextScreen_put_at(&text_screen_oled1, 7, 16 - len, tmp, 0);

		TextScreen_char_put_at(&text_screen_oled2, 2, 8, running[cnt_udp % sizeof(running)], 0);
	};
}

// ---------------------------------------------------


#define UART1_BUF_SIZE 32
static uint8_t uart1_buf_data[2 * UART1_BUF_SIZE];
volatile static uint32_t uart1_half_cnt = 0, uart1_full_cnt = 0;

static inline int uart1_packet_find(uint8_t* buf)
{
	int i;

	for(i = 0; i < 17; i++)
		if(buf[i] == 0xDE && buf[i + 1] == 0xC0)
			return i;
	return -1;
};

static inline int uart1_packet_parse(uint8_t* buf)
{
	int i;
	uint32_t cs0, cs1 = 0, r1 = 0, r2 = 0, r3 = 0;

	cs0 = buf[2];
	cs0 <<= 8;
	cs0 |= buf[3];
	cs0 ^= 0x0000FFFF;

	for(i = 4; i < 15; i++)
	{
		uint8_t v = buf[i];
		cs1 += v;
		switch(i)
		{
			case 6:
			case 7:
			case 8: r1 <<= 8; r1 |= v; break;
			case 9:
			case 10:
			case 11: r2 <<= 8; r2 |= v; break;
			case 12:
			case 13:
			case 14: r3 <<= 8; r3 |= v; break;
		}
	}

	/* checksum */
	if(cs1 != cs0)
		return -1;

	/* sign restore */
	if(r1 & 0x00800000)
		r1 |= 0xFF000000;
	if(r2 & 0x00800000)
		r2 |= 0xFF000000;
	if(r3 & 0x00800000)
		r3 |= 0xFF000000;

	/* save data */
	rot1 = r1;
	rot2 = r2;
	rot3 = r3;

	/* correct packet counter */
	cnt_pkt++;

	return 0;
};

static void uart1_cb_full(UART_HandleTypeDef *huart)
{
	uart1_full_cnt++;
	cnt_uart++;
};

static void uart1_cb_half(UART_HandleTypeDef *huart)
{
	int r;

	uart1_half_cnt++;

	r = uart1_packet_find(uart1_buf_data);
	if(r >= 0)
		uart1_packet_parse(uart1_buf_data + r);
};

static void uart1_cb_err(UART_HandleTypeDef *huart)
{
	uart1_half_cnt++;
};

// ----------------------------------------

int app_loop()
{
	int len;
	char tmp[64];

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
    	int p = 0;

    	TextScreen_put_at(&text_screen_oled2, 3, p, " ", 0); p++;

		len = dec_unsigned_to_str(net_info.ip[0], tmp);
		TextScreen_put_at(&text_screen_oled2, 3, p, tmp, 0); p += len;
		TextScreen_put_at(&text_screen_oled2, 3, p, ".", 0); p++;

		len = dec_unsigned_to_str(net_info.ip[1], tmp);
		TextScreen_put_at(&text_screen_oled2, 3, p, tmp, 0); p += len;
		TextScreen_put_at(&text_screen_oled2, 3, p, ".", 0); p++;

		len = dec_unsigned_to_str(net_info.ip[2], tmp);
		TextScreen_put_at(&text_screen_oled2, 3, p, tmp, 0); p += len;
		TextScreen_put_at(&text_screen_oled2, 3, p, ".", 0); p++;

		len = dec_unsigned_to_str(net_info.ip[3], tmp);
		TextScreen_put_at(&text_screen_oled2, 3, p, tmp, 0); p += len;
		TextScreen_put_at(&text_screen_oled2, 3, p, "        ", 0);
	};

	static uint8_t addr[4] = {255, 255, 255, 255}; //{10, 1, 5, 57};
	uint16_t port = 50000 + (CS & 0x00ff);
	static uint8_t buf[29] = {0};
	socket(W5500_FREED_SOCKET, Sn_MR_UDP, port, 0);
	trig_udp = 0;
	while(1)
	{
		if(trig_udp)
		{
			trig_udp = 0;
			cnt_udp++;
			{
				uint32_t v;
				uint8_t cs = 0x40;

				cs -= buf[0] = 0xD1;
				cs -= buf[1] = (CS & 0x00ff);

				v = rot1;
				cs -= buf[20] = v >> 16;
				cs -= buf[21] = v >>  8;
				cs -= buf[22] = v >>  0;

				v = rot2;
				cs -= buf[23] = v >> 16;
				cs -= buf[24] = v >>  8;
				cs -= buf[25] = v >>  0;

				v = rot3;
				cs -= buf[26] = v >>  8;
				cs -= buf[27] = v >>  0;

				buf[28] = cs;
			}

			len = sendto(W5500_FREED_SOCKET, buf, sizeof(buf), addr, port);

			W5500_phy = getPHYCFGR();
			W5500_id = getVERSIONR();

			if(trig_udp)
				trig_udp++;
		};
	}

	return 0;
};

int app_init()
{
	return 0;
};

int app_begin()
{
	int i, p;
	char tmp[64];

	/* build CS value used as unique system ID */
	for(i = 0; i < 12; i++)
		CS += *(uint8_t*)(UID_BASE + i);

	/* update net info */
	net_info.mac[4] = CS & 0x00FF;

	/* reset w5500 start */
	HAL_Delay(100);
	W5500_RST_LOW;

	// init text screen
	TextScreen_init(&text_screen_oled1, 8, 16);
	TextScreen_init(&text_screen_oled2, 8, 16);

	// run oled1
	SSD1306_init
	(
		&oled1,
		&hi2c1, 			// i2handle
		SSD1306_I2C_ADDR,	// i2c address: 0x3C
		64,					// display height: 64 or 32
		2,					// column shift: 2 for SH1106 (132x64), 0 for all other cases
		&text_screen_oled1
	);
	SSD1306_setup(&oled1);
	SSD1306_run(&oled1);

	// run oled2
	SSD1306_init
	(
		&oled2,
		&hi2c2, 			// i2handle
		SSD1306_I2C_ADDR,	// i2c address: 0x3C
		64,					// display height: 64 or 32
		2,					// column shift: 2 for SH1106 (132x64), 0 for all other cases
		&text_screen_oled2
	);
	SSD1306_setup(&oled2);
	SSD1306_run(&oled2);

	/* setup uart handling */
	HAL_UART_RegisterCallback(&huart1, HAL_UART_ERROR_CB_ID, uart1_cb_err);
	HAL_UART_RegisterCallback(&huart1, HAL_UART_ABORT_COMPLETE_CB_ID, uart1_cb_err);
	HAL_UART_RegisterCallback(&huart1, HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID, uart1_cb_err);
	HAL_UART_RegisterCallback(&huart1, HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID, uart1_cb_err);
	HAL_UART_RegisterCallback(&huart1, HAL_UART_RX_COMPLETE_CB_ID, uart1_cb_full);
	HAL_UART_RegisterCallback(&huart1, HAL_UART_RX_HALFCOMPLETE_CB_ID, uart1_cb_half);
	HAL_UART_Receive_DMA(&huart1, uart1_buf_data, 2 * UART1_BUF_SIZE);

	/* unreset w5500 start */
	HAL_Delay(100);
	W5500_RST_HIGH;

	// Registering W5500 callbacks...
	reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
	reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
	reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);

	// Calling wizchip_init
	uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};
	wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);

	// Calling DHCP_init()
	setSHAR(net_info.mac);// set MAC address before using DHCP
	DHCP_init(W5500_DHCP_SOCKET, W5500_dhcp_buffer);
	reg_dhcp_cbfunc(W5500_cb_IPAssigned, W5500_cb_IPAssigned, W5500_cb_IPConflict);

	HAL_Delay(100);

	// initial LED pic
	TextScreen_cls(&text_screen_oled1);
	TextScreen_put_at(&text_screen_oled1, 0, 0, " LensRotaryDecod" TXT_PAD, 0);
	TextScreen_put_at(&text_screen_oled1, 1, 0, "----------------" TXT_PAD, 0);
	TextScreen_put_at(&text_screen_oled1, 2, 0, " ZOOM:    122321" TXT_PAD, 0);
	TextScreen_put_at(&text_screen_oled1, 3, 0, "FOCUS:    323323" TXT_PAD, 0);
	TextScreen_put_at(&text_screen_oled1, 4, 0, " IRIS:   5676575" TXT_PAD, 0);
	TextScreen_put_at(&text_screen_oled1, 5, 0, "----------------" TXT_PAD, 0);
	TextScreen_put_at(&text_screen_oled1, 6, 0, "UARTs:    323323" TXT_PAD, 0);
	TextScreen_put_at(&text_screen_oled1, 7, 0, " PKTs:   5676575" TXT_PAD, 0);

	TextScreen_cls(&text_screen_oled2);
	TextScreen_put_at(&text_screen_oled2, 0, 0, "---:XXXX|XXX|XXX" TXT_PAD, 0);
	TextScreen_put_at(&text_screen_oled2, 3, 0, " --dhcp query-- ", 0);

	/* show MAC address */
	for(p = 0, i = 0; i < 6; i++)
	{
		static const char* hex = "0123456789ABCDEF";
		tmp[p++] = hex[(net_info.mac[i] >> 4) & 0x0f];
		tmp[p++] = hex[(net_info.mac[i] >> 0) & 0x0f];
		tmp[p++] = ':';
	}
	tmp[p++] = 0;
	TextScreen_put_at(&text_screen_oled2, 1, 0, tmp, 0);
	TextScreen_put_at(&text_screen_oled2, 2, 0, "                " TXT_PAD, 0);

	// timer task
	HAL_TIM_RegisterCallback(&htim1, HAL_TIM_PERIOD_ELAPSED_CB_ID, timer_1k_cb);
	HAL_TIM_Base_Start_IT(&htim1);

	return 0;
};
