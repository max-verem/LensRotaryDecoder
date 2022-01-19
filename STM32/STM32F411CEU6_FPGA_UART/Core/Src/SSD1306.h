#ifndef SSD1306_h
#define SSD1306_h

enum
{
	SSD1306_FB_IDLE = 0,
	SSD1306_FB_READY,
	SSD1306_FB_POS0,
	SSD1306_FB_BLT,
	SSD1306_FB_NONE
};

typedef struct SSD1306_ctx_desc
{
	I2C_HandleTypeDef *hi2c;
	int state, cb_error_cnt, cb_complete_cnt, width, height;
	uint8_t addr;
	uint8_t fb[2048];
	void (*cb_complete_proc)(struct __I2C_HandleTypeDef *hi2c);
	void (*cb_error_proc)(struct __I2C_HandleTypeDef *hi2c);
	int font_width, font_height;
	uint8_t *font_map;
	int text_width, text_height;
} SSD1306_ctx_t;

#define SSD1306_I2C_ADDR 0x3C
#define SSD1306_adress 0x78
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SWITCHCAPVCC 0x2
#define SSD1306_NOP 0xE3

static inline int SSD1306_CMD(SSD1306_ctx_t *ctx, uint8_t C)
{
	uint8_t buf[] = {0x00, C};
    return HAL_I2C_Master_Transmit(ctx->hi2c, ctx->addr << 1, buf, 2, 100);
};

static void SSD1306_cb_complete(SSD1306_ctx_t *ctx)
{
	int __attribute__((unused)) r = 0, sz = ctx->width * ctx->height / 8;
	static uint8_t buf0[] = {0x00, 0xB0 | 0x00 /* y low 4 bits */, 0x01 /* x low 4 bits */, 0x10 | 0x00 /* x high 4 bits */};

	ctx->cb_complete_cnt++;
	switch(ctx->state)
	{
		case SSD1306_FB_READY:
			ctx->state = SSD1306_FB_POS0;
			r = HAL_I2C_Master_Transmit_DMA(ctx->hi2c, ctx->addr << 1, buf0, sizeof(buf0));
			break;

		case SSD1306_FB_POS0:
			ctx->state = SSD1306_FB_BLT;
			ctx->fb[0] = 0x40;
			r = HAL_I2C_Master_Transmit_DMA(ctx->hi2c, ctx->addr << 1, ctx->fb, sz + 1);
			break;

		case SSD1306_FB_IDLE:
		case SSD1306_FB_BLT:
			ctx->state = SSD1306_FB_READY;
			SSD1306_cb_complete(ctx);
			break;
	}
}

static void SSD1306_cb_error(SSD1306_ctx_t *ctx)
{
	ctx->cb_error_cnt++;
}

#define SSD1306_DEF(I2C_NAME, CTX_NAME, ADDR_VAL, WIDTH, HEIGHT, FONT_WIDTH, FONT_HEIGHT, FONT_MAP) \
/*volatile */static SSD1306_ctx_t CTX_NAME; \
\
static void SSD1306_##I2C_NAME##_##CTX_NAME##_cb_complete () { \
	SSD1306_cb_complete((SSD1306_ctx_t*)&CTX_NAME); \
} \
\
static void SSD1306_##I2C_NAME##_##CTX_NAME##_cb_error () { \
	SSD1306_cb_error((SSD1306_ctx_t*)&CTX_NAME); \
} \
\
static inline void SSD1306_##CTX_NAME##_init () { \
	CTX_NAME.addr = ADDR_VAL; \
	CTX_NAME.hi2c = &I2C_NAME; \
	CTX_NAME.width = WIDTH; \
	CTX_NAME.height = HEIGHT; \
	CTX_NAME.font_width = FONT_WIDTH; \
	CTX_NAME.font_height = FONT_HEIGHT; \
	CTX_NAME.font_map = FONT_MAP; \
	CTX_NAME.text_width = WIDTH / FONT_WIDTH; \
	CTX_NAME.text_height = HEIGHT / FONT_HEIGHT; \
	CTX_NAME.cb_complete_proc = SSD1306_##I2C_NAME##_##CTX_NAME##_cb_complete; \
	CTX_NAME.cb_error_proc = SSD1306_##I2C_NAME##_##CTX_NAME##_cb_error; \
} \

