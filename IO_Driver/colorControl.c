#include "colorControl.h"
#include "time.h"
#include "led7seg.h"

#ifndef DISABLE_ALL_LOG
#define ENABLE_LOG_COLORCTRL
#endif
#ifdef ENABLE_LOG_COLORCTRL
#define log_info(format, ... ) {printf("COLOR_CONTROL: "); printf(format , ##__VA_ARGS__);}
#else
#define log_info(format, ... )
#endif

color_type_t typeColor[NUM_COLOR] = {COLOR_R,COLOR_G,COLOR_B ,COLOR_RG,COLOR_RB,COLOR_GB,COLOR_RGB};

color_t colorValueLed7[NUM_LED7_RTC] = {{255,0,0},{255,0,0},{255,0,0},{255,0,0},{255,0,0},{255,0,0}};	//luu gia tri mau ung voi tung led7seg
color_t colorValueLed2 = {0,0,0};

uint8_t indexTypeColor[4] = {1,1,1,1};
void colorControl_setColorAllLed7(uint8_t red, uint8_t green, uint8_t blue)
{
	for(int i = 0 ; i< NUM_LED7_RTC; i++)
	{
		colorValueLed7[i].r = red;
		colorValueLed7[i].g = green;
		colorValueLed7[i].b = blue;
	}
}

void colorControl_setColorLed7Index(uint8_t red, uint8_t green, uint8_t blue, led7_id_t ledIndex)
{
	colorValueLed7[ledIndex].r = red;
	colorValueLed7[ledIndex].g = green;
	colorValueLed7[ledIndex].b = blue;
}
void colorControl_setColorLed2(uint8_t red, uint8_t green, uint8_t blue)
{
	colorValueLed2.r = red;
	colorValueLed2.g = green;
	colorValueLed2.b = blue;
}

color_t colorControl_getColorLed7Index(led_type_t ledtype)
{
	uint8_t index =0;
	index = 2* ledtype;			//coi cac cap led mau giong nhau 0,2,4
	return colorValueLed7[index];
}

color_t colorControl_convertTypeToValue(color_type_t type)
{
	color_t color;
	color.r = (type & 0b100) ? 255: 0;
	color.g = (type & 0b010) ? 255: 0;
	color.b = (type & 0b001) ? 255: 0;
	return color;
}

void colorControl_setTypeColorForLed(color_type_t type, led_type_t ledId)
{
	color_t color;
	color = colorControl_convertTypeToValue(type);
	if(ledId == LED7_H)
	{
		colorControl_setColorLed7Index(color.r, color.g, color.b, LED7_HOUR1);
		colorControl_setColorLed7Index(color.r, color.g, color.b, LED7_HOUR2);
	}
	else if(ledId == LED7_M)
	{
		colorControl_setColorLed7Index(color.r, color.g, color.b, LED7_MIN1);
		colorControl_setColorLed7Index(color.r, color.g, color.b, LED7_MIN2);
	}
	else if(ledId == LED7_S)
	{
		colorControl_setColorLed7Index(color.r, color.g, color.b, LED7_SEC1);
		colorControl_setColorLed7Index(color.r, color.g, color.b, LED7_SEC2);
	}
	else 	colorControl_setColorLed2(color.r, color.g, color.b);


}


void colorControl_PlusOrMinusTypeColorLed(led_type_t ledId, bool isPlus)
{
	indexTypeColor[ledId] = (isPlus == true) ? indexTypeColor[ledId]+1 :indexTypeColor[ledId]-1;
	if(indexTypeColor[ledId] == 0xff && !isPlus) indexTypeColor[ledId] = 6;
	if(indexTypeColor[ledId] == 7 && isPlus) indexTypeColor[ledId] = 0;
   colorControl_setTypeColorForLed(typeColor[indexTypeColor[ledId]], ledId);
}

