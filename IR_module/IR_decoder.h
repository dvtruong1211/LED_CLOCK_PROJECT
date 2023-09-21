#ifndef _IR_DECODER_H_
#define _IR_DECODER_H_
#include "flash.h"
#include "stdbool.h"


#define NUM_BIT_IR_CODE 32

#define IR_BIT_START_H_OK(t) (t > 4000 && t < 5000)
#define IR_BIT_START_L_OK(t) (t > 8500 && t < 9500)

#define IR_BIT_0_H_OK(t) (t > 400 && t < 700)
#define IR_BIT_0_L_OK(t) (t > 400 && t < 700)
#define IR_BIT_1_H_OK(t) (t > 1500 && t < 1800)
#define IR_BIT_1_L_OK(t) (t > 400 && t < 700)


void printf_decToBin(uint32_t dec);
bool IR_checkBit_start_ok(uint32_t width_H, uint32_t width_L);
bool IR_processRawDataOk(uint32_t* rawDataHigh, uint32_t* rawDataLow, uint32_t* codeIROut);

#endif
