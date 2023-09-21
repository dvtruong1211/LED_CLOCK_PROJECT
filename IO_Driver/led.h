#ifndef _LED_H_
#define _LED_H_
#include "stdint.h"
#include "main.h"
#include "gpio.h"
#include "stdbool.h"
#define NUM_LED_MAX 3	//khong tinh led MCU
#define NUM_LED2 3		//so led dau 2 cham gom 3 led red, green, blue

#define LED2_PORT GPIOB
#define LED2_ON(gpio_pin)  HAL_GPIO_WritePin(LED2_PORT, gpio_pin, GPIO_PIN_SET)
#define LED2_OFF(gpio_pin) HAL_GPIO_WritePin(LED2_PORT, gpio_pin, GPIO_PIN_RESET)
#define LED2_TGL(gpio_pin) HAL_GPIO_TogglePin(LED2_PORT,gpio_pin);

#define LED2_ON_ALL 		HAL_GPIO_WritePin(LED2_PORT, LED2_RED_Pin|LED2_GRE_Pin|LED2_BLU_Pin, GPIO_PIN_SET)
#define LED2_OFF_ALL 		HAL_GPIO_WritePin(LED2_PORT, LED2_RED_Pin|LED2_GRE_Pin|LED2_BLU_Pin, GPIO_PIN_RESET)


typedef enum
{
	 LED2_RED_ID = 0,
	 LED2_GREEN_ID,
	 LED2_BLUE_ID,
} led2_id_t;

void led_toggleStatusMcu(uint16_t nTickToggle);
void led2_startShow(void);
void led2_onOffLeds(bool isOnRed, bool isOnGreen, bool isOnBlue);
void led2_offAllLed();
void led2_onAllLed();
void led2_enable();
void led2_disable();
void led2_blink(uint16_t period);
void led2_stopBlink();
void led2_colorScan(uint8_t bitpos);
void led_tick();		//10ms




#endif