/*lay gia tri color led2 hien tai de plus or minus sau do gan cho led7
 * typeColor la 0,1,2: ung voi mau r,g,b,
 * 2*ledType va 2*ledType+1 ra 2 ledcouple*/
void colorControl_PlusOrMinusValueColorLed2ToLed7(bool isPlus, uint8_t step, uint8_t typeColor,led_type_t ledType)
{
	uint8_t valueColor;
	uint8_t *pValue;
	pValue = (uint8_t*)&colorValueLed2;
	valueColor = *(pValue+typeColor);
	valueColor = (isPlus == true) ? (valueColor +step) : (valueColor - step);
	valueColor = valueColor - valueColor%COLOR_STEP_MIX;

	*(pValue+typeColor) = valueColor;
	pValue = (uint8_t*)&colorValueLed7[2*ledType];
	*(pValue+typeColor) = valueColor;
	pValue = (uint8_t*)&colorValueLed7[2*ledType+1];
	*(pValue+typeColor) = valueColor;

	log_info("value current: %d\r\n",valueColor);

}
void colorControl_mixColor()
{
	static uint8_t r,g,b, index;
	r +=50; g+=100; b+=200; index++;
	if(index == NUM_LED7_RTC) index = 0;
	colorControl_setColorLed7Index(r, g, b,index);

}

void colorControl_effectChangeSingColor()
{

}


/*auto mix color
 * fade led  doi voi mau trong tung led:
 * 	+ red: 255-> 0, green: 0->255, blue: 0			//case 0
 * 	+ red: 0, 		green: 255->0, blue: 0->255		//case 1
 * 	+ red: 0->255,  green: 0,      blue: 255->0		//case 2
 * */

uint8_t fadeIndex = 0;
uint8_t stepFade = 0;
color_t colorFadeValue = {255,0,0};
uint8_t fadeUp = 0, fadeDown = 255;
void colorControl_fadeLed()		//10ms
{
	if(fadeIndex)
	{
		if(fadeUp < 255) fadeUp += 1;
		if(fadeDown > 0) fadeDown -= 1;
		if(fadeDown == 0  && fadeUp == 255)
		{
			fadeUp = 0; fadeDown = 255;
			stepFade++;
			if(stepFade == 3) stepFade = 0;

		}
		switch(stepFade)
		{
			case 0:
				colorFadeValue.r = fadeDown;
				colorFadeValue.g = fadeUp;
				colorFadeValue.b = 0;
				break;
			case 1:
				colorFadeValue.r = 0;
				colorFadeValue.g = fadeDown;
				colorFadeValue.b = fadeUp;
				break;
			case 2:
				colorFadeValue.r = fadeUp;
				colorFadeValue.g = 0;
				colorFadeValue.b = fadeDown;
				break;
		}

		if(fadeIndex == 1)
			colorControl_setColorAllLed7(colorFadeValue.r,colorFadeValue.g, colorFadeValue.b);
		else if(fadeIndex == 2)
		{
			colorControl_setColorLed7Index(colorFadeValue.r,colorFadeValue.g, colorFadeValue.b, LED7_HOUR1);
			colorControl_setColorLed7Index(colorFadeValue.r,colorFadeValue.g, colorFadeValue.b, LED7_HOUR2);
			colorControl_setColorLed7Index(colorFadeValue.g,colorFadeValue.b, colorFadeValue.r, LED7_MIN1);
			colorControl_setColorLed7Index(colorFadeValue.g,colorFadeValue.b, colorFadeValue.r, LED7_MIN2);
			colorControl_setColorLed7Index(colorFadeValue.b,colorFadeValue.r, colorFadeValue.g, LED7_SEC1);
			colorControl_setColorLed7Index(colorFadeValue.b,colorFadeValue.r, colorFadeValue.g, LED7_SEC2);
		}

	}
}


void colorControl_autoFade()
{
	fadeIndex++;
	if(fadeIndex == 3)
	{
		fadeIndex = 0;
	}
}