static void SSD1306_run(SSD1306_ctx_t *ctx)
{
	int __attribute__((unused)) r = 0;

	/* primary setup */
	r = SSD1306_CMD(ctx, SSD1306_DISPLAYOFF);	// turn off display
	r = SSD1306_CMD(ctx, SSD1306_SETDISPLAYCLOCKDIV);
	r = SSD1306_CMD(ctx, 0x80);
	r = SSD1306_CMD(ctx, SSD1306_SETMULTIPLEX);
	if(ctx->height == 32)
		SSD1306_CMD(ctx, 0x1F);//128×32
	else
		r = SSD1306_CMD(ctx, 0x3F);//128×64
	r = SSD1306_CMD(ctx, SSD1306_SETDISPLAYOFFSET);
	r = SSD1306_CMD(ctx, 0x00);
	r = SSD1306_CMD(ctx, SSD1306_SETSTARTLINE | 0x00);//0
	r = SSD1306_CMD(ctx, SSD1306_CHARGEPUMP);
	r = SSD1306_CMD(ctx, 0x14);
	r = SSD1306_CMD(ctx, SSD1306_MEMORYMODE);
	r = SSD1306_CMD(ctx, 0x00);
	r = SSD1306_CMD(ctx, SSD1306_SEGREMAP | 0x1);
	r = SSD1306_CMD(ctx, SSD1306_COMSCANDEC);
	r = SSD1306_CMD(ctx, SSD1306_SETCOMPINS);
	if(ctx->height == 32)
		SSD1306_CMD(ctx, 0x02);//128×32
	else
		r = SSD1306_CMD(ctx, 0x12);//128×64
	r = SSD1306_CMD(ctx, SSD1306_SETCONTRAST);// Max contrast
	//SSD1306_CMD(ctx, 0x0F);//0xCF
	r = SSD1306_CMD(ctx, 0xCF);//0xCF
	r = SSD1306_CMD(ctx, SSD1306_SETPRECHARGE);
	r = SSD1306_CMD(ctx, 0xF1);
	r = SSD1306_CMD(ctx, SSD1306_SETVCOMDETECT);
	//SSD1306_CMD(ctx, 0x10);//0x40
	r = SSD1306_CMD(ctx, 0x40);//0x40
	r = SSD1306_CMD(ctx, SSD1306_DISPLAYALLON_RESUME);
	r = SSD1306_CMD(ctx, SSD1306_NORMALDISPLAY); // Нормальный режим (не инверсный)
	r = SSD1306_CMD(ctx, SSD1306_DISPLAYON);// Включаем дисплей

	// register callbacks
	HAL_I2C_RegisterCallback(ctx->hi2c, HAL_I2C_MASTER_TX_COMPLETE_CB_ID, ctx->cb_complete_proc);
	HAL_I2C_RegisterCallback(ctx->hi2c, HAL_I2C_ERROR_CB_ID, ctx->cb_error_proc);
	HAL_I2C_RegisterCallback(ctx->hi2c, HAL_I2C_ABORT_CB_ID, ctx->cb_error_proc);

	// run loop
	SSD1306_cb_complete(ctx);
}

static inline void SSD1306_char_put_at(SSD1306_ctx_t *ctx, int row, int col, char ch)
{
	uint32_t* font = (uint32_t*)(ctx->font_map + ch * ctx->font_width);
	uint32_t* fb = (uint32_t*)(ctx->fb + row * ctx->width + col * ctx->font_width);
	fb[0] = font[0];
	fb[1] = font[1];
}

static inline void SSD1306_text_cls(SSD1306_ctx_t *ctx)
{
	int i, j;

	for(i = 0; i < ctx->text_height; i++)
		for(j = 0; j < ctx->text_height; j++)
			SSD1306_char_put_at(ctx, i, j, ' ');
}


static inline void SSD1306_text_put_at(SSD1306_ctx_t *ctx, int row, int col, char* txt)
{
	int i, c;

	if(row < 0 || row >= ctx->text_height)
		return;

	for(i = col, c = 0; i < ctx->text_width && txt[c]; i++, c++)
		SSD1306_char_put_at(ctx, row, i, txt[c]);
}

#endif
