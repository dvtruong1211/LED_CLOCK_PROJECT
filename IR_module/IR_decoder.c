#include "IR_decoder.h"
#include "stdio.h"
#include "stdbool.h"



void printf_decToBin(uint32_t dec)
{
	for (int c = 31; c >= 0; c--)
	  {
		uint32_t k = dec >> c;

		if (k & 1)
		  printf("1 ");
		else
		  printf("0 ");
		//	if((c == 24) || (c == 16) || (c== 8)) printf(" ");
	  }
	printf("\r\n");
}



bool IR_checkBit_start_ok(uint32_t width_H, uint32_t width_L)
{
		if(IR_BIT_START_H_OK(width_H) && IR_BIT_START_L_OK(width_L))
			return 1;
		else return 0;
}

bool IR_checkBit_1_ok(uint32_t width_H, uint32_t width_L)
{
		if(IR_BIT_1_H_OK(width_H) && IR_BIT_1_L_OK(width_L))
			return 1;
		else return 0;
}

bool IR_checkBit_0_ok(uint32_t width_H, uint32_t width_L)
{
		if(IR_BIT_0_H_OK(width_H) && IR_BIT_0_L_OK(width_L))
			return 1;
		else return 0;
}



/*mang du lieu gom 40 phan tu, 1bit dau tien bo qua, bit 1 la bit start, 32 bit tiep la data, 1 bit stop*/
bool IR_processRawDataOk(uint32_t* rawDataHigh, uint32_t* rawDataLow, uint32_t* codeIROut)
{
	uint32_t u32bit = 0;
	for(int i = 0; i < NUM_BIT_IR_CODE; i++)
	{
		u32bit = u32bit << 1;
		if(IR_checkBit_1_ok(rawDataHigh[i], rawDataLow[i]))
		{
			u32bit = u32bit | 0x01;
		}
		else  if(IR_checkBit_0_ok(rawDataHigh[i], rawDataLow[i]))
		{

		}
		else
		{
			printf("IR: data bad\r\n");
			return 0;
		}
	}

	printf("IR: u32data: %d, %X\r\n", (int)u32bit,(int)u32bit);
	//printf_decToBin(u32bit);
	*codeIROut = u32bit;

	return true;

}




