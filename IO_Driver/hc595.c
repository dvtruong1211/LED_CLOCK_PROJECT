#include "hc595.h"
#include "gpio.h"


#define HC595_CLK

void hc595_clk()
{
	HAL_GPIO_WritePin(SHCP_595_GPIO_Port, SHCP_595_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(SHCP_595_GPIO_Port, SHCP_595_Pin, GPIO_PIN_SET);

}

void hc595_latch()
{
	HAL_GPIO_WritePin(STCP_595_GPIO_Port, STCP_595_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(STCP_595_GPIO_Port, STCP_595_Pin, GPIO_PIN_SET);
}
//dich byte cao truoc
void hc595_outByte(uint8_t u8data)
{
	for(int i = 0; i< 8; i++)
	{
		HAL_GPIO_WritePin(DS595_GPIO_Port, DS595_Pin, ((u8data & (0x80>>i)) != 0)? GPIO_PIN_SET:GPIO_PIN_RESET);
		hc595_clk();
	}
	hc595_latch();
}

void hc595_out3Byte(uint32_t u24Data)
{
	uint32_t tem;
	tem = u24Data;
	for(int i =0; i < 24; i++)
	{
		HAL_GPIO_WritePin(DS595_GPIO_Port, DS595_Pin, ((tem & 0x00800000) != 0)? GPIO_PIN_SET:GPIO_PIN_RESET);
		tem = tem<<1;
		hc595_clk();
	}
	hc595_latch();
}

void hc595_outDataRGB(uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t bit_index = 0x80;
	for(int i = 0; i< 8; i++)
	{
		HAL_GPIO_WritePin(DS595_GPIO_Port, DS595_Pin, blue & bit_index);
		hc595_clk();
		bit_index = bit_index >>1;
	}
	bit_index = 0x80;
	for(int i = 0; i< 8; i++)
	{
		HAL_GPIO_WritePin(DS595_GPIO_Port, DS595_Pin, green & bit_index);
		hc595_clk();
		bit_index = bit_index >>1;
	}
	bit_index = 0x80;
	for(int i = 0; i< 8; i++)
	{
		HAL_GPIO_WritePin(DS595_GPIO_Port, DS595_Pin, red & bit_index);
		hc595_clk();
		bit_index = bit_index >>1;
	}
	hc595_latch();
}
