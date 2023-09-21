#include "flash.h"
#include "userConfig.h"

uint16_t test_write[5] = {1,2,3,4,5};
uint16_t test_read[10] = {1,1,1,1,1,1,1,1,1,1};
/*datasizeof: so byte*/
void flash_writeData(uint32_t address, uint16_t* data, uint16_t dataSizeof)
{

	uint32_t PageError =0;
	FLASH_EraseInitTypeDef EraseInit = {FLASH_TYPEERASE_PAGES, address, 1};
	HAL_FLASH_Unlock();
	if(HAL_FLASHEx_Erase(&EraseInit, &PageError) == HAL_OK)
	{
		uint16_t numElem;
		numElem = (dataSizeof%2 == 0)? (dataSizeof/2): (dataSizeof/2 +1);
		for(uint16_t i = 0; i < numElem; i++)
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address + i*2, (uint64_t)(*(data+i)));
		}
	}
	HAL_FLASH_Lock();
}


void flash_readData(uint32_t address, uint8_t* data_out, uint16_t dataSizeOf)
{
	for(uint16_t i =0; i < dataSizeOf; i++)
	{
		*(data_out + i) = *((uint8_t*)(address+i));
	}
}

void flash_write(uint32_t address, uint64_t data, uint32_t TypeProgram)
{
	uint32_t PageError =0;
	FLASH_EraseInitTypeDef EraseInit = {FLASH_TYPEERASE_PAGES, address, 1};
	HAL_FLASH_Unlock();
	if(HAL_FLASHEx_Erase(&EraseInit, &PageError) == HAL_OK)
	{
			HAL_FLASH_Program(TypeProgram, address, data);
	}
	HAL_FLASH_Lock();
}

uint64_t flash_read(uint32_t address, uint32_t TypeProgram)
{
			uint64_t data_return = 0xFFFFFFFFFFFFFFF;
			 if(TypeProgram == FLASH_TYPEPROGRAM_HALFWORD) 	 data_return = (uint64_t)(*((uint16_t*)address));
	else if(TypeProgram == FLASH_TYPEPROGRAM_WORD) 			 data_return = (uint64_t)(*((uint32_t*)address));
	else if(TypeProgram == FLASH_TYPEPROGRAM_DOUBLEWORD) data_return = *((uint64_t*)address);
	return data_return;
}


void test_flash()
{
	flash_writeData(FLASH_ADDRESS_USER_CONFIG, (uint16_t*)test_write, 10);
	flash_readData(FLASH_ADDRESS_USER_CONFIG, (uint8_t*)test_read, 20);
	for(uint8_t i = 0; i < 10; i++)
	{
		printf("%d ",test_read[i]);
	}

}
