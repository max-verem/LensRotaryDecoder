#ifndef SRC_APP_H_
#define SRC_APP_H_

#define ROTARY_TABLE

int app_init();
int app_begin();
int app_loop();

#ifdef ROTARY_TABLE
#define LED_CNT             1000	// 10KHz / 1000 = 10Hz
#define SEQ_CNT             100		// 10KHz / 100 = 100Hz
#define DMA_BUF_SIZE        400		// 4MHz / 400 = 10KHz
#define TIMER_PRESCALER     8 // 96MHz / 8 = 12Mhz
#define TIMER_PERIOD        3 // 12Mhz / 3 = 4MHz
#else
#define LED_CNT             100
#define SEQ_CNT             10
#define DMA_BUF_SIZE        100
#define TIMER_PRESCALER     96 // 96MHz / 96 = 1Mhz
#define TIMER_PERIOD        10 // 1Mhz / 10 = 100 KHz
#endif

/*
NB:

[...]
  htim1.Init.Prescaler = TIMER_PRESCALER - 1;
[...]
  htim1.Init.Period = TIMER_PERIOD - 1;
[...]

 */

#endif /* SRC_APP_H_ */
