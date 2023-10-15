#ifndef SSD1306_h
#define SSD1306_h

#include "TextScreen.h"

enum
{
	SSD1306_FSM_IDLE = 0,
	SSD1306_FSM_SETPAGECOLROW,
	SSD1306_FSM_TRANSFER_PAGE,
	SSD1306_FSM_NONE
};
#define SSD1306_FSM_READY SSD1306_FSM_SETPAGECOLROW


typedef struct SSD1306_ctx_desc
{
	I2C_HandleTypeDef *hi2c;
	uint8_t dma_buf[132];
	uint8_t current_page, addr, column_shift;
	TextScreen_t* screen;

	int state, cb_error_cnt, cb_complete_cnt, height;
	void (*cb_complete_proc)(struct __I2C_HandleTypeDef *hi2c);
	void (*cb_error_proc)(struct __I2C_HandleTypeDef *hi2c);
} SSD1306_ctx_t;


#define SSD1306_DEF(CTX_NAME) \
\
static SSD1306_ctx_t CTX_NAME; \
\
static void SSD1306_##I2C_NAME##_##CTX_NAME##_cb_complete () { \
	SSD1306_cb_complete((SSD1306_ctx_t*)&CTX_NAME); \
} \
\
static void SSD1306_##I2C_NAME##_##CTX_NAME##_cb_error () { \
	SSD1306_cb_error((SSD1306_ctx_t*)&CTX_NAME); \
} \
\
static SSD1306_ctx_t CTX_NAME = { \
	.cb_complete_proc = SSD1306_##I2C_NAME##_##CTX_NAME##_cb_complete, \
	.cb_error_proc = SSD1306_##I2C_NAME##_##CTX_NAME##_cb_error \
} \

#define SSD1306_I2C_ADDR 0x3C

void SSD1306_init(SSD1306_ctx_t *ctx, I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t height, uint8_t column_shift, TextScreen_t* screen);
int SSD1306_setup(SSD1306_ctx_t *ctx);
void SSD1306_run(SSD1306_ctx_t *ctx);

void SSD1306_cb_complete(SSD1306_ctx_t *ctx);
void SSD1306_cb_error(SSD1306_ctx_t *ctx);

#endif
