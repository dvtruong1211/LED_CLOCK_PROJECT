#include "display.h"
#include "hc595.h"
#include "led.h"
#include "led7seg.h"
#include "buzzer.h"
#include "tim.h"
#include "ds3231.h"
#include "DateTime.h"
#include "uiControl.h"
#include "IR_keyCode.h"


#ifndef DISABLE_ALL_LOG
#define ENABLE_LOG_DISPLAY
#endif
#ifdef ENABLE_LOG_DISPLAY
#define log_info(format, ... ) {printf("DISPLAY: "); printf(format , ##__VA_ARGS__);}
#else
#define log_info(format, ... )
#endif

void display_ledShowStart()
{
	//led 2 luan phien nhau on off
	led2_offAllLed();
	led7seg_disableAll();
	buzzer_startShow();
	/*show each segment of all led*/
	for(int i = 0; i <24; i++)
	{
		if(i==7 || i== 15 || i==23) continue;
		led2_onOffLeds((i%3)== 0, (i % 3) == 1, (i%3)== 2 );
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
			led2_onOffLeds((index%3)== 0, (index % 3) == 1, (index%3)== 2 );
			led7seg_enableLedIndex(index);
			HAL_Delay(500);
		}
	}
	led7seg_disableAll();
	led2_offAllLed();
}

void display_config()
{
	//ds3231_setTimeDefault();
	//ds3231_switchMode();
	led7seg_setValueLed(12,34,56);
	led7seg_setCodeAllLed(CODE_SUB);
	//colorControl_setColorAllLed7(255,255,255);
//	colorControl_setTypeColorForLed(COLOR_B, LED7_H);
//	colorControl_setTypeColorForLed(COLOR_B, LED7_M);
//	colorControl_setTypeColorForLed(COLOR_G, LED7_S);
	colorControl_setTypeColorForLed(COLOR_RGB, LED2);

	HAL_TIM_Base_Start_IT(&htim3);
	HAL_Delay(3000);
	//led7seg_setBlinkLed(LED7_ALL, 1000);
	led2_blink(500);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

}
void display_processTick()
{
	led_tick();
	led7seg_tick();
	buzzer_tick();

}


/*hien tai co cac van de sau can chu y:
 * khi muon blink 2 led SEC1 SEC2 cung nhau thi ngoai cung chu ki thi phai goi ngay sat nhau
 * khi blink LED7_ALL thi tam thoi cac led khac van blink theo chu ki truoc do
 * khi LED7_ALL dang blink thi cac led khac neu co set blink thi bi bo qua
 *
 * */
void testBlinkLed7seg()
{
	static uint16_t cnt_1s = 0;
	cnt_1s++;
	if(cnt_1s == 5)
	{
		led7seg_stopBlinkLed(LED7_ALL);
	}
	if(cnt_1s == 10)
	{
		led7seg_setBlinkLed(LED7_SEC1, 500);
		led7seg_setBlinkLed(LED7_SEC2,500);

		//led7seg_setBlinkLed(LED7_HOUR1, 1000);
		//led7seg_setBlinkLed(LED7_HOUR2,1000);
	}
	if(cnt_1s == 70)
	{
		//led7seg_stopBlinkLed(LED7_SEC1);
		led7seg_stopBlinkLed(LED7_SEC2);
		//led7seg_stopBlinkAll();
		//led7seg_setBlinkLed(LED7_ALL, 1000);
	}
	if(cnt_1s == 50)
	{
		//led7seg_setBlinkLed(LED7_SEC2,500);

		//led7seg_setBlinkLed(LED7_HOUR1, 1000);
	}
}

/*hien thi khi vao trang thai cai thoi gian
 * tam thoi chua cai dat giay*/
