#ifndef SRC_APP_H_
#define SRC_APP_H_

#define ROTARY_TABLE

int app_init();
int app_begin();
int app_loop();

#define DMA_BUF_SIZE        400	// 4MHz / 400 = 10KHz
#define TIMER_PRESCALER     8 	// 96MHz / 8 = 12Mhz
#define TIMER_PERIOD        2 	// 12Mhz / 2 = 6MHz
//#define SERIAL_OUTPUT_TEXT	// This defines if UART1 sent with human readable text or binary formatted packet

/*
NB:

[...]
  htim1.Init.Prescaler = TIMER_PRESCALER - 1;
[...]
  htim1.Init.Period = TIMER_PERIOD - 1;
[...]

 */

#endif /* SRC_APP_H_ */
