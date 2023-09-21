#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include "DateTime.h"
#include "colorControl.h"
void display_config();

void display_ledShowStart();
void display_processTick();
void testBlinkLed7seg();

void display_normal();

void display_settingTimeDate(type_dateTime_t typeTimeDate);

void display_settingColor(led_type_t ledType);
void display_colorLed7OnLed2(led_type_t ledType, uint8_t colorIdx);
void display_temperature();

void display_learningIr();

#endif
