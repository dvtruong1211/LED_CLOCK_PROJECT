#include "uiControl.h"
#include "ds3231.h"
#include "timeCheck.h"
#include "led.h"
#include "led7seg.h"
#include "display.h"
#include "DateTime.h"
#include "colorControl.h"
#include "buzzer.h"
#include "userConfig.h"
#include "rf_transfer.h"
#include "IR_keyCode.h"
#ifndef DISABLE_ALL_LOG
#define ENABLE_LOG_UICONTROL
#endif
#ifdef ENABLE_LOG_UICONTROL
#define log_info(format, ... ) {printf("UI_CONTROL: "); printf(format , ##__VA_ARGS__);}
#else
#define log_info(format, ... )
#endif

extern uint32_t g_sys_tick;
uint32_t timeIrStartLearning = 0;
uint32_t timeStartSetting = 0;
ui_state_t  s_uiState = UI_STATE_NORMAL;

//rieng STATE_LEARING_IR thi chi nhan giu BTN_MODE thi moi thuc hien


void uiControl_updateUI()
{
	switch(s_uiState)
	{
		case UI_STATE_NORMAL:
		{
			display_normal();
			log_info("display UI_STATE_NORMAL\r\n");
		}
		break;
		case UI_STATE_SETTING_RTC_HOUR:
		{
			display_settingTimeDate(TYPE_HOUR);
			log_info("display state UI_STATE_SETTING_RTC_HOUR\r\n");
		}
		break;
		case UI_STATE_SETTING_RTC_MIN:
		{
			display_settingTimeDate(TYPE_MIN);
			log_info("display UI_STATE_SETTING_RTC_MIN\r\n");
		}
		break;
		case UI_STATE_SETTING_RTC_DAY:
		{
			display_settingTimeDate(TYPE_DAY);
			log_info("display UI_STATE_SETTING_RTC_DAY\r\n");
		}
		break;
		case UI_STATE_SETTING_RTC_MONTH:
		{
			display_settingTimeDate(TYPE_MONTH);
			log_info("display UI_STATE_SETTING_RTC_MONTH\r\n");
		}
		break;
		case UI_STATE_SETTING_RTC_DATE:
		{
			display_settingTimeDate(TYPE_DATE);
			log_info("display UI_STATE_SETTING_RTC_DATE\r\n");
		}
		break;
		case UI_STATE_SETTING_RTC_YEAR:
		{
			display_settingTimeDate(TYPE_YEAR);
			log_info("display UI_STATE_SETTING_RTC_YEAR\r\n");
		}
		break;
		case UI_STATE_SETTING_COLOR_HOUR:
		{
			display_settingColor(LED7_H);
			log_info("display UI_STATE_SETTING_COLOR_HOUR\r\n");
		}
		break;
		case UI_STATE_SETTING_COLOR_MIN:
		{
			display_settingColor(LED7_M);
			log_info("display UI_STATE_SETTING_COLOR_MIN\r\n");
		}
		break;
		case UI_STATE_SETTING_COLOR_SEC:
		{
			display_settingColor(LED7_S);
			log_info("display UI_STATE_SETTING_COLOR_SEC\r\n");
		}
		break;
		case UI_STATE_TEMPERATURE:
		{
			display_temperature();
			log_info("display UI_STATE_TEMPERATURE\r\n");
		}
		break;
		case UI_STATE_LEARNING_IR:
		{
			display_learningIr();
			log_info("display UI_STATE_LEARNING_IR\r\n");
		}
		break;
		case UI_STATE_POWER:
		{

		}
		break;
		default:
				break;

	}
}

void uiControl_switchToStateUI(ui_state_t uiState)
{
	s_uiState = uiState;
}

