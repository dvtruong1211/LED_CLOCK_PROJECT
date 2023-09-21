#ifndef _HC595_H_
#define _HC595_H_
#include "stdint.h"



void hc595_out3Byte(uint32_t u24Data);
void hc595_outDataRGB(uint8_t red, uint8_t green, uint8_t blue);


#endif
