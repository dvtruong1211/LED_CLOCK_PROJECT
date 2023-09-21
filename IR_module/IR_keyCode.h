#ifndef _IR_KEYCODE_H_
#define _IR_KEYCODE_H_
#include "global.h"
#include "flash.h"

#define NUM_CODE_IR_MAX 21


typedef enum
{
	IR_KEY_0 = 0,
	IR_KEY_1,
	IR_KEY_2,
	IR_KEY_3,
	IR_KEY_4,
	IR_KEY_5,
	IR_KEY_6,
	IR_KEY_7,
	IR_KEY_8,
	IR_KEY_9,
	IR_KEY_MODE,
	IR_KEY_UP,
	IR_KEY_DOWN,
	IR_KEY_RIGTH,
	IR_KEY_LEFT,
	IR_KEY_OK,
	IR_KEY_POWER,
	IR_KEY_MUTE,
	IR_KEY_SW1224,
	IR_KEY_TEMP,
	IR_KEY_AUTO_COLOR,
	MAX_IR_KEY = NUM_CODE_IR_MAX
} ir_keyCode_id_t;

void IR_keyCodeCallback(uint32_t keyCode);

void IR_keyCode_PlusOrMinusIndex(bool isPlus);

void IR_keyCodeUpOrDownReleaseFromHold();

#endif
