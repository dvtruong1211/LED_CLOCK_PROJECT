#include "led7seg.h"
#include "hc595.h"
#include "stdio.h"
#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "stm32f3xx_hal_tim.h"
#include "led.h"
#include "timeCheck.h"
#include "colorControl.h"
#include "stdlib.h"
#include "string.h"
#include "rf_transfer.h"
#ifndef DISABLE_ALL_LOG
#define ENABLE_LOG_LED7SEG
#endif
#ifdef ENABLE_LOG_LED7SEG
#define log_info(format, ... ) {printf("LED7SEG: "); printf(format , ##__VA_ARGS__);}
#else
#define log_info(format, ... )
#endif


#define SCAN_WAY_1	0		// =1 thi quet het các led roi moi chuyen sang bit mau tiep theo
							// = 0 thi het 8 bit mau cua led 1 roi moi chuyen sang led 2


const uint8_t CODE_7SEG_CHAR[NUM_CODE_7SEG_CHAR] =  {0xFF, 0xBF, 0x88, 0x83, 0xA7, 0xA1, 0x86, 0x8E};
const uint8_t CODE_7SEG[NUM_CODE_7SEG] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
											0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E,0xFF, 0xBF, 0xAB, 0x87, 0x9C};
led7seg_blink_t blink[NUM_LED7_RTC+1] = {
		[LED7_HOUR1] = {true, false, 500, 0},
		[LED7_HOUR2] = {true, false, 500, 0},
		[LED7_MIN1] =  {true, false, 500, 0},
		[LED7_MIN2] =  {true, false, 500, 0},
		[LED7_SEC1] =  {true, false, 500, 0},
		[LED7_SEC2] =  {true, false, 500, 0},
		[LED7_ALL] =   {true, false, 500, 0}
};
bool led7Blink = false;

extern uint32_t g_sys_tick;

uint8_t s_valueLedHour = 0, s_valueLedMin = 0, s_valueLedSec = 0;
uint16_t s_valueTotal6Led = 0;
uint8_t s_codeLed7[NUM_LED7_RTC] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
uint8_t codeLed7Blink[NUM_LED7_RTC]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
//bien de scan mau
extern color_t colorValueLed7[NUM_LED7_RTC];		//luu gia tri mau ung voi tung led7seg
					//code_r, code_g, code_b
codeColor_t codeColor = {0xff, 0xff, 0xff};		//luu ma code tuc thoi hien thi, mac dinh ban dau code ca 3 mau giong nhau (tuc la ban ca 3 mau)


//voi do phan giai 8 bit color thi thoi gian pha mau chia ra 8 khoang thoi gian nho,
//thoi gian cho moi khoang phu thuoc trong so cua bit do
//thoi gian nay nap lai cho ngat timer
uint16_t timeSlide[RESOLUTION_BIT] = {
										BIT(0) * TIME_MIX_1BIT_COLOR , BIT(1) * TIME_MIX_1BIT_COLOR ,
										BIT(2) * TIME_MIX_1BIT_COLOR , BIT(3) * TIME_MIX_1BIT_COLOR ,
										BIT(4) * TIME_MIX_1BIT_COLOR , BIT(5) * TIME_MIX_1BIT_COLOR ,
										BIT(6) * TIME_MIX_1BIT_COLOR , BIT(7) * TIME_MIX_1BIT_COLOR };

volatile uint8_t ledIndexScan = 0;
uint8_t bitIndexColor = 0;
uint8_t bitpos = 0x01;
void led7seg_enableLedIndex(uint8_t index)
{
	HAL_GPIO_WritePin(A0_138_GPIO_Port, A0_138_Pin, index & 0x01);
	HAL_GPIO_WritePin(A1_138_GPIO_Port, A1_138_Pin, index & 0x02);
	HAL_GPIO_WritePin(A2_138_GPIO_Port, A2_138_Pin, index & 0x04);
}

