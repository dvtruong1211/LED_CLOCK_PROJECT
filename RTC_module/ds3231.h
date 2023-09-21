#ifndef _DS3231_H_
#define _DS3231_H_
#include "stdint.h"
#include "stdbool.h"
#define DS3231_DEVICE_ADD 0x68
#define DS3231_OK 1
enum
{
	ADDR_SEC = 0x00,
	ADDR_MIN,
	ADDR_HOUR,
	ADDR_DY,
	ADDR_DT,
	ADDR_MONTH,
	ADDR_YEAR,
	ADDR_TEMP = 0x11,
	ADDR_CTRL1 = 0x0E,
	ADDR_CTRL2 = 0x0F,

	ADDR_TIME = ADDR_SEC,
	ADDR_DATE = ADDR_DY,

	NUM_REG_TIME = 3,
	NUM_REG_DATE = 4,
	NUM_REG_TEMP = 2,
	NUM_REG_TIMEDATE = 7
};
typedef enum
{
	DAY_CN = 1,
	DAY_T2,
	DAY_T3,
	DAY_T4,
	DAY_T5,
	DAY_T6,
	DAY_T7
} ds3231_day_t;
typedef enum
{
	_MODE_24 =0,
	_MODE_12
} rtc_mode_t;
typedef enum
{
	_AM = 0,
	_PM =1
} rtc_session_t;
/*luu y cau truc dung thu tu dia chi thanh ghi de khi truyen vao *pData trong i2c thi lay gia tri dung thu tu*/
typedef struct
{
	uint8_t s;	//0x00
	uint8_t m;
	uint8_t h;
} ds3231_time_t;
typedef struct
{
	uint8_t dy;
	uint8_t dt;
	uint8_t month;
	uint8_t year;	//0x06
} ds3231_date_t;

typedef struct
{
	ds3231_time_t time;
	ds3231_date_t date;
} ds3231_timeDate_t;


bool ds3231_readAllTimeDate(ds3231_timeDate_t* timeDate);
bool ds3231_readTemperature(float* temp);

void ds3231_setTimeDefault();
void ds3231_switchMode();
void ds3231_setAt(uint8_t addr, uint8_t value);
ds3231_timeDate_t ds3231_get_s_timeDate();
void ds3231_set_s_timeDate(ds3231_timeDate_t timeDate);


void ds3231_testAll();
void ds3231_getTimeDateTick();			//1s
void ds3231_stopGetTimeDate();
void ds3231_startGetTimeDate();

#endif
