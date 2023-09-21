#include "DateTime.h"
#include "led7seg.h"
#include "ds3231.h"

#ifndef DISABLE_ALL_LOG
#define ENABLE_LOG_DATETIME
#endif
#ifdef ENABLE_LOG_DATETIME
#define log_info(format, ... ) {printf("DATETIME: "); printf(format , ##__VA_ARGS__);}
#else
#define log_info(format, ... )
#endif

bool dateTime_checkValid(type_dateTime_t type,uint16_t value)
{
	switch(type)
	{
	case TYPE_SEC:
	case TYPE_MIN:
		if(value <0 || value > 59) return false;
		break;
	case TYPE_HOUR:
		if(value < 0 || value >23) return false;		//mac dinh che do 24h khi setting
		break;
	case TYPE_DAY:
		if(value < 1 || value > 7) return false;
		break;
	case TYPE_DATE:
		if(value < 1 || value > 31) return false;
		break;
	case TYPE_MONTH:
		if(value <1 || value >12) return false;
		break;
	case TYPE_YEAR:
		if(value <00 || value > 99) return false;
		break;
	}

	return true;
}
uint8_t dateTime_getMaxDateOfMonth(uint8_t month)
{
	switch(month)
	{
	case 1: case 3: case 5: case 7: case 8: case 10: case 12: return 31;
	case 2: return 29;			//tam thoi chua tinh nam nhuan
	default: return 30;
	}
}

void dateTime_PlusOrMinus(type_dateTime_t typeDateTime, bool isPlus)
{
	static uint8_t value_month;		//check gia tri month de setting date
	uint8_t value = 0;
	if(typeDateTime == TYPE_SEC || typeDateTime == TYPE_DAY) value = led7seg_getValueAtLed(LED7_SEC);
	else if(typeDateTime == TYPE_MIN || typeDateTime == TYPE_YEAR || typeDateTime == TYPE_MONTH) value = led7seg_getValueAtLed(LED7_MIN);
	else if (typeDateTime == TYPE_HOUR || typeDateTime == TYPE_DATE) value = led7seg_getValueAtLed(LED7_HOUR);
	value = (isPlus == true)? (value+1) : (value-1);

	switch(typeDateTime)
	{
	case TYPE_SEC:
		break;
	case TYPE_MIN:
		if(value == 60 && isPlus == true) value = 0;
		if(value == 0xff && isPlus == false) value = 59;
		break;
	case TYPE_HOUR:
		if(value == 24 && isPlus == true) value = 0;
		if(value == 0xff && isPlus == false) value = 23;
		break;
	case TYPE_DAY:
		if(value == 8 && isPlus == true) value = 1;
		if(value == 0 && isPlus == false) value = 7;
		break;
	case TYPE_DATE:
		if(value == dateTime_getMaxDateOfMonth(value_month)+1 && isPlus == true) value = 1;
		if(value == 0 && isPlus == false) value = dateTime_getMaxDateOfMonth(value_month);
		break;
	case TYPE_MONTH:
		if(value == 13 && isPlus == true) value = 1;
		if(value == 0 && isPlus == false) value = 12;
		value_month = value;
		break;
	case TYPE_YEAR:
		if(value == 100 && isPlus == true) value = 0;
		if(value == 0xff && isPlus == false) value = 99;
		break;
	}

	if(typeDateTime == TYPE_SEC || typeDateTime == TYPE_DAY)
	{
		led7seg_setValueAtLed(LED7_SEC, value);
		if(typeDateTime == TYPE_DAY)
		{
			if(value ==1)
			{
				led7seg_setCodeIndex(CODE_C,LED7_SEC1);
				led7seg_setCodeIndex(CODE_N,LED7_SEC2);
			}
			else led7seg_setCodeIndex(CODE_T,LED7_SEC1);
		}
	}
	else if(typeDateTime == TYPE_MIN || typeDateTime == TYPE_YEAR || typeDateTime == TYPE_MONTH)
	{
		led7seg_setValueAtLed(LED7_MIN, value);
		if(typeDateTime == TYPE_MONTH)
		{
			if(value < 10) led7seg_setCodeIndex(CODE_OFF,LED7_MIN1);
		}
	}
	else if (typeDateTime == TYPE_HOUR || typeDateTime == TYPE_DATE)
	{
		led7seg_setValueAtLed(LED7_HOUR, value);
	}
	log_info("value current: %d\r\n", value);
}

void dateTime_saveToDs3231(type_dateTime_t type)
{
	uint8_t value = 0;
	led7_id_t led7Id = LED7_SEC;
	switch(type)
	{
		case TYPE_HOUR:
		case TYPE_DATE:
			led7Id = LED7_HOUR;
			break;
		case TYPE_MIN:
		case TYPE_MONTH:
		case TYPE_YEAR:
			led7Id = LED7_MIN;
			break;
		case TYPE_SEC:
		case TYPE_DAY:
			led7Id = LED7_SEC;
			break;
	}
	value = led7seg_getValueAtLed(led7Id);
	ds3231_setAt(type, value);
	log_info("saved %d to ds3231 register %d\r\n",value, type);
	/*ghi xong doc lai de vua check vua s_timeDate update gia tri*/
	ds3231_timeDate_t timedate;
	if(ds3231_readAllTimeDate(&timedate) == DS3231_OK)
	{
		ds3231_set_s_timeDate(timedate);
	}

}

/*k ap dung cho sec and day*/
void dateTime_setTimeDateDirectionByDigitalKeyIr(type_dateTime_t typeDateTime, uint8_t digit)
{
	uint8_t value = 0;
	if(typeDateTime == TYPE_MIN || typeDateTime == TYPE_YEAR || typeDateTime == TYPE_MONTH) value = led7seg_getValueAtLed(LED7_MIN);
	else if (typeDateTime == TYPE_HOUR || typeDateTime == TYPE_DATE) value = led7seg_getValueAtLed(LED7_HOUR);

	if(value/10 == 0)
	{
		if(dateTime_checkValid(typeDateTime,value * 10 + digit) == true)
		{
			value = value * 10 + digit;
		}
		else value =  digit;
	}
	else
	{
		value =  digit;
	}
	if(value == 0 && (typeDateTime == TYPE_MONTH || typeDateTime == TYPE_DATE)) return;
	 if(typeDateTime == TYPE_MIN || typeDateTime == TYPE_YEAR || typeDateTime == TYPE_MONTH)
		{
			led7seg_setValueAtLed(LED7_MIN, value);
			if(typeDateTime == TYPE_MONTH || typeDateTime == TYPE_MIN)
			{
				if(value < 10) led7seg_setCodeIndex(CODE_OFF,LED7_MIN1);
			}
		}
		else if (typeDateTime == TYPE_HOUR || typeDateTime == TYPE_DATE)
		{
			led7seg_setValueAtLed(LED7_HOUR, value);
		}
	log_info("value current: %d\r\n", value);
}