ui_state_t uiControl_getStateUI()
{
	return s_uiState;
}
void uiControl_nextOrBackUiStateSettingColor(bool isNext)		//next, back ap dung doi voi remote
{
	buzzer_onInMs(50);
	if(isNext)
	{
		if(s_uiState >= UI_STATE_SETTING_COLOR_HOUR && s_uiState < UI_STATE_SETTING_COLOR_SEC)
		{
			s_uiState++;
			uiControl_updateUI();
		}
	}
	else
	{
		if(s_uiState > UI_STATE_SETTING_COLOR_HOUR && s_uiState <= UI_STATE_SETTING_COLOR_SEC)
		{
			s_uiState--;
			uiControl_updateUI();
		}
	}
}
void uiControl_nextOrBackUiStateSettingRtc(bool isNext)
{
	buzzer_onInMs(50);
	if(isNext)
	{
		if(s_uiState >= UI_STATE_SETTING_RTC_HOUR && s_uiState < UI_STATE_SETTING_RTC_YEAR)
		{
			s_uiState++;
			uiControl_updateUI();
		}
	}
	else
	{
		if(s_uiState > UI_STATE_SETTING_RTC_HOUR && s_uiState <= UI_STATE_SETTING_RTC_YEAR)
		{
			s_uiState--;
			uiControl_updateUI();
		}
	}

}
void uiControl_returnNorMal()
{
	  s_uiState = UI_STATE_NORMAL;
	  led7seg_stopBlinkAll();
	  ds3231_startGetTimeDate();
	  led2_enable();
	  led2_blink(500);
	  log_info("return to state NORMAL\r\n");
}
extern uint8_t indexTypeColorOfLed;
/* khi nhan nut ok thi chuyen sang cai mau tiep theo, het 3 mau thi chuyen sang led tiep theo*/
void uiControl_switchNextTypeColorAtLed7(led_type_t ledType)
{
	indexTypeColorOfLed++;
	if(indexTypeColorOfLed > TYPE_COLOR_B)
	{
		if(s_uiState == UI_STATE_SETTING_COLOR_SEC)
		{
			user_flashUpdate();
			indexTypeColorOfLed = 0;
			uiControl_switchToStateUI(UI_STATE_NORMAL);
			uiControl_updateUI();
			return;
		}
		uiControl_nextOrBackUiStateSettingColor(true);
		return;
	}
	display_colorLed7OnLed2(ledType, indexTypeColorOfLed);
}
void uiControl_BtnModePressHolding3()
{
	if(s_uiState == UI_STATE_NORMAL)
	{
		buzzer_onInMs(300);
		uiControl_switchToStateUI(UI_STATE_LEARNING_IR);
		uiControl_updateUI();
		timeIrStartLearning = g_sys_tick;
		log_info("start learning IR...\r\n");
	}
}
void uiControl_checkTimeLearningIR()	//1s
{
	if(s_uiState == UI_STATE_LEARNING_IR)
	{
		uint32_t timeRemain;
		timeRemain = elapsedTime(g_sys_tick, timeIrStartLearning);
		if( timeRemain > TIME_IN_LEARNING_IR)
		{
			log_info("time out learning IR\r\n");
			uiControl_switchToStateUI(UI_STATE_NORMAL);
			uiControl_updateUI();
		}
		else
		log_info("time learning remain: %d\r\n", (int)(TIME_IN_LEARNING_IR - timeRemain)/1000);
	}
}

void uiControl_checkTimeSetting()
{
	if(s_uiState != UI_STATE_NORMAL && s_uiState != UI_STATE_LEARNING_IR && s_uiState != UI_STATE_POWER)
	{
		uint32_t timeRemain;
		timeRemain = elapsedTime(g_sys_tick, timeStartSetting);
		if( timeRemain > TIME_IN_SETTING)
		{
			log_info("time out setting\r\n");
			uiControl_switchToStateUI(UI_STATE_NORMAL);
			uiControl_updateUI();
		}
		else
		log_info("time setting remain: %d\r\n", (int)(TIME_IN_SETTING - timeRemain)/1000);
	}
}
uint32_t timeStartShowTemp = 0;
void uiControl_keyTemp()
{
	if(s_uiState == UI_STATE_NORMAL)
	{
		buzzer_onInMs(50);
		uiControl_switchToStateUI(UI_STATE_TEMPERATURE);
		uiControl_updateUI();
		timeStartShowTemp = g_sys_tick;
		log_info("start show temperature...\r\n");
	}
}
void uiControl_checkTimeShowTemp()
{
	if(s_uiState == UI_STATE_TEMPERATURE)
		{
			uint32_t timeRemain;
			timeRemain = elapsedTime(g_sys_tick, timeStartShowTemp);
			if( timeRemain > TIME_SHOW_TEMPERATURE)
			{
				log_info("time out show temperature\r\n");
				uiControl_switchToStateUI(UI_STATE_NORMAL);
				uiControl_updateUI();
			}

		}
}
void uiControl_resetTimeIrLearning()
{
	timeIrStartLearning = g_sys_tick;
}
void uiControl_resetTimeSetting()
{
	timeStartSetting = g_sys_tick;
}