/*su dung 74138 ma NUM_LED7_RTC = 6 nen dia chi A0A1A2 = 6 hoac 7 thi se enable chan k ket noi voi led*/
void led7seg_disableAll()
{
	led7seg_enableLedIndex(7);
}
void led7seg_offAll()
{
	hc595_outDataRGB(CODE_7SEG[CODE_OFF],CODE_7SEG[CODE_OFF],CODE_7SEG[CODE_OFF]);
}
void led7seg_showStart()
{
	led7seg_disableAll();
	/*show each segment of all led*/
	for(int i = 0; i <24; i++)
	{
		if(i==7 || i== 15 || i==23) continue;
			uint32_t u24 =0;
			u24 = ~(0x01<<i);
			hc595_out3Byte(u24);
		for(int cnt = 0; cnt < 50;cnt++)					//scan led use delay
		for(int index =0; index < NUM_LED7_RTC; index++)
		{
			led7seg_enableLedIndex(index);
			HAL_Delay(1);
		}
		led7seg_disableAll();
	}
	/*show all segment of each led*/
	for(int i = 0; i< 3; i++)
	{
		hc595_out3Byte(~((uint32_t)0xff << (8*i)));
		for(int index = 0; index < NUM_LED7_RTC; index++)
		{
			led7seg_enableLedIndex(index);
			HAL_Delay(500);
		}
	}
	led7seg_disableAll();
}



void led7seg_encoder(uint8_t valueHour, uint8_t valueMin, uint8_t valueSec)
{
	s_codeLed7[0] = (valueHour/10)? CODE_7SEG[valueHour/10] : CODE_7SEG[CODE_OFF];

	s_codeLed7[1] =CODE_7SEG[ valueHour%10];

	s_codeLed7[2] = CODE_7SEG[valueMin/10];
	s_codeLed7[3] = CODE_7SEG[valueMin%10];

	s_codeLed7[4] = CODE_7SEG[valueSec/10];
	s_codeLed7[5] = CODE_7SEG[valueSec%10];
}
void led7seg_setValueLed(uint8_t valueHour, uint8_t valueMin, uint8_t valueSec)
{
	s_valueLedHour = valueHour;
	s_valueLedMin = valueMin;
	s_valueLedSec = valueSec;
	led7seg_encoder(valueHour, valueMin, valueSec);

}

void led7seg_setValueAtLed(led_couple_7seg_id_t led, uint8_t value)
{
	if(led == LED7_HOUR)
	{
		s_codeLed7[0] = (value/10)? CODE_7SEG[value/10] : CODE_7SEG[CODE_OFF];
		s_codeLed7[1] =CODE_7SEG[ value%10];
		s_valueLedHour = value;
	}
	else if(led == LED7_MIN)
	{
		s_codeLed7[2] = CODE_7SEG[value/10];
		s_codeLed7[3] = CODE_7SEG[value%10];
		s_valueLedMin = value;
	}
	else
	{
		s_codeLed7[4] = CODE_7SEG[value/10];
		s_codeLed7[5] = CODE_7SEG[value%10];
		s_valueLedSec = value;
	}
}
uint8_t led7seg_getValueAtLed(led_couple_7seg_id_t led)
{
	if(led == LED7_HOUR) return s_valueLedHour;
	else if(led == LED7_MIN) return s_valueLedMin;
	else return s_valueLedSec;
}

void led7seg_setCodeIndex(code7seg_id_t code_id, uint8_t indexLed7)
{
	s_codeLed7[indexLed7] = CODE_7SEG[code_id];
}
void led7seg_setCodeAllLed(code7seg_id_t code_id)
{
	for(int i = 0; i < NUM_LED7_RTC; i++)
	{
		s_codeLed7[i] = CODE_7SEG[code_id];
	}
}

//called in ISR
void led7seg_scanLed()
{
	ledIndexScan++;
}

