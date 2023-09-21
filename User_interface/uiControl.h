#ifndef _UICONTROL_H_
#define _UICONTROL_H_
#include "global.h"


#define TIME_IN_LEARNING_IR 60000	//1 phut
#define TIME_IN_SETTING 20000	//1 phut

#define TIME_SHOW_TEMPERATURE 5000	//3s
#define NUM_UI_STATE 13
typedef enum
{
	UI_STATE_NORMAL = 0,
	UI_STATE_SETTING_RTC_HOUR,
	UI_STATE_SETTING_RTC_MIN,
	UI_STATE_SETTING_RTC_DAY,
	UI_STATE_SETTING_RTC_MONTH,
	UI_STATE_SETTING_RTC_DATE,
	UI_STATE_SETTING_RTC_YEAR,
	UI_STATE_SETTING_COLOR_HOUR,
	UI_STATE_SETTING_COLOR_MIN,
	UI_STATE_SETTING_COLOR_SEC,

	UI_STATE_TEMPERATURE,
	UI_STATE_LEARNING_IR,
	UI_STATE_POWER,
	UI_STATE_MAX

} ui_state_t;



void uiControl_BtnModePressHolding3();
void uiControl_btnModePressShort();
void uiControl_checkTimeLearningIR();	//1s
void uiControl_checkTimeSetting();

void uiControl_resetTimeIrLearning();
void uiControl_resetTimeSetting();


void uiControl_keyTemp();
void uiControl_checkTimeShowTemp();

void uiControl_nextOrBackUiStateSettingColor(bool isNext);		//next, back ap dung doi voi remote
void uiControl_nextOrBackUiStateSettingRtc(bool isNext);

void uiControl_btnPlusOrMinusPressShort(bool isPlus); //lam tuong tu cho btnMinus, dung con tro ham de gom 2 ham lai 1
void uiControl_btnOkPressShort();

void uiControl_controlPower();
void uiControl_settingDirection(uint8_t keyDigit);
void uiControl_restartBlinkLed7();

ui_state_t uiControl_getStateUI();

#endif
