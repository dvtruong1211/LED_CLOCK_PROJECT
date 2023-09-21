#ifndef _DATETIME_H_
#define _DATETIME_H_
#include "global.h"


typedef enum
{
	TYPE_SEC = 0,		//gia tri nay cung chinh la dia chi thanh ghi
	TYPE_MIN,
	TYPE_HOUR,
	TYPE_DAY,
	TYPE_DATE,
	TYPE_MONTH,
	TYPE_YEAR
} type_dateTime_t;



void dateTime_PlusOrMinus(type_dateTime_t typeDateTime, bool isPlus);
void dateTime_saveToDs3231(type_dateTime_t type);

void dateTime_setTimeDateDirectionByDigitalKeyIr(type_dateTime_t type, uint8_t digit);



#endif
