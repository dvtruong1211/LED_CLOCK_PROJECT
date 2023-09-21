#include "buzzer.h"
#include "stm32f3xx_hal.h"
#include "timeCheck.h"
#ifndef DISABLE_ALL_LOG
#define ENABLE_LOG_BUZZER
#endif
#ifdef ENABLE_LOG_BUZZER
#define log_info(format, ... ) {printf("BUZZER: "); printf(format , ##__VA_ARGS__);}
#else
#define log_info(format, ... )
#endif

extern volatile uint32_t g_sys_tick;

bool buzzerIsEnable = true;
bool buzzerIsOn = false;
uint32_t timeOffLimit;
uint32_t timeStartBlink = 0;
uint16_t timesBlink = 0;
uint16_t periodBlink = 0;

#define BUZZER_ON 	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET)
#define BUZZER_OFF  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET)


//use delay
void buzzer_on1period()
{
	BUZZER_ON;
	HAL_Delay(BUZZ_TIME_ON);
	BUZZER_OFF;
	HAL_Delay(BUZZ_TIME_OFF);
}

void buzzer_on1periodWaring()
{
	BUZZER_ON;
	HAL_Delay(BUZZ_TIME_ON_WARNING);
	BUZZER_OFF;
	HAL_Delay(BUZZ_TIME_OFF_WARNING);
}
void buzzer_startShow()
{
	for(int i = 0; i< 2;i++)
	{
		buzzer_on1period();
	}
}


//use g_sys_tick
void buzzer_onInMs(uint16_t ms)
{
	if(!buzzerIsEnable) return;
		timeOffLimit = g_sys_tick + ms;
		BUZZER_ON; //on buzzer
		buzzerIsOn = true;
		log_info("on in %d\r\n", ms);
}
void buzzer_blink(uint16_t times, bool isWarning)
{
	if(!buzzerIsEnable)  return;
	periodBlink = (isWarning == 1)? BUZZ_BLINK_PPERIOD_WARNING : BUZZ_BLINK_PPERIOD;
	timeStartBlink = g_sys_tick;
	timesBlink = times;
	log_info("blink %d times, iswarning %d\r\n", times,isWarning);
}


void buzzer_switchEnableOrDisable()
{
	if(!buzzerIsEnable)
	{
		log_info("buzzer enable\r\n");
		buzzerIsEnable = true;
		buzzer_blink(2, false);
	}
	else
	{
		log_info("buzzer disable\r\n");
		buzzer_onInMs(300);
		buzzerIsEnable = false;
	}
}
//check each 10ms
void buzzer_tick()
{
	if(buzzerIsOn)
	{
			if(timeIsAfter(g_sys_tick, timeOffLimit))			//out of timeon
			{
				BUZZER_OFF; //off buzzer
				buzzerIsOn = false;

			}
	}

	if(timesBlink > 0)
	{
		if(timeIsAfter(g_sys_tick, timeStartBlink))
		{

			buzzer_onInMs(100);
			timesBlink--;
			timeStartBlink = g_sys_tick + periodBlink;

		}
	}
}