void display_settingTimeDate(type_dateTime_t typeTimeDate)
{

	ds3231_timeDate_t timedate;
	timedate = ds3231_get_s_timeDate();
	led7seg_stopBlinkAll();
	switch(typeTimeDate)
	{
		case TYPE_HOUR:
		{
			ds3231_stopGetTimeDate();
			/*hien thi led hour va led min, led hour nhap nhay, led2 stop*/
			//off led sec
			led7seg_setValueLed(timedate.time.h, timedate.time.m, timedate.time.s);
			led7seg_setCodeIndex(CODE_OFF, LED7_SEC1);
			led7seg_setCodeIndex(CODE_OFF, LED7_SEC2);
			led7seg_blinkLed(LED7_HOUR,true,300);
			led2_stopBlink();						//can fix cho nay de led stop nhung phai sang

		}
			break;
		case TYPE_MIN:		//co the switch tu day ve min
		{
			/*hien thi led hour va led min, led min nhap nhay, led2 stop*/
			//off led sec
			led7seg_setValueLed(timedate.time.h, timedate.time.m, timedate.time.s);
			led7seg_setCodeIndex(CODE_OFF, LED7_SEC1);
			led7seg_setCodeIndex(CODE_OFF, LED7_SEC2);
			led7seg_blinkLed(LED7_MIN,true,300);
			led2_enable();
			led2_onAllLed();
		}
			break;
			/*hien thi day tai led sec, date tai led hour, month tai led min
			* nhap nhay day, led2 off*/
		case TYPE_DAY:
		{
			led7seg_setValueLed(timedate.date.dt, timedate.date.month, timedate.date.dy);
			if(timedate.date.dy == 1)
			{	//chu nhat
				led7seg_setCodeIndex(CODE_C,LED7_SEC1);
				led7seg_setCodeIndex(CODE_N,LED7_SEC2);
			}
			else led7seg_setCodeIndex(CODE_T,LED7_SEC1);
			if(timedate.date.month < 10)
			{
				led7seg_setCodeIndex(CODE_OFF,LED7_MIN1);
			}
			led7seg_blinkLed(LED7_SEC, true, 300);
			led2_disable();		//off led2
		}
				break;

		case TYPE_MONTH:				//trang thai nay o giua day va date nen hien thi khong thay doi
		{
			led7seg_blinkLed(LED7_MIN, true, 300);
		}
			break;
		case TYPE_DATE:			//co the switch tu year ve date
		{
			led7seg_setValueLed(timedate.date.dt, timedate.date.month, timedate.date.dy);
			if(timedate.date.dy == 1)
			{	//chu nhat
				led7seg_setCodeIndex(CODE_C,LED7_SEC1);
				led7seg_setCodeIndex(CODE_N,LED7_SEC2);
			}
			else led7seg_setCodeIndex(CODE_T,LED7_SEC1);
			if(timedate.date.month < 10)
			{
				led7seg_setCodeIndex(CODE_OFF,LED7_MIN1);
			}
			led7seg_blinkLed(LED7_HOUR, true, 300);
			led2_disable();		//off led2
		}
			break;

		/*hien thi nam o 4 led hour va min, led sec off, led2 off*/
		case TYPE_YEAR:
		{
			led7seg_setValueLed(20, timedate.date.year, 0);
			led7seg_setCodeIndex(CODE_OFF, LED7_SEC1);
			led7seg_setCodeIndex(CODE_OFF, LED7_SEC2);
			led7seg_setBlinkLed(LED7_ALL, 300);
		}
			break;
		case TYPE_SEC:
			break;
	}
}


/*mac dinh bat dau hien thi led hour, sang tat ca cac thanh, cac led con lai off
 * led 2 hien thi mau red cua led hour, bat dau cai mix red, khi nhan ok thi moi chuyen sang green, ok -> blue
 * tuong tu cho cac led min ,sec */
uint8_t indexTypeColorOfLed = 0;
void display_colorLed7OnLed2(led_type_t ledType, uint8_t colorIdx)
{
	color_t color;
	color = colorControl_getColorLed7Index(ledType);
	if(colorIdx == TYPE_COLOR_R) colorControl_setColorLed2(color.r,0 , 0);
	else if(colorIdx == TYPE_COLOR_G) colorControl_setColorLed2(0,color.g,0);
	else if(colorIdx == TYPE_COLOR_B) colorControl_setColorLed2(0,0,color.b);
	log_info("type color %d\r\n",colorIdx);
}
void display_settingColor(led_type_t ledType)
{
	indexTypeColorOfLed = 0;
	if(ledType == LED7_H)
	{
		ds3231_stopGetTimeDate();
		led7seg_stopBlinkAll();
		led7seg_setCodeAllLed(CODE_OFF);
		led7seg_setCodeIndex(CODE_8, LED7_HOUR1);
		led7seg_setCodeIndex(CODE_8, LED7_HOUR2);
		led2_enable(); // fix hien thi mau r/g/b cua led hour
		//led2_onAllLed();
		display_colorLed7OnLed2(LED7_H, indexTypeColorOfLed);
	}
	else if(ledType == LED7_M)
	{
		led7seg_setCodeAllLed(CODE_OFF);
		led7seg_setCodeIndex(CODE_8, LED7_MIN1);
		led7seg_setCodeIndex(CODE_8, LED7_MIN2);
		display_colorLed7OnLed2(LED7_M, indexTypeColorOfLed);
	}
	else if(ledType == LED7_S)
	{
		led7seg_setCodeAllLed(CODE_OFF);
		led7seg_setCodeIndex(CODE_8, LED7_SEC1);
		led7seg_setCodeIndex(CODE_8, LED7_SEC2);
		display_colorLed7OnLed2(LED7_S, indexTypeColorOfLed);
	}
	else
	{
		//tam thoi chua set mau led2
	}
}

void display_temperature()
{
	float temp= 0;
	ds3231_stopGetTimeDate();
	led7seg_setCodeAllLed(CODE_OFF);
	led7seg_setCodeIndex(CODE_o,LED7_MIN1);
	led7seg_setCodeIndex(CODE_C,LED7_MIN2);
	led2_stopBlink();
	led2_disable();
	if(ds3231_readTemperature(&temp) == DS3231_OK)
	{
		led7seg_setValueAtLed(LED7_HOUR, (int)temp);
		log_info("value current %d.%d\r\n", (int)temp,(int)(temp*100)%100);
	}

}
void display_learningIr()
{
	uint8_t indexCodeIr = IR_KEY_0;
	ds3231_stopGetTimeDate();
	led7seg_setCodeAllLed(CODE_OFF);
	led7seg_setValueAtLed(LED7_SEC, indexCodeIr);
	led7seg_setCodeIndex(CODE_OFF,LED7_SEC1);
	colorControl_setTypeColorForLed(COLOR_R, LED2);
	led2_enable();
	led2_blink(300);


}

/*trang thai binh thuong hien thi gio, phut giay, led2 blink*/
void display_normal()
{
	  led7seg_stopBlinkAll();
	  ds3231_startGetTimeDate();
	  led2_enable();
	  colorControl_setColorLed2(128,128,128);
	  led2_blink(500);
}