void uiControl_btnModePressShort()
{
	buzzer_onInMs(50);
	switch(s_uiState)
	{
		case UI_STATE_NORMAL:
		{
			uiControl_switchToStateUI(UI_STATE_SETTING_RTC_HOUR);
			uiControl_updateUI();
			log_info("uiState: UI_STATE_SETTING_RTC_HOUR\r\n");
		}
		break;
		case UI_STATE_SETTING_RTC_HOUR:
		{
			uiControl_switchToStateUI(UI_STATE_SETTING_COLOR_HOUR);
			uiControl_updateUI();
			log_info("uiState: UI_STATE_SETTING_COLOR_HOUR\r\n");
		}
		break;
		case UI_STATE_POWER:
		{
			buzzer_blink(2, false);
			log_info("power ON\r\n");
			uiControl_switchToStateUI(UI_STATE_NORMAL);
			uiControl_updateUI();						//co the bat bang nut mode, nhung chi tat bang nut power o remote
		}
		break;
		default:
		{
			uiControl_switchToStateUI(UI_STATE_NORMAL);
			uiControl_updateUI();
			log_info("uiState: UI_STATE_NORMAL\r\n");
		}
		break;
	}
}

void uiControl_btnPlusOrMinusPressShort(bool isPlus) //lam tuong tu cho btnMinus, dung con tro ham de gom 2 ham lai 1
{
	if(uiControl_getStateUI() == UI_STATE_POWER)
	{
		log_info("power off\r\n");
		return;
	}
	buzzer_onInMs(50);
	switch(s_uiState)
	{
		case UI_STATE_LEARNING_IR:
			IR_keyCode_PlusOrMinusIndex(isPlus);	//thay doi indexKeyCode
			break;
		case UI_STATE_SETTING_RTC_HOUR:
			dateTime_PlusOrMinus(TYPE_HOUR, isPlus);
			break;
		case UI_STATE_SETTING_RTC_MIN:
			dateTime_PlusOrMinus(TYPE_MIN, isPlus);
			break;
		case UI_STATE_SETTING_RTC_DAY:
			dateTime_PlusOrMinus(TYPE_DAY, isPlus);
			break;
		case UI_STATE_SETTING_RTC_MONTH:
			dateTime_PlusOrMinus(TYPE_MONTH, isPlus);
			break;
		case UI_STATE_SETTING_RTC_DATE:
			dateTime_PlusOrMinus(TYPE_DATE, isPlus);
			break;
		case UI_STATE_SETTING_RTC_YEAR:
			dateTime_PlusOrMinus(TYPE_YEAR, isPlus);
			break;
		case UI_STATE_SETTING_COLOR_HOUR:
			colorControl_PlusOrMinusValueColorLed2ToLed7(isPlus, COLOR_STEP_MIX, indexTypeColorOfLed, LED7_H);
			break;
		case UI_STATE_SETTING_COLOR_MIN:
			colorControl_PlusOrMinusValueColorLed2ToLed7(isPlus, COLOR_STEP_MIX, indexTypeColorOfLed, LED7_M);
			break;
		case UI_STATE_SETTING_COLOR_SEC:
			colorControl_PlusOrMinusValueColorLed2ToLed7(isPlus, COLOR_STEP_MIX, indexTypeColorOfLed, LED7_S);
			break;

		default: break;		//normal thi k lam gi ca
	}

}


void uiControl_btnOkPressShort()
{
	buzzer_onInMs(50);
	switch(s_uiState)
		{
			case UI_STATE_LEARNING_IR:
				break;
			case UI_STATE_NORMAL:
				rf_transferCodeEv1527(2099525);
				break;
			case UI_STATE_SETTING_RTC_HOUR:
				{
					//goi ham luu gia tri hour
					dateTime_saveToDs3231(TYPE_HOUR);
					//chuyen qua setting min
					uiControl_switchToStateUI(UI_STATE_SETTING_RTC_MIN);
					uiControl_updateUI();
				}
				break;
			case UI_STATE_SETTING_RTC_MIN:
			{
				//goi ham luu gia tri hour
				dateTime_saveToDs3231(TYPE_MIN);
				//chuyen qua setting min
				uiControl_switchToStateUI(UI_STATE_SETTING_RTC_DAY);
				uiControl_updateUI();
			}
				break;
			case UI_STATE_SETTING_RTC_DAY:
			{
				//goi ham luu gia tri hour
				dateTime_saveToDs3231(TYPE_DAY);
				//chuyen qua setting min
				uiControl_switchToStateUI(UI_STATE_SETTING_RTC_MONTH);
				uiControl_updateUI();

			}
				break;
			case UI_STATE_SETTING_RTC_MONTH:
			{
				//goi ham luu gia tri hour
				dateTime_saveToDs3231(TYPE_MONTH);
				//chuyen qua setting min
				uiControl_switchToStateUI(UI_STATE_SETTING_RTC_DATE);
				uiControl_updateUI();
			}
				break;
			case UI_STATE_SETTING_RTC_DATE:
			{
				//goi ham luu gia tri hour
				dateTime_saveToDs3231(TYPE_DATE);
				//chuyen qua setting min
				uiControl_switchToStateUI(UI_STATE_SETTING_RTC_YEAR);
				uiControl_updateUI();
			}
				break;
			case UI_STATE_SETTING_RTC_YEAR:
			{
				//goi ham luu gia tri hour
				dateTime_saveToDs3231(TYPE_YEAR);
				//chuyen qua setting min
				uiControl_switchToStateUI(UI_STATE_NORMAL);
				uiControl_updateUI();
			}
				break;
			case UI_STATE_SETTING_COLOR_HOUR:

				uiControl_switchNextTypeColorAtLed7(LED7_H);
				break;
			case UI_STATE_SETTING_COLOR_MIN:

				uiControl_switchNextTypeColorAtLed7(LED7_M);
				break;
			case UI_STATE_SETTING_COLOR_SEC:

				uiControl_switchNextTypeColorAtLed7(LED7_S);
				break;

			default: break;		//normal thi ka lam gi ca
		}
}

