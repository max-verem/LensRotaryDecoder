#ifndef TEXT_SCREEN_H
#define TEXT_SCREEN_H

#define TEXT_SCREEN_COLS_MAX 32
#define TEXT_SCREEN_ROWS_MAX 16

#define TEXT_SCREEN_ATTR_INVERT 0x80
#define TEXT_SCREEN_ATTR_BLINK 0x40

#define TXT_PAD "                          "

typedef struct
{
	uint16_t fb[TEXT_SCREEN_ROWS_MAX][TEXT_SCREEN_COLS_MAX];
	uint8_t rows, cols, blink;
} TextScreen_t;

void TextScreen_init(TextScreen_t *ctx, const int rows, const int cols);
void TextScreen_char_put_at(TextScreen_t *ctx, const int row, const int col, const uint16_t ch, const uint16_t attr);
void TextScreen_cls(TextScreen_t *ctx);
void TextScreen_put_at(TextScreen_t *ctx, const int row, const int col, const char* txt, uint16_t attr);

#endif /* TEXT_SCREEN_H */
