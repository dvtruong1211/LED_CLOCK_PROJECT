#ifndef _UARTHANDLE_H_
#define _UARTHANDLE_H_
#include "stdbool.h"
#include "stdint.h"
typedef enum
{
	UART_STT_IDLE,
	UART_STT_DATA_COMING,
	UART_STT_DATA_DONE
} uart_data_status_t;

#define MAX_SIZE_BUFFER 100
typedef struct
{
	uint8_t buffer_rx[100];
	uint8_t buffer_index;
	volatile uint8_t state_cnt;
	uart_data_status_t uart_stt;

} uart_handle_t;


void uart1_addDataFromItr(void);
void uart1_Handle_tick(void);		//1ms
void uart1_init(void);

#endif
