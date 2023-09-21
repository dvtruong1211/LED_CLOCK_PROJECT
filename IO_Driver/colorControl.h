#ifndef _COLORCONTROL_H_
#define _COLORCONTROL_H_
#include "global.h"

#define RESOLUTION_BIT 8	//do phan giai mau 8 bit, moi mau co gia tri uint8
#define NUM_COLOR 7
#define COLOR_STEP_MIX 15
typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;

} color_t;


typedef enum
{
	COLOR_R = 0b100,
	COLOR_G = 0b010,
	COLOR_B = 0b001,
	COLOR_RG= 0b110,
	COLOR_RB= 0b101,
	COLOR_GB = 0b011,
	COLOR_RGB = 0b111
} color_type_t;

enum
{
	TYPE_COLOR_R =0,
	TYPE_COLOR_G =1,
	TYPE_COLOR_B = 2
};
typedef enum
{
	LED7_H,
	LED7_M,
	LED7_S,
	LED2
} led_type_t;
void colorControl_setColorAllLed7(uint8_t red, uint8_t green, uint8_t blue);
void colorControl_setColorLed2(uint8_t red, uint8_t green, uint8_t blue);

void colorControl_setTypeColorForLed(color_type_t type, led_type_t ledId);
void colorControl_PlusOrMinusTypeColorLed(led_type_t ledId, bool isPlus);
void colorControl_PlusOrMinusValueColorLed2ToLed7(bool isPlus, uint8_t step, uint8_t typeColor,led_type_t ledType);

color_t colorControl_getColorLed7Index(led_type_t ledtype);

void colorControl_mixColor();
void colorControl_autoFade();
void colorControl_fadeLed();		//10ms


#endif
