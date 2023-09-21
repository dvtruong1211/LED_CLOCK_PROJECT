#include "userConfig.h"
#include "flash.h"
#include "string.h"

#ifndef DISABLE_ALL_LOG
#define ENABLE_LOG_USERCONFIG
#endif
#ifdef ENABLE_LOG_USERCONFIG
#define log_info(format, ... ) {printf("USER_CONFIG: "); printf(format , ##__VA_ARGS__);}
#else
#define log_info(format, ... )
#endif

user_data_flash_t s_dataConfig;

extern uint32_t keyCodeIR[NUM_CODE_IR_MAX];
extern color_t colorValueLed7[NUM_LED7_RTC];
bool check_flash_Isblank(uint8_t* data, uint16_t numByte)
{
	for(int i = 0; i< numByte; i++)
	{
		if(data[i] != 0xff) return false;
	}
	return true;
}

void userConfig_default()
{
	memset((void*)s_dataConfig.keyDataIr, 0, sizeof(uint32_t) * NUM_CODE_IR_MAX);
	memcpy((void*)s_dataConfig.colorDataLed7, (void*)colorValueLed7, sizeof(color_t) * NUM_LED7_RTC);

}
void userConfig_flashConfig()
{
	flash_readData(FLASH_ADDRESS_USER_CONFIG, (uint8_t*)&s_dataConfig, sizeof(s_dataConfig));
	if(check_flash_Isblank((uint8_t*)&s_dataConfig, sizeof(s_dataConfig)) == true)
	{
		userConfig_default();
		flash_writeData(FLASH_ADDRESS_USER_CONFIG, (uint16_t*)&s_dataConfig, sizeof(s_dataConfig));
		log_info("flash blank, write default\r\n");
	}

	memcpy((void*)keyCodeIR, s_dataConfig.keyDataIr, sizeof(uint32_t) * NUM_CODE_IR_MAX);
	memcpy((void*)colorValueLed7, s_dataConfig.colorDataLed7, sizeof(color_t) * NUM_LED7_RTC);


	log_info("CODE IR:\r\n");
	printf("key: code \r\n");
	for(int i = 0; i< NUM_CODE_IR_MAX; i++)
	{
		printf("%d    %d\r\n",  (int)i, (int)keyCodeIR[i]);
	}
	log_info("COLOR_VALUE:\r\n");
	printf("led:   r   g    b\r\n");
	for(int i = 0; i< NUM_LED7_RTC; i++)
	{
		printf("%d     %d   %d   %d\r\n",  (int)i, (int)colorValueLed7[i].r, (int)colorValueLed7[i].g,(int)colorValueLed7[i].b);
	}
}

void user_flashUpdate()
{
	memcpy((void*)s_dataConfig.keyDataIr,(void*)keyCodeIR,  sizeof(uint32_t) * NUM_CODE_IR_MAX);
	memcpy((void*)s_dataConfig.colorDataLed7,(void*)colorValueLed7, sizeof(color_t) * NUM_LED7_RTC);
	flash_writeData(FLASH_ADDRESS_USER_CONFIG, (uint16_t*)&s_dataConfig, sizeof(s_dataConfig));
	log_info("flash update data\r\n");

}
