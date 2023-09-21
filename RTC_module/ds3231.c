#include "ds3231.h"
#include "i2c.h"
#include "led7seg.h"
#include "global.h"
#include "buzzer.h"

#ifndef DISABLE_ALL_LOG
//#define ENABLE_LOG_DS3231
#endif
#ifdef ENABLE_LOG_DS3231
#define log_info(format, ... ) {printf("DS3231: "); printf(format , ##__VA_ARGS__);}
#else
#define log_info(format, ... )
#endif

#define BIT_MODE    0b01000000	//vi tri bit trong thanh ghi ds3231
#define BIT_SESSION 0b00100000
rtc_mode_t s_mode = _MODE_24;
rtc_session_t s_session = _AM;		//chi quan tam khi s_mode = _MODE_12
ds3231_timeDate_t s_timeDate;
float s_temperature;
extern uint32_t g_sys_tick;
bool timeDateRead_en  = true;
// Convert normal decimal numbers to binary coded decimal
uint8_t decToBcd(uint8_t val)
{
	return ( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
uint8_t bcdToDec(uint8_t val)
{
	return ( (val/16*10) + (val%16) );
}

ds3231_time_t ds3231_time_bcdToDec(ds3231_time_t time)
{
	ds3231_time_t ret;
	s_mode = (time.h & BIT_MODE)? _MODE_12: _MODE_24;
	if(s_mode == _MODE_12)
	{
		s_session = (time.h & BIT_SESSION)? _PM: _AM;
		ret.h = bcdToDec(time.h & 0b00011111);
	}
	else ret.h = bcdToDec(time.h);

	ret.m = bcdToDec(time.m);
	ret.s = bcdToDec(time.s);
	return ret;
}
ds3231_time_t ds3231_time_decToBcd(ds3231_time_t time)
{
	ds3231_time_t ret;
	if(s_mode == _MODE_12)
	{
		ret.h = decToBcd(time.h) | BIT_MODE | (s_session<<5);		// 1<= time.h <=12
	}
	else
	{
		ret.h = decToBcd(time.h);
	}
	ret.m = decToBcd(time.m);
	ret.s = decToBcd(time.s);
	return ret;
}
ds3231_date_t ds3231_date_bcdToDec(ds3231_date_t date)
{
	ds3231_date_t ret;
	ret.dy 		= bcdToDec(date.dy);
	ret.dt 		= bcdToDec(date.dt);
	ret.month 	= bcdToDec(date.month);
	ret.year 	= bcdToDec(date.year);
	return ret;
}
ds3231_date_t ds3231_date_decToBcd(ds3231_date_t date)
{
	ds3231_date_t ret;
	ret.dy 		= decToBcd(date.dy);
	ret.dt 		= decToBcd(date.dt);
	ret.month 	= decToBcd(date.month);
	ret.year 	= decToBcd(date.year);
	return ret;
}
ds3231_timeDate_t ds3231_bcdToDec_AllTime(ds3231_timeDate_t val)
{
	ds3231_timeDate_t ret;
	ret.time = ds3231_time_bcdToDec(val.time);
	ret.date = ds3231_date_bcdToDec(val.date);
	return ret;
}
ds3231_timeDate_t ds3231_decToBcd_AllTime(ds3231_timeDate_t val)
{
	ds3231_timeDate_t ret;
	ret.time = ds3231_time_decToBcd(val.time);
	ret.date = ds3231_date_decToBcd(val.date);
	return ret;
}

bool ds3231_readReg(uint8_t address, uint8_t* value)
{
	if(HAL_I2C_Mem_Read(&hi2c1, DS3231_DEVICE_ADD<<1, address, I2C_MEMADD_SIZE_8BIT, value, 1, 10) != HAL_OK)
	{
		log_info("read at address %x fail!\r\n",address);
		return 0;
	}
	else
	{
		log_info("read at address %d success, value: %x\r\n", address, *value);
		return DS3231_OK;
	}
}
bool ds3231_writeReg(uint8_t address, uint8_t value)
{
	if(HAL_I2C_Mem_Write(&hi2c1, DS3231_DEVICE_ADD<<1, address, I2C_MEMADD_SIZE_8BIT, &value, 1, 10) != HAL_OK)
	{
		log_info("write at address %x fail!\r\n",address);
		return 0;
	}
	else
	{
		log_info("write value: %x at address %d success\r\n", value, address);
		return DS3231_OK;
	}
}
void ds3231_logMode()
{
	char* buf1 = "", *buf2 ="";
	buf1 = (s_mode == _MODE_12)? "MODE_12": "MODE_24";
	buf2 = (s_session == _AM)? "AM": "PM";
	log_info("mode: %s, session: %s \r\n", buf1, buf2);
}
void ds3231_logTime(ds3231_timeDate_t timedate, bool isDec)
{
	if(isDec)
	{
		log_info("time and date dec:\r\n");
		log_info("time:%d - %d - %d\r\nday: %d, date: %d- %d- %d\r\n", timedate.time.h,  timedate.time.m, timedate.time.s,
											timedate.date.dy, 	timedate.date.dt, 	timedate.date.month, 	timedate.date.year);


	}
	else
	{
		log_info("time and date bcd:\r\n");
		log_info("time:%x - %x - %x\r\nday: %x, date: %x- %x- %x\r\n", timedate.time.h,  timedate.time.m, timedate.time.s,
											timedate.date.dy, 	timedate.date.dt, 	timedate.date.month, 	timedate.date.year);
	}
}


bool ds3231_readAllTimeDate(ds3231_timeDate_t* timeDate)
{
	if(HAL_I2C_Mem_Read(&hi2c1, DS3231_DEVICE_ADD<<1, ADDR_TIME, I2C_MEMADD_SIZE_8BIT, (uint8_t*)timeDate, NUM_REG_TIMEDATE, 10) != HAL_OK)
	{
		log_info("read all time fail!\r\n");
		return 0;
	}
	else
	{
		//log_info("read all time success\r\n");
		*timeDate = ds3231_bcdToDec_AllTime(*timeDate);
		return DS3231_OK;
	}
}

bool ds3231_readTime(ds3231_time_t* time)
{

	if(HAL_I2C_Mem_Read(&hi2c1, DS3231_DEVICE_ADD<<1, ADDR_TIME, I2C_MEMADD_SIZE_8BIT, (uint8_t*)time, NUM_REG_TIME, 10) != HAL_OK)
	{
		log_info("read time fail!\r\n");
		return 0;
	}
	else
	{
		//log_info("read time success\r\n");
		*time = ds3231_time_bcdToDec(*time);
		return DS3231_OK;
	}
}
bool ds3231_readDate(ds3231_date_t* date)
{
	if(HAL_I2C_Mem_Read(&hi2c1, DS3231_DEVICE_ADD<<1, ADDR_DATE, I2C_MEMADD_SIZE_8BIT, (uint8_t*)date, NUM_REG_DATE, 10) != HAL_OK)
	{
		log_info("read date fail!\r\n");
		return 0;
	}
	else
	{
		//log_info("read date success\r\n");
		*date = ds3231_date_bcdToDec(*date);
		return DS3231_OK;
	}
}

bool ds3231_readTemperature(float* temp)
{
	uint8_t pdata[2];
	if(HAL_I2C_Mem_Read(&hi2c1, DS3231_DEVICE_ADD<<1, ADDR_TEMP, I2C_MEMADD_SIZE_8BIT, pdata, NUM_REG_TEMP, 300) != HAL_OK)
	{
		log_info("read temperature fail!\r\n");
		return 0;
	}
	else
	{
		log_info("read temperature success: ");
		*temp = (float)pdata[0] + (float)(pdata[1]>>6)*0.25;
		log_info("%d.%d *C\r\n",pdata[0],(pdata[1]>>6)*25);	//k printf dc %f
		return DS3231_OK;
	}
}

void ds3231_enableOsc(bool isEn)
{
	uint8_t value;
	if(ds3231_readReg(ADDR_CTRL1, &value) == DS3231_OK)
	{
		if(isEn) value &= 0x7F;		// bit 7 = 0: enable
		else value |= 0x80;
		ds3231_writeReg(ADDR_CTRL1, value);
	}
}

bool ds3231_setAllTime(ds3231_timeDate_t timeDate)
{
	ds3231_timeDate_t pdata;
	pdata = ds3231_decToBcd_AllTime(timeDate);
	if(HAL_I2C_Mem_Write(&hi2c1, DS3231_DEVICE_ADD<<1, ADDR_TIME, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&pdata, NUM_REG_TIMEDATE, 10) != HAL_OK)
	{
		log_info("write all time fail!\r\n");
		return 0;
	}
	log_info("write all time success!");
	return DS3231_OK;
}
//ghi thoi gian cho tung thanh ghi
void ds3231_setAt(uint8_t addr, uint8_t value)
{
	uint8_t timeBcd;
	timeBcd = decToBcd(value);
	if(ds3231_writeReg(addr, timeBcd) == DS3231_OK)
	{
		log_info("set time at address %d ok", addr);
	}
	else log_info("set time at address %d fail", addr);

}
/*chu y: trong thanh ghi ds3231 hour: che do mode_12 (mode-24): AM 12->11 (00->11), PM 12->11 (12->23)*/
bool sw_ready = true;
uint32_t timeStartSw = 0;
void ds3231_switchMode()
{
	if(!sw_ready) return;
	if(s_timeDate.time.m == 59 && s_timeDate.time.s == 59) return;
	/*tranh truong hop: co the ds3231 da thay doi thanh ghi hour (+ len 1)
	 * ma chua kip doc ra thi ham nay lai ghi vao gia tri hour truoc do*/
	//log_info("switch mode\r\n");
	buzzer_onInMs(100);
	if(s_mode == _MODE_24)
	{
		s_mode = _MODE_12;
		if(s_timeDate.time.h > 11)
		{
			if(s_timeDate.time.h > 12) { s_timeDate.time.h -= 12; }
			s_session = _PM;
		}
		else
		{
			if(s_timeDate.time.h == 0) { s_timeDate.time.h = 12;}
			 s_session = _AM;
		}
		s_timeDate.time.h = decToBcd(s_timeDate.time.h) | BIT_MODE | (s_session<<5);		// 1<= time.h <=12
	}
	else
	{
		s_mode = _MODE_24;
		if(s_session == _PM) { if(s_timeDate.time.h <12)   s_timeDate.time.h += 12;}
		else if(s_timeDate.time.h == 12) s_timeDate.time.h = 0;
		s_timeDate.time.h = decToBcd(s_timeDate.time.h);
	}
	ds3231_writeReg(ADDR_HOUR, s_timeDate.time.h);
	timeStartSw = g_sys_tick;
	sw_ready = false;
}
/*khi IR nhan qua nhanh thi ghi vao lien tuc ds3231 bi loi
 * co the kiem tra bang cach giong nhu ham uiControl_controlPower() de don gian*/
void ds3231_sw1224checkTimeReady()
{
	if(!sw_ready)
	{
		if(g_sys_tick - timeStartSw > 900)
		{
			sw_ready = true;
		}
	}
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance == hi2c1.Instance)
	{

	}
}
/*mac dinh khi vao che do cai dat voi user thi se chuyen thoi gian cai dat ve mode 24.*/
void ds3231_setTimeDefault()
{
	ds3231_timeDate_t timedateDf;
		timedateDf.time.h = 21;
		timedateDf.time.m = 25;
		timedateDf.time.s = 10;
		timedateDf.date.dy = 6;
		timedateDf.date.dt = 15;
		timedateDf.date.month= 3;
		timedateDf.date.year = 19;
		s_mode = _MODE_24;
	ds3231_setAllTime(timedateDf);
	ds3231_readAllTimeDate(&s_timeDate);
}
void ds3231_settingTimeDate(ds3231_timeDate_t timedate)
{
	s_mode = _MODE_24;
	ds3231_setAllTime(timedate);
	ds3231_readAllTimeDate(&s_timeDate);
}


void ds3231_getTimeDateTick()			//1s
{
	ds3231_sw1224checkTimeReady();
	if(timeDateRead_en){
		ds3231_time_t time;
		ds3231_date_t date;
		if(ds3231_readTime(&time) == DS3231_OK)
		{
			s_timeDate.time = time;
		}
		if(ds3231_readDate(&date) == DS3231_OK)
		{
			s_timeDate.date = date;
		}
		led7seg_setValueLed(s_timeDate.time.h,s_timeDate.time.m,s_timeDate.time.s);
		ds3231_logTime(s_timeDate, true);
		ds3231_logMode();

	}

}
void ds3231_stopGetTimeDate()
{
	timeDateRead_en = false;
}
void ds3231_startGetTimeDate()
{
	ds3231_timeDate_t timedate;
	if(ds3231_readAllTimeDate(&timedate) == DS3231_OK)
	{
		s_timeDate = timedate;
		led7seg_setValueLed(s_timeDate.time.h,s_timeDate.time.m,s_timeDate.time.s);
	}
	timeDateRead_en = true;
}
ds3231_timeDate_t ds3231_get_s_timeDate()
{
	return s_timeDate;
}
void ds3231_set_s_timeDate(ds3231_timeDate_t timeDate)
{
	s_timeDate = timeDate;
}
void ds3231_testAll()
{
	//	ds3231_readTemperature(&temp);
	//ds3231_readReg(ADDR_CTRL1, &ctr_value);
}




