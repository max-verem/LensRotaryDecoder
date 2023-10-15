#include <stdint.h>

#include "TextScreen.h"

void TextScreen_init(TextScreen_t *ctx, const int rows, const int cols)
{
	ctx->rows = rows >  TEXT_SCREEN_ROWS_MAX ? TEXT_SCREEN_ROWS_MAX : rows;
	ctx->cols = cols >  TEXT_SCREEN_COLS_MAX ? TEXT_SCREEN_COLS_MAX : cols;
	TextScreen_cls(ctx);
}

void TextScreen_put_at(TextScreen_t *ctx, const int row, const int col, const char* txt, const uint16_t attr)
{
	int i, c;

	if(row < 0 || row >= ctx->rows)
		return;

	for(i = col, c = 0; i < ctx->cols && txt[c]; i++, c++)
		TextScreen_char_put_at(ctx, row, i, (uint8_t)txt[c], attr);
}

void TextScreen_cls(TextScreen_t *ctx)
{
	int i, j;

	for(j = 0; j < ctx->rows; j++)
		for(i = 0; i < ctx->cols; i++)
			TextScreen_char_put_at(ctx, j, i, ' ', 0);
}

void TextScreen_char_put_at(TextScreen_t *ctx, const int row, const int col, const uint16_t ch, const uint16_t attr)
{
	if(row >= 0 && row < ctx->rows && col >= 0 && col < ctx->cols)
		ctx->fb[row][col] = (attr << 8) | ch;
}