bool powerIsOn = true;
uint32_t timeStartKeyPower = 0;
void uiControl_controlPower()
{
	if(s_uiState != UI_STATE_NORMAL && s_uiState != UI_STATE_POWER) return;		//chi co the tat khi dang o normal
	/*tranh truong hop key nhan lien tuc*/
	if(g_sys_tick - timeStartKeyPower  < 1000) return;
	/*nhuoc diem cua cach kiem tra nay so voi viec kiem tra bang 1 task khac (nhu  ham ds3231_switchMode)
		neu ngay lan nhan dau tien gan sat voi timeStartKeyPower thi se k thuc hien. tuy nhien xac suat cuc ki nho
	*/


	powerIsOn = !powerIsOn;
	if(powerIsOn == false)
	{
		uiControl_switchToStateUI(UI_STATE_POWER);
		log_info("power OFF\r\n");
		buzzer_onInMs(200);
		ds3231_stopGetTimeDate();
		led7seg_setCodeAllLed(CODE_OFF);
		led2_blink(1000);
	}
	else
	{

		buzzer_blink(2, false);
		log_info("power ON\r\n");
		uiControl_switchToStateUI(UI_STATE_NORMAL);
		uiControl_updateUI();

	}
	timeStartKeyPower = g_sys_tick;
}

void uiControl_settingDirection(uint8_t keyDigit)
{
	buzzer_onInMs(50);
	switch(s_uiState)
	{
		case UI_STATE_SETTING_RTC_HOUR:
			dateTime_setTimeDateDirectionByDigitalKeyIr(TYPE_HOUR, keyDigit);
			break;
		case UI_STATE_SETTING_RTC_MIN:
			dateTime_setTimeDateDirectionByDigitalKeyIr(TYPE_MIN, keyDigit);
			break;
		case UI_STATE_SETTING_RTC_MONTH:
			dateTime_setTimeDateDirectionByDigitalKeyIr(TYPE_MONTH, keyDigit);
			break;
		case UI_STATE_SETTING_RTC_DATE:
			dateTime_setTimeDateDirectionByDigitalKeyIr(TYPE_DATE, keyDigit);
			break;
		case UI_STATE_SETTING_RTC_YEAR:
			dateTime_setTimeDateDirectionByDigitalKeyIr(TYPE_YEAR, keyDigit);
			break;
		case UI_STATE_NORMAL:
			rf_transferCodeEv1527(2219970 + keyDigit);
			break;
		default: break;
	}
}

void uiControl_restartBlinkLed7()
{
	switch(s_uiState)
	{
		case UI_STATE_SETTING_RTC_HOUR:
		case UI_STATE_SETTING_RTC_DATE:
			led7seg_blinkLed(LED7_HOUR, true, 300);
			break;
		case UI_STATE_SETTING_RTC_MIN:
		case UI_STATE_SETTING_RTC_MONTH:
		case UI_STATE_SETTING_RTC_YEAR:
			led7seg_blinkLed(LED7_MIN, true, 300);
			break;
		case UI_STATE_SETTING_RTC_DAY:
			led7seg_blinkLed(LED7_SEC, true, 300);
			break;
		default: break;
	}
}