extern bool codeEv1527Ready;			//ngat timer htim15 cho truyen RF
/*quet led7seg trong ngat*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == htim3.Instance)
	{
#if (SCAN_WAY_1 == 0)
		if(bitIndexColor == 8)			//het thoi gian scan 1led
		{
			bitpos = 0x01;
			bitIndexColor = 0;
			ledIndexScan++;				//next led
			//led7seg_disableAll();						//off nay di cung voi lenh ***, nhung led bi luu mau led truoc, co the la do 74138 xuat muc 0 co van de
			led7seg_offAll();
			if(ledIndexScan == NUM_LED7_RTC)
			{
				ledIndexScan = 0;
			}
			led7seg_enableLedIndex(ledIndexScan);
		}
		if(led7Blink)
		{
			/*neu tai vi tri bitIndex ma gia tri mau =1 thi cho phep hien thi mau do, =0 thi tat*/
			codeColor.code_r = (colorValueLed7[ledIndexScan].r &  bitpos)? codeLed7Blink[ledIndexScan]: CODE_7SEG[CODE_OFF];
			codeColor.code_g = (colorValueLed7[ledIndexScan].g &  bitpos)? codeLed7Blink[ledIndexScan]: CODE_7SEG[CODE_OFF];
			codeColor.code_b = (colorValueLed7[ledIndexScan].b &  bitpos)? codeLed7Blink[ledIndexScan]: CODE_7SEG[CODE_OFF];
		}
		else{
			/*neu tai vi tri bitIndex ma gia tri mau =1 thi cho phep hien thi mau do, =0 thi tat*/
			codeColor.code_r = (colorValueLed7[ledIndexScan].r &  bitpos)? s_codeLed7[ledIndexScan]: CODE_7SEG[CODE_OFF];
			codeColor.code_g = (colorValueLed7[ledIndexScan].g &  bitpos)? s_codeLed7[ledIndexScan]: CODE_7SEG[CODE_OFF];
			codeColor.code_b = (colorValueLed7[ledIndexScan].b &  bitpos)? s_codeLed7[ledIndexScan]: CODE_7SEG[CODE_OFF];
		}
		hc595_outDataRGB(codeColor.code_r,codeColor.code_g,codeColor.code_b);

		//if(bitIndexColor == 0) led7seg_enableLedIndex(ledIndexScan);  //***

		led2_colorScan(bitpos);
		__HAL_TIM_SET_AUTORELOAD(&htim3, timeSlide[bitIndexColor]);
		__HAL_TIM_SET_COUNTER(&htim3, 0);
		//set counter ve 0 neu khong gia tri ngat nho qua luc dang thuc thien ISR thi lai co ngat tiep theo nhung bi bo qua
		//do do counter se dem qua gia tri period, khi do 1 so timeslide k thuc hien

		bitIndexColor++;
		bitpos<<= 1;


#elif (SCAN_WAY_1 == 1)
				ledIndexScan++;				//next led
				if(ledIndexScan == NUM_LED7_RTC)
				{
					ledIndexScan = 0;
					bitIndexColor++;
					bitpos<<= 1;
					if(bitpos == 0x00)			//het thoi gian scan 1led
					{
						bitpos = 0x01;
						bitIndexColor = 0;

					}
					led2_colorScan(bitpos);
					__HAL_TIM_SET_AUTORELOAD(&htim3, timeSlide[bitIndexColor]);
				}
				led7seg_offAll();
				led7seg_enableLedIndex(ledIndexScan);
	//			led7seg_disableAll();//off nay di cung voi lenh ***, nhung led bi luu mau led truoc, co the la do 74138 xuat muc 0 co van de
				/*neu tai vi tri bitIndex ma gia tri mau =1 thi cho phep hien thi mau do, =0 thi tat*/
				codeColor.code_r = (colorValueLed7[ledIndexScan].r &  bitpos)? s_codeLed7[ledIndexScan]: CODE_7SEG[CODE_OFF];
				codeColor.code_g = (colorValueLed7[ledIndexScan].g &  bitpos)? s_codeLed7[ledIndexScan]: CODE_7SEG[CODE_OFF];
				codeColor.code_b = (colorValueLed7[ledIndexScan].b &  bitpos)? s_codeLed7[ledIndexScan]: CODE_7SEG[CODE_OFF];

				hc595_outDataRGB(codeColor.code_r,codeColor.code_g,codeColor.code_b);
	//			led7seg_enableLedIndex(ledIndexScan);  //***
				__HAL_TIM_SET_COUNTER(&htim3, 0);

