#ifndef _USERCONFIG_H_
#define _USERCONFIG_H_
#include "global.h"
#include "led7seg.h"
#include "colorControl.h"
#include "IR_keyCode.h"

#define FLASH_ADDRESS_USER_CONFIG		ADDR_FLASH_PAGE_31

typedef struct
{
	color_t colorDataLed7[NUM_LED7_RTC];
	uint32_t keyDataIr[NUM_CODE_IR_MAX];

} user_data_flash_t;


void userConfig_flashConfig();

void user_flashUpdate();

#endif
