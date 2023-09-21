#ifndef _RF_TRANSFER_H_
#define _RF_TRANSFER_H_

#include "Global.h"


typedef enum
{
    RF_EV1527 = 0,
	RF_PT2262
} rf_typeDevice_t;


typedef struct
{
    uint32_t rfCode;
    rf_typeDevice_t typeDevice;
    uint8_t timesTransfer;          //num of frame

}  rf_infoTransfer_t;

void rf_transfer_codeEv1527_steps();

void rf_transferCodeEv1527(uint32_t code);

#endif