#endif
	}
	else if(htim->Instance == htim15.Instance)
	{
		/* Retrieve the interrupt status and the counter value from the timer that reported the interrupt */
		if(codeEv1527Ready)
		{
			rf_transfer_codeEv1527_steps();
		}
	}
}

void led7seg_setCodeBlink(led7_id_t ledId, bool isOn)
{
	if(isOn)
	{
		if(ledId == LED7_ALL)
		{
			memcpy(codeLed7Blink, s_codeLed7, sizeof(codeLed7Blink));
		}
		else
		{
			codeLed7Blink[ledId] = s_codeLed7[ledId];
		}
	}
	else
	{
		if(ledId == LED7_ALL)
		{
			memset(codeLed7Blink, 0xff, sizeof(codeLed7Blink));
		}
		else
		{
			codeLed7Blink[ledId] = 0xff;
		}
	}
}
void led7seg_setBlinkLed(led7_id_t ledId, uint32_t period)
{

	if((ledId != LED7_ALL) && (blink[LED7_ALL].isBlink == true)) return;			//neu blink all thi k blink tung led nua
	else
	{
		if(!led7Blink)
		{
			memcpy(codeLed7Blink, s_codeLed7, sizeof(codeLed7Blink));
			led7Blink = true;
		}
		blink[ledId].isBlink = true;
		blink[ledId].isOn = true;
		blink[ledId].periodBlink = period;
		blink[ledId].timeStartBlink = g_sys_tick;
		log_info("set blink ledId: %d , period: %d\r\n",ledId, (int)period);
	}
}

void led7seg_stopBlinkLed(led7_id_t ledId)
{
	blink[ledId].isBlink = false;
	blink[ledId].isOn	= true;
	led7seg_setCodeBlink(ledId, true);
	for(int i = 0; i< NUM_LED7_RTC+1; i++)
	{
		if(blink[i].isBlink == true) return;
	}
	led7Blink = false;									//neu k con led nao trang thai blink thi tat blink

}

void led7seg_blinkLed(led_couple_7seg_id_t ledType, bool isBlink, uint32_t period)
{
	led7_id_t led7Id1 = LED7_HOUR1, led7Id2 =LED7_HOUR2;
	if(ledType == LED7_HOUR) 	 {led7Id1 = LED7_HOUR1; led7Id2 = LED7_HOUR2;}
	else if(ledType == LED7_MIN) {led7Id1 = LED7_MIN1; led7Id2 = LED7_MIN2;}
	else if(ledType == LED7_SEC) {led7Id1 = LED7_SEC1; led7Id2 = LED7_SEC2;}
	if(isBlink)
	{
		led7seg_setBlinkLed(led7Id1, period);
		led7seg_setBlinkLed(led7Id2, period);
	}
	else
	{
		led7seg_stopBlinkLed(led7Id1);
		led7seg_stopBlinkLed(led7Id2);
	}
}
void led7seg_stopBlinkAll()
{
	if(led7Blink)
	{
		led7seg_offAll();
		led7Blink = false;
		for(int i = 0; i< NUM_LED7_RTC+1; i++)
		{
			blink[i].isBlink = false;				//tranh khi blink lai led nao do thi cac led khac k blink, coi nhu reset trang thai
		}
		log_info("stop blink all\r\n");
	}
}
void led7seg_tick()
{
	if(led7Blink)
	for(int i = 0; i< NUM_LED7_RTC+1; i++)
	{
		if(blink[i].isBlink)
		{
			if(elapsedTime(g_sys_tick, blink[i].timeStartBlink) > blink[i].periodBlink)
			{
				blink[i].timeStartBlink = g_sys_tick;
				blink[i].isOn ^= 1;
				led7seg_setCodeBlink((led7_id_t)i, blink[i].isOn);
			}
		}
		else
		{
			if((i!= LED7_ALL) && (blink[LED7_ALL].isBlink == false))
			codeLed7Blink[i] = s_codeLed7[i];			//led k blink thi phai update phong truong hop s_codeLed7 thay doi
														//con led blink thi da update khi isOn, da goi trong ham setCodeBlink
		}
	}

}
