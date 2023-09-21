#ifndef _TIMECHECK_H_
#define _TIMECHECK_H_
#include "stdbool.h"
#include "stdint.h"


uint32_t elapsedTime(uint32_t newTime,uint32_t oldTime);
bool timeIsAfter(uint32_t newTime,uint32_t oldTime);

#endif
