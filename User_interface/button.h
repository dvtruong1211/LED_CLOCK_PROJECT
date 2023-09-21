#ifndef _BUTTON_H_
#define _BUTTON_H_
#include "global.h"


#define NUM_BUTTON 4
#define TIME_HOLDING_1 1500		//ms
#define TIME_HOLDING_2 3000		//ms
#define TIME_HOLDING_3 5000		//ms


typedef enum
{
	BTN_ID_MODE =0,
	BTN_ID_PLUS,
	BTN_ID_MINUS,
	BTN_ID_OK,
	MAX_BTN_ID = NUM_BUTTON
} btn_id_t;


typedef enum
{
	BTN_STATE_RELEASE = 0,
	BTN_STATE_PRESS,
	BTN_STATE_HOLD1,
	BTN_STATE_HOLD2,
	BTN_STATE_HOLD3,
} btn_state_t;


typedef struct
{
	btn_id_t id_btn;
	btn_state_t state_btn;
	uint32_t timeStartPress;			//thoi diem nhan
	uint32_t timePress;
}	btn_profile_t;


typedef enum button_event
{
    BTN_EVENT_RELEASED ,        ///< Button is in the released state
    BTN_EVENT_PRESSED ,         ///< Button is in the pressed state
    BTN_EVENT_HOLD ,            ///< Button released within valid timing
    BTN_EVENT_MULTI_TOUCH,     ///< More than one touch sensor is being touched
    BTN_EVENT_REQUEST_DELAY,   ///< Lower level asking for a delay
} btn_event_t;                     ///< Refer @ref e_touch_button_event


typedef void(*btn_cb)(btn_id_t, uint32_t);
typedef struct
{
	btn_event_t event_btn;
	btn_cb 			event_btn_cb;
} event_process_t;

void btn_processEvent(void);	//10ms		//1ms ->10ms
void btn_processState(void);	//100ms		//do nut nhan cham nen k can xu li nhanh (timepress sai so 1 chut)

void btn_holdPlusOrMinusFast();



#endif
