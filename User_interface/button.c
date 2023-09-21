#include "button.h"
#include "main.h"
#include "gpio.h"
#include "timeCheck.h"
#include "buzzer.h"
#include "led.h"
#include "uiControl.h"
#include "led7seg.h"

#ifndef DISABLE_ALL_LOG
#define ENABLE_LOG_BUTTON
#endif
#ifdef ENABLE_LOG_BUTTON
#define log_info(format, ... ) {printf("BUTTON: "); printf(format , ##__VA_ARGS__);}
#else
#define log_info(format, ... )
#endif





#define 	BTN_GPIO_PORT GPIOB			//tat ca ca pin BTN deu la portB
#define 	PIN_BTN_IS_PRESSED(state_pin) (state_pin == 0)
#define 	TIME_NOISE 30		//ms
#define 	NUM_EVENT 2

extern uint32_t g_sys_tick;
static uint8_t pin_statePre[NUM_BUTTON] = {1,1,1,1}; //gio_level
static uint32_t timeStart[NUM_BUTTON] = {0,0,0,0};

void btn_Pressed_cb(btn_id_t btn, uint32_t timeStartPress);
void btn_Released_cb(btn_id_t btn, uint32_t timeStartRelease);

event_process_t event_arg[2] ={
	{BTN_EVENT_RELEASED, btn_Released_cb},
	{BTN_EVENT_PRESSED,	btn_Pressed_cb}
};


btn_profile_t btnProfile[NUM_BUTTON] = {
	 {BTN_ID_MODE, BTN_STATE_RELEASE, 0,0},
	 {BTN_ID_PLUS, BTN_STATE_RELEASE, 0,0},
	 {BTN_ID_MINUS,BTN_STATE_RELEASE, 0,0},
	 {BTN_ID_OK,   BTN_STATE_RELEASE, 0,0}
};

GPIO_PinState btn_findBtnPinSate(uint8_t btn)
{
	uint16_t gpio_pin = 0;
	switch(btn)
	{
		case BTN_ID_MODE:
			gpio_pin = BTN_MODE_Pin;
			break;
		case BTN_ID_PLUS:
			gpio_pin = BTN_PLUS_Pin;
			break;
		case BTN_ID_MINUS:
			gpio_pin = BTN_MINUS_Pin;
			break;
		case BTN_ID_OK:
			gpio_pin = BTN_OK_Pin;
			break;
	}
	return HAL_GPIO_ReadPin(BTN_GPIO_PORT, gpio_pin);
}


/*check pin button la thay doi tu 1->0 hay tu 0->1
	truong hop press thi kiem tra co nhieu khong*/
void btn_processEvent()		//1ms
{
	static bool enable_cnt_noise[NUM_BUTTON] = {false, false, false, false};
	for(uint8_t btnIndex = 0; btnIndex < MAX_BTN_ID; btnIndex++)
	{
		uint8_t pinStateCur = (btn_findBtnPinSate(btnIndex)== GPIO_PIN_SET)? 1: 0;

		/*chong nhieu*/
		if(PIN_BTN_IS_PRESSED(pinStateCur) && !enable_cnt_noise[btnIndex])
		{
			timeStart[btnIndex] = g_sys_tick;
			enable_cnt_noise[btnIndex] = true;

		}
		/*event pressed*/
		if(enable_cnt_noise[btnIndex] && (elapsedTime(g_sys_tick, timeStart[btnIndex]) > TIME_NOISE))
		{
			enable_cnt_noise[btnIndex] = false;

			if(PIN_BTN_IS_PRESSED(pinStateCur))					// =0-> dung la nhan
			{
				if(pin_statePre[btnIndex] == 1)					//truoc do dang  = 1
				{
					pin_statePre[btnIndex] = 0;
					event_arg[BTN_EVENT_PRESSED].event_btn_cb((btn_id_t)btnIndex, timeStart[btnIndex]); //ham cb xu li event pressed
					//log_info("btn_id: %d, state pin: %d\r\n", btnIndex, pinStateCur);
				}
			}
		}
		/*event released*/
		if(!PIN_BTN_IS_PRESSED(pinStateCur))
		{
			if(pin_statePre[btnIndex] == 0)					//truoc do dang  = 0
			{
				pin_statePre[btnIndex] = 1;
				event_arg[BTN_EVENT_RELEASED].event_btn_cb((btn_id_t)btnIndex, g_sys_tick); //ham cb xu li event released
				//log_info("btn_id: %d, state pin: %d\r\n", btnIndex, pinStateCur);
			}
		}
	}
}
/*ngay khi phat hien cac thay doi o pin button thi thay doi trang thai cho button*/
void btn_Pressed_cb(btn_id_t btn, uint32_t timeStartPress)
{
	btnProfile[btn].state_btn = BTN_STATE_PRESS;
	btnProfile[btn].timeStartPress = timeStartPress;
	printf("\r\n");
	log_info("EVENT_PRESS: timeStartPress: %d, BTN_ID: %d\r\n", (int)timeStartPress, btn);
}
void btn_Released_cb(btn_id_t btn, uint32_t timeStartRelease)
{
	btnProfile[btn].state_btn = BTN_STATE_RELEASE;
	btnProfile[btn].timePress = elapsedTime(timeStartRelease, btnProfile[btn].timeStartPress);	//de xac dinh xu li phim khi release
	log_info("EVENT_RELEASE: timePress: %d\r\n", (int)btnProfile[btn].timePress);
}

