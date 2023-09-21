#include "IR_keyCode.h"
#include "string.h"
#include "led.h"
#include "buzzer.h"
#include "ds3231.h"
#include "IR_decoder.h"
#include "led7seg.h"
#include "colorControl.h"
#include "userConfig.h"
#include "uiControl.h"

#ifndef DISABLE_ALL_LOG
#define ENABLE_LOG_IR_KEY
#endif
#ifdef ENABLE_LOG_IR_KEY
#define log_info(format, ... ) {printf("IR_KEY: "); printf(format , ##__VA_ARGS__);}
#else
#define log_info(format, ... )
#endif

uint32_t keyCodeIR[NUM_CODE_IR_MAX] = {0};

ir_keyCode_id_t s_keyCodeId = MAX_IR_KEY;
extern bool isHolding;


ir_keyCode_id_t check_IsNewCode(uint32_t newdata)
{
	for(ir_keyCode_id_t id = 0; id< NUM_CODE_IR_MAX; id++)
	{
		if(keyCodeIR[id] == newdata)
		{
			return id;
		}
	}
	return MAX_IR_KEY;
}

void IR_eraseAllDevice()
{

	printf("IR: erase all code ok\r\n");
}

void IR_keyCode_PlusOrMinusIndex(bool isPlus)
{
	uint8_t valueIndex;
	uiControl_resetTimeIrLearning();
	valueIndex = led7seg_getValueAtLed(LED7_SEC);
	valueIndex = (isPlus == true)? valueIndex+1: valueIndex-1;
	if(isPlus== true && valueIndex == NUM_CODE_IR_MAX) valueIndex = IR_KEY_0;
	if(isPlus== false && valueIndex == 0xff) valueIndex = MAX_IR_KEY - 1;
	led7seg_setValueAtLed(LED7_SEC, valueIndex);
	if(valueIndex < 10) 	led7seg_setCodeIndex(CODE_OFF,LED7_SEC1);
	colorControl_setTypeColorForLed(COLOR_R, LED2);
	led2_enable();
	led2_blink(300);
	log_info("value current: %d\r\n", valueIndex);
}

void IR_keyCodeUpOrDownReleaseFromHold()
{
	if(s_keyCodeId == IR_KEY_UP || s_keyCodeId == IR_KEY_DOWN)
	{
		uiControl_restartBlinkLed7();
		s_keyCodeId = MAX_IR_KEY;
	}
}

/*mot so key co chuc nang giong nut nhan thi goi luon ham cua nut nhan*/
void IR_handlekeyCodeOfUser(ir_keyCode_id_t keyId)
{
	s_keyCodeId = keyId;
	log_info("key code user: %d\r\n", keyId);
	uiControl_resetTimeSetting();
	if(keyId <= IR_KEY_9)
	{
		uiControl_settingDirection(keyId);
	}
	else if(keyId == IR_KEY_MODE)
	{
		uiControl_btnModePressShort();
	}
	else if(keyId == IR_KEY_UP || keyId == IR_KEY_DOWN)
	{
		bool isPlus;
		isPlus = (keyId == IR_KEY_UP) ? true: false;
		if(isHolding) led7seg_stopBlinkAll();
		uiControl_btnPlusOrMinusPressShort(isPlus);
	}

	else if(keyId == IR_KEY_RIGTH)
	{
		uiControl_nextOrBackUiStateSettingRtc(true);
		uiControl_nextOrBackUiStateSettingColor(true);
		log_info("next ui\r\n");
	}
	else if(keyId == IR_KEY_LEFT)
	{
		uiControl_nextOrBackUiStateSettingRtc(false);
		uiControl_nextOrBackUiStateSettingColor(false);
		log_info("back ui\r\n");
	}
	else if(keyId == IR_KEY_OK)
	{
		uiControl_btnOkPressShort();
	}
	else if(keyId == IR_KEY_SW1224)
	{
		if(uiControl_getStateUI() == UI_STATE_NORMAL)
		{
			ds3231_switchMode();
			log_info("rtc switch mode\r\n");
		}
	}
	else if(keyId == IR_KEY_POWER)
	{
		uiControl_controlPower();
	}
	else if(keyId == IR_KEY_MUTE)
	{
		buzzer_switchEnableOrDisable();
	}
	else if(keyId == IR_KEY_TEMP)
	{
		uiControl_keyTemp();
	}
	else if(keyId == IR_KEY_AUTO_COLOR)
	{
		colorControl_autoFade();
	}
}


void IR_keyCodeCallback(uint32_t keyCode)
{
	if(uiControl_getStateUI() == UI_STATE_LEARNING_IR)
	{
		uint8_t valueIndex;
		valueIndex = led7seg_getValueAtLed(LED7_SEC);
		keyCodeIR[valueIndex] = keyCode;
		user_flashUpdate();
		buzzer_onInMs(50);
		colorControl_setTypeColorForLed(COLOR_G, LED2);
		led2_stopBlink();
		log_info("learning IR keCode success %d\r\n",(int)keyCode);
	}

	else
	{
		ir_keyCode_id_t keyid;
		keyid = check_IsNewCode(keyCode);
		if(keyid == MAX_IR_KEY) return;
		else
		{
			if(uiControl_getStateUI() == UI_STATE_POWER && keyid != IR_KEY_POWER)
			{
				log_info("power off\r\n");
				return;
			}
			IR_handlekeyCodeOfUser(keyid);
		}
	}
}








