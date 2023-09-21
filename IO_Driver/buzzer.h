#ifndef _BUZZER_H_
#define _BUZZER_H_
#include "stdint.h"
#include "stdbool.h"

#define BUZZ_TIME_ON 100		//ms
#define BUZZ_TIME_OFF 300		//ms
#define BUZZ_BLINK_PPERIOD 400	//ms

#define BUZZ_TIME_ON_WARNING 100		//ms
#define BUZZ_TIME_OFF_WARNING 100		//ms
#define BUZZ_BLINK_PPERIOD_WARNING 200	//ms

void buzzer_startShow(void);
void buzzer_on1period(void);
void buzzer_on1periodWaring(void);

void buzzer_tick(void);
void buzzer_onInMs(uint16_t ms);
void buzzer_blink(uint16_t times, bool isWarning);
void buzzer_switchEnableOrDisable();

#endif
