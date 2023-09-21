#include "led.h"
#include "main.h"
#include "gpio.h"
#include "stdbool.h"
#include "timeCheck.h"
#include "stdio.h"
#include "colorControl.h"
#ifndef DISABLE_ALL_LOG
#define ENABLE_LOG_LED
#endif
#ifdef ENABLE_LOG_LED
#define log_info(format, ... ) {printf("LED: "); printf(format , ##__VA_ARGS__);}
#else
#define log_info(format, ... )
#endif

uint16_t led2_pin[NUM_LED2] = {LED2_RED_Pin,LED2_BLU_Pin,LED2_GRE_Pin};			//phan cung han nguoc 2 pin
extern color_t colorValueLed2;
bool led2IsOn[NUM_LED2]  = {false, false, false};

bool led2IsEnAll = true;
bool led2IsOnAll = false;
bool led2IsBlink = false;
uint32_t periodBlinkLed2 = 500;
uint32_t timeStartBlinkLed2= 0;

extern uint32_t g_sys_tick;
/*led bao trang thai MCU*/
void led_toggleStatusMcu(uint16_t nTickToggle)		//10ms
{

    static uint16_t cnt = 0;
	cnt++;
    if(cnt == nTickToggle)
    {
				HAL_GPIO_TogglePin(LED_MCU_GPIO_Port, LED_MCU_Pin);

        cnt = 0;
    }
}


void led2_startShow()
{

	//toggle all
	for(uint8_t index = 0; index < NUM_LED2; index++)
	{
		for(int i = 0; i<4; i++)
		{
			LED2_ON(led2_pin[index]);
			HAL_Delay(300);
			LED2_OFF(led2_pin[index]);
			HAL_Delay(300);
		}
	}
	for(uint8_t index = 0; index < NUM_LED2; index++)
	{
		LED2_ON(led2_pin[index]);
		HAL_Delay(500);
	}


	for(uint8_t index = 0; index < NUM_LED2; index++)
	{
		LED2_OFF(led2_pin[index]);
		HAL_Delay(500);
	}

}


void led2_onOffLeds(bool isOnRed, bool isOnGreen, bool isOnBlue)
{
	if(led2IsEnAll)
	{
		if(isOnRed != led2IsOn[LED2_RED_ID])		//neu khac trang thai cu thi moi thay doi
		{
			if(isOnRed) LED2_ON(led2_pin[LED2_RED_ID]);
			else LED2_OFF(led2_pin[LED2_RED_ID]);
			led2IsOn[LED2_RED_ID] = isOnRed;
		}

		if(isOnGreen != led2IsOn[LED2_GREEN_ID])		//neu khac trang thai cu thi moi thay doi
		{
			if(isOnGreen) LED2_ON(led2_pin[LED2_GREEN_ID]);
			else LED2_OFF(led2_pin[LED2_GREEN_ID]);
			led2IsOn[LED2_GREEN_ID] = isOnGreen;
		}

		if(isOnBlue != led2IsOn[LED2_BLUE_ID])		//neu khac trang thai cu thi moi thay doi
		{
			if(isOnBlue) LED2_ON(led2_pin[LED2_BLUE_ID]);
			else LED2_OFF(led2_pin[LED2_BLUE_ID]);
			led2IsOn[LED2_BLUE_ID] = isOnBlue;
		}
	}

}

void led2_offAllLed()
{
	LED2_OFF_ALL;
	for(int i = 0; i< NUM_LED2; i++)
	{
		led2IsOn[i] = false;
	}
}

void led2_onAllLed()
{
	LED2_ON_ALL;
	for(int i = 0; i< NUM_LED2; i++)
	{
		led2IsOn[i] = true;
	}

}

void led2_enable()
{
	led2IsEnAll = true;
}

void led2_disable()
{
	LED2_OFF_ALL;
	led2IsEnAll = false;

}
void led2_blink(uint16_t period)
{
	led2IsBlink = true;
	periodBlinkLed2 = period;
	timeStartBlinkLed2 = g_sys_tick;
	log_info("set blink period: %d\r\n",period);
}

void led2_stopBlink()
{
	led2IsEnAll = true;
//	led2_offAllLed();
	led2_onAllLed();
	led2IsBlink = false;
	log_info("stop blink\r\n")
}

void led2_colorScan(uint8_t bitpos)
{
	bool isR, isG, isB;
	if(led2IsEnAll)
	{
		isR = (colorValueLed2.r & bitpos)? 1 : 0;
		isG = (colorValueLed2.g & bitpos)? 1 : 0;
		isB = (colorValueLed2.b & bitpos)? 1 : 0;
		led2_onOffLeds(isR, isG, isB);
	}

}

void led_tick()		//10ms
{
	if(led2IsBlink)
	{
		if(elapsedTime(g_sys_tick, timeStartBlinkLed2) > periodBlinkLed2)
		{
			timeStartBlinkLed2 = g_sys_tick;
			led2IsEnAll ^= 1;
			if(!led2IsEnAll) led2_offAllLed();
			//else led2_onAllLed();		//khi dung ngat de pha mau thi k can goi ham nay
		}
	}
}