/*trong ham nay  kiem tra xem phim nao duoc nhan giu trong bao lau
	thuong di kem voi kiem tra trang thai chung cua ca he thong
*/
void btn_Hold_cb(btn_id_t btn, uint32_t timeHold)
{
	if(btn == BTN_ID_MODE)
	{
		if(timeHold == TIME_HOLDING_3) uiControl_BtnModePressHolding3();
	}
	else if(btn == BTN_ID_PLUS)
	{

	}
	else if(btn == BTN_ID_MINUS)
	{

	}
	else if(btn == BTN_ID_OK)
	{

	}
}
/*thuc hien ham khi nha phim*/
void btn_shortPress_cb(btn_id_t btn)			//xu li khi phim duoc nha
{
	uiControl_resetTimeSetting();
	if(uiControl_getStateUI() == UI_STATE_POWER && btn != BTN_ID_MODE)
	{
		log_info("power off\r\n");
		return;
	}
	if(btnProfile[btn].timePress <= TIME_HOLDING_1)		//k phai truong hop nhan giu,
	{
		if(btn == BTN_ID_MODE)
		{
			uiControl_btnModePressShort();
		}
		else if(btn == BTN_ID_PLUS)
		{
			uiControl_btnPlusOrMinusPressShort(true);
		}
		else if(btn == BTN_ID_MINUS)
		{
			uiControl_btnPlusOrMinusPressShort(false);
		}
		else if(btn == BTN_ID_OK)
		{
			uiControl_btnOkPressShort();
		}
	}
	else
	{
		if(btnProfile[btn].timePress > TIME_HOLDING_1)		//truong hop nha sau khi nhan giu cua phim Plus va Minus
		{
			if(btn == BTN_ID_PLUS || btn == BTN_ID_MINUS)
			{
				uiControl_restartBlinkLed7();
			}
		}


	}
}
/* kiem tra cac trang thai va thuc hien ham tuong ung*/
void btn_processState(void)	//10ms
{
	for(uint8_t btnIndex = 0; btnIndex < NUM_BUTTON; btnIndex++)
	{

		switch(btnProfile[btnIndex].state_btn)
		{
			case BTN_STATE_RELEASE:
				/*vi day la trang thai thuong co cua button
					ham xu li phim can thuc hien sau khi release va thuc hien 1 lan*/
				if(btnProfile[btnIndex].timePress > 0) 		//chung to co su kien nha (vua duoc nhan truoc do)
				{
					btn_shortPress_cb((btn_id_t)btnIndex);
					btnProfile[btnIndex].timePress = 0;

				}
				break;
			case BTN_STATE_PRESS:
				if(elapsedTime(g_sys_tick, btnProfile[btnIndex].timeStartPress) > TIME_HOLDING_1)
				{
					log_info("time holding 1: %d\r\n",(int)elapsedTime(g_sys_tick, btnProfile[btnIndex].timeStartPress));
					btnProfile[btnIndex].state_btn = BTN_STATE_HOLD1;
					btn_Hold_cb((btn_id_t)btnIndex, TIME_HOLDING_1);
				}
				break;
			case BTN_STATE_HOLD1:
				if(elapsedTime(g_sys_tick, btnProfile[btnIndex].timeStartPress) > TIME_HOLDING_2)
				{
					log_info("time holding 2: %d\r\n",(int)elapsedTime(g_sys_tick, btnProfile[btnIndex].timeStartPress));
					btnProfile[btnIndex].state_btn = BTN_STATE_HOLD2;
					btn_Hold_cb((btn_id_t)btnIndex, TIME_HOLDING_2);
				}
				break;
			case BTN_STATE_HOLD2:
				if(elapsedTime(g_sys_tick, btnProfile[btnIndex].timeStartPress) > TIME_HOLDING_3)
				{
					log_info("time holding 3: %d\r\n",(int)elapsedTime(g_sys_tick, btnProfile[btnIndex].timeStartPress));
					btnProfile[btnIndex].state_btn = BTN_STATE_HOLD3;
					btn_Hold_cb((btn_id_t)btnIndex, TIME_HOLDING_3);
				}
					break;
			case BTN_STATE_HOLD3:
				break;
		}
	}

}


