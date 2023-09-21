#ifndef _LED7SEG_H_
#define _LED7SEG_H_
#include "stdint.h"
#include "stdbool.h"

#define NUM_LED7_RTC	6
#define LEVEL_ACTIVE_7SEG 0
#define NUM_CODE_7SEG 21
#define NUM_CODE_7SEG_CHAR 8

#define BIT(x)		(1<<x)
#define TIME_MIX_1BIT_COLOR	  7// us
#define TIME_SCAN_1LED7   TIME_MIX_1BIT_COLOR * (BIT(RESOLUTION_BIT)-1)	//7* 255us


typedef enum
{
	CODE_0 = 0,
	CODE_1,
	CODE_2,
	CODE_3,
	CODE_4,
	CODE_5,
	CODE_6,
	CODE_7,
	CODE_8,
	CODE_9,
	CODE_A,
	CODE_B,
	CODE_C,
	CODE_D,
	CODE_E,
	CODE_F,
	CODE_OFF,		//tat  het led
	CODE_SUB,			// dau '-'
	CODE_N,
	CODE_T,
	CODE_o

} code7seg_id_t;

typedef enum
{
	LED7_HOUR1 = 0,
	LED7_HOUR2,
	LED7_MIN1,
	LED7_MIN2,
	LED7_SEC1,
	LED7_SEC2,
	LED7_ALL
} led7_id_t;

typedef enum
{
	LED7_HOUR = 0,
	LED7_MIN,
	LED7_SEC
} led_couple_7seg_id_t;
typedef struct
{
	uint8_t code_r;
	uint8_t code_g;
	uint8_t code_b;
} codeColor_t;

typedef struct
{
	bool isOn;
	bool isBlink;
	uint32_t periodBlink;
	uint32_t timeStartBlink;
} led7seg_blink_t;

void led7seg_enableLedIndex(uint8_t index);
void led7seg_disableAll();
void led7seg_showStart();
void led7seg_setCodeIndex(code7seg_id_t code_id, uint8_t indexLed7);
void led7seg_setCodeAllLed(code7seg_id_t code);
void led7seg_setValueLed(uint8_t valueHour, uint8_t valueMin, uint8_t valueSec);
void led7seg_encoder(uint8_t valueHour, uint8_t valueMin, uint8_t valueSec);
void led7seg_setValueAtLed(led_couple_7seg_id_t led, uint8_t value);
uint8_t led7seg_getValueAtLed(led_couple_7seg_id_t led);


void led7seg_setBlinkLed(led7_id_t ledId, uint32_t period);
void led7seg_blinkLed(led_couple_7seg_id_t ledType, bool isBlink, uint32_t period);
void led7seg_stopBlinkLed(led7_id_t ledId);
void led7seg_stopBlinkAll();

void led7seg_tick();

#endif
