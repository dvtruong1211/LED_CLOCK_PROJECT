#include "uartHandle.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "usart.h"
#include "buzzer.h"

#define UART_DEBUG &huart1
#define VERSION "version 1.0"
#define MAX_LEN_CMD_NAME (15)
#define MAX_NUM_CMD (1)
#define MAX_NUM_STR (2)

typedef void(*cmd_ptr_function)(uint8_t* , uint8_t* );
typedef struct
{
	uint16_t cmd_id;
	const char cmd_name[MAX_LEN_CMD_NAME];
	cmd_ptr_function ptr_fun;
} command_packet_t;


void uart_handleVersion(uint8_t* data_in, uint8_t* data_out);

command_packet_t cmd_user[MAX_NUM_CMD] =
{

	{1,"GET_VERSION",uart_handleVersion}
};


uart_handle_t uart1_struct = {{0},0,0,UART_STT_IDLE};
uint8_t g_rx_char;

void uart1_init()
{
			printf("/\r\n**********************************/\r\nhello...........\r\n/**********************************/\r\n");
			HAL_UART_Receive_IT(UART_DEBUG, &g_rx_char, 1);
}
//handle command
//data reply
void uart_reply(uint16_t cmd_id, uint8_t* cmd_name,  uint8_t* data_reply)
{
//	char buf_reply[100];
	//	sprintf((char*)buf_reply, "\n[%d, %s:\r\n%s]\r\n\n", cmd_id ,cmd_name, data_reply);
//	uart_transmit(USART1,(uint8_t*)buf_reply, strlen(buf_reply));
	printf("\n[%d, %s, %s]\r\n", cmd_id ,cmd_name, data_reply);
}

//tach chuoi thanh 2 phan(chuoi): ptr_arr[0] phan ten command, ptr_arr[1]: gia tri tham so
void uart_handleProcess(uint8_t* data_in, uint16_t length)
{
	uint16_t i =0;
	uint8_t data_reply[100] = {0};
	uint8_t* ptr_arr[MAX_NUM_STR] = {NULL, NULL};
	uint8_t num_ptr = 0;
	uint8_t num_cmd =0;
	uint8_t* buf_data_in = NULL;
	buf_data_in = data_in;

		if((buf_data_in[0]== '[') && (buf_data_in[length-1]== ']'))
		{
			ptr_arr[num_ptr] = buf_data_in +1;
			while(i < length )
			{
				if(buf_data_in[i] == ',')
				{
					num_ptr++;
					if(num_ptr == MAX_NUM_STR) break;
					buf_data_in[i] = '\0';
					ptr_arr[num_ptr] = buf_data_in+i+1;
				}
				i++;
			}
				buf_data_in[length-1] = '\0';

			for( num_cmd = 0; num_cmd < MAX_NUM_CMD; num_cmd++)
			{
				if(strcmp((const char*)ptr_arr[0], cmd_user[num_cmd].cmd_name) ==0)
				{
					cmd_user[num_cmd].ptr_fun(ptr_arr[1], (uint8_t*)data_reply);
					uart_reply(cmd_user[num_cmd].cmd_id, (uint8_t*)cmd_user[num_cmd].cmd_name,(uint8_t*)data_reply);
					break;
				}
			}

		}
		else printf("command not support");
}


uint16_t get_dataFromBuffer(uint8_t* data_out)
{
    uint16_t len = 0;
		len = uart1_struct.buffer_index;
//    for (len = 0; len < uart1_struct.buffer_index; len++)
//    {
//        data_out[len] = uart1_struct.buffer_rx[len];
//    }
//
	memcpy((void*)data_out,(const void*)uart1_struct.buffer_rx, uart1_struct.buffer_index);
		uart1_struct.buffer_index = 0;

    return len;
}

void uart1_addDataFromItr()
{
	if(uart1_struct.buffer_index < MAX_SIZE_BUFFER)
	{
		uart1_struct.buffer_rx[uart1_struct.buffer_index++] = g_rx_char;
	}
	uart1_struct.state_cnt = 3;
	HAL_UART_Receive_IT(UART_DEBUG, &g_rx_char, 1);
}

//ham xu li du lieu ban dau nhan tu uart
void uart1_process()
{
    uint8_t data_rx[100] = {0};
    uint16_t length = 0;
		length = get_dataFromBuffer((uint8_t*)data_rx);
		buzzer_onInMs(100);
		uart_handleProcess(data_rx, length);
}

void uart1_setTimeCheck(uint8_t time_cnt)
{
	uart1_struct.state_cnt = time_cnt;
}
void uart1_Handle_tick()		//1ms
{
	switch(uart1_struct.uart_stt)
	{
		case UART_STT_IDLE:
			if(uart1_struct.state_cnt)											//chua co du lieu thi k lam gi ca
			{
				uart1_struct.uart_stt = UART_STT_DATA_COMING;
			}
			break;
		case UART_STT_DATA_COMING:
			{
				if(--uart1_struct.state_cnt == 0)
					uart1_struct.uart_stt = UART_STT_DATA_DONE;//het timeout, thoi gian quiet
				break;
			}
		case UART_STT_DATA_DONE:

			uart1_process();
			uart1_struct.uart_stt = UART_STT_IDLE;
			break;

	}
}


//function for cmd_id1
void uart_handleVersion(uint8_t* data_in, uint8_t* data_out)
{
	strcpy((char*)data_out, (const char*)VERSION);
}



