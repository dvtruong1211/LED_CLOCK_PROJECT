#include "capture.h"
#include "tim.h"
#include "main.h"
#include "stdbool.h"
#include "string.h"
#include "buzzer.h"
#include "timeCheck.h"
#include "IR_decoder.h"
#include "IR_keyCode.h"
/*dang khung truyen toshiba
 * 			  4000	1600 600			    39000		       2000    96000
 _______  	   __	__	 _ 		    ..._	_______		       ___	  _____________
  	  	\_____/  \_/  \_/ \..........	\__/	        \_____/   \__/
  	  	  9000   600  600				600			    9000	  600
  	  	  start  B1 	B0   	   	   	trail_stop |   neu nhan giu thi gui noi vao
		  <--------106000-------------------------->    <--------106000----------->
  doi voi remote toshiba neu nhan giu thi sau bit stop se lien tuc gui di khung truyen moi (su dung repeat)
				 2000		96000
					 ___	__________
			  \_____/   \__/
			   9000      600
	(time_mid)
			<--------106000----------->
	 voi tong thoi gian CONST_LENTH 106000ms
 * */
#define IR_CHECK_TIME_MID_OK(t) (t> 95000 && t < 97000)
#define IR_BIT_HOLDING_L_OK(t) (t > 8500 && t< 9500)
#define IR_BIT_HOLDING_H_OK(t) (t > 1800 && t< 2500)


#define MAX_SIZE_BUF_DATA 40
#define EDGE_IS_RISING ((CAP_IR_GPIO_Port->IDR & CAP_IR_Pin) != 0)	//read pin
#define EDGE_IS_FALLING	 ((CAP_IR_GPIO_Port->IDR & CAP_IR_Pin) == 0)
#define DIF_VALUE(cur, pre) ((cur < pre)?(0xFFFFFFFF - pre +cur + 1):(cur - pre))

ic_state_t s_state = IC_STATE_START;

uint32_t							ic_preValue = 0;
uint32_t							ic_curValue = 0;
uint32_t 							ic_highDifValue = 0;
uint32_t 							ic_lowDifValue = 0;
uint8_t diff_index= 0;
uint32_t               uwDiffCapHigh[MAX_SIZE_BUF_DATA] = {0};
uint32_t               uwDiffCapLow[MAX_SIZE_BUF_DATA] = {0};

extern uint32_t g_sys_tick;
bool ic_isCaturing = false;
uint32_t timeCapturing = 0;
bool hadData = false;

//uint8_t ic_cnt_timeout = 0;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
	if (EDGE_IS_RISING)	//Check for interrupt on rising edge on capture signal
	{
		 ic_curValue = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);; //get capture signal counter value
		 ic_lowDifValue = DIF_VALUE(ic_curValue,ic_preValue);
		 ic_preValue = ic_curValue;
	 }
	 if (EDGE_IS_FALLING) { //Check for interrupt on falling edge on capture signal
		 ic_curValue = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); //get capture signal counter value
		 ic_highDifValue = DIF_VALUE(ic_curValue,ic_preValue);
		 ic_preValue = ic_curValue;
	 }
	 //ic_cnt_timeout = 2; //dem giong xu li uart
	 ic_isCaturing= true;
	 timeCapturing = g_sys_tick;

	 switch(s_state)
	 {
	 	 case IC_STATE_NONE:
	 			 break;
		 case IC_STATE_START:
			 if(hadData){			//neu bat dau co ma code dau tien thi moi check xem co nhan giu khong
				// if(IR_CHECK_TIME_MID_OK(ic_highDifValue))
				 if(IR_BIT_HOLDING_H_OK(ic_highDifValue) && IR_BIT_HOLDING_L_OK(ic_lowDifValue))
				 {
					s_state = IC_STATE_HOLD;
					break;
				 }
			 }
			 if(IR_checkBit_start_ok(ic_highDifValue, ic_lowDifValue) == true)
			 {
				 s_state = IC_STATE_DATA;
				 diff_index = 0;
			 }
			 break;
		 case IC_STATE_HOLD:

			 break;
		 case IC_STATE_DATA:
			 if(EDGE_IS_RISING)
			 {
				 	 uwDiffCapLow[diff_index] = ic_lowDifValue;
			 }
			 else if(EDGE_IS_FALLING)
			 {
				 	 uwDiffCapHigh[diff_index] = ic_highDifValue;
				 	 diff_index++;
				 	 if(diff_index == NUM_BIT_IR_CODE + 2)	//+1 bit stop
				 	 {
				 		 s_state = IC_STATE_STOP;
				 	 }
			 }
			 break;
		 case IC_STATE_STOP:
			 break;

	 }

//		 if(diff_index == 0)
//		 {
//			 buzzer_onInMs(100);
//		 }
//		 if(diff_index == MAX_SIZE_BUF_DATA) return;
//
//		 if(EDGE_IS_FALLING) uwDiffCapHigh[diff_index++] = ic_highDifValue;
//		 else if(EDGE_IS_RISING) uwDiffCapLow[diff_index] = ic_lowDifValue;



  }

}
/*xu li giong nhan uart,
 * tinh tu lan cuoi capture, sau khoang timeout ma k con capture nua thi coi nhu nhan xong 1 khung */
uint32_t irCode;
bool isHolding = false;
void capture_tick()		//100ms
{

		if(s_state == IC_STATE_STOP)
		{
			printf("Capture: received:\r\n");
//			for(int i = 0 ; i < MAX_SIZE_BUF_DATA; i++)
//			{
//				printf("%d   ",(int)uwDiffCapHigh[i]);
//				printf("%d\r\n",(int)uwDiffCapLow[i]);
//			}
//			printf("CAP: %d\r\n",s_state);
//			printf("\r\n");printf("\r\n");
			if(IR_processRawDataOk(uwDiffCapHigh, uwDiffCapLow,&irCode) == true)
			{
				printf("IR callback:...\r\n");
				IR_keyCodeCallback(irCode);
				hadData = true;
			}
			memset((void*)uwDiffCapHigh, 0, MAX_SIZE_BUF_DATA*4);
			memset((void*)uwDiffCapLow, 0, MAX_SIZE_BUF_DATA*4);
			s_state = IC_STATE_START;
		}
		else if(s_state == IC_STATE_HOLD)
		{
			isHolding = true;
			printf("IR callback:...\r\n");
			IR_keyCodeCallback(irCode);		//neu là holding thi van là ma truoc do ke tu state STOP
			s_state = IC_STATE_START;
		}
		/*check released*/
		if(ic_isCaturing)
		if(elapsedTime(g_sys_tick, timeCapturing) > 150)
		{
			ic_isCaturing = false;
			if(isHolding)
			{
				IR_keyCodeUpOrDownReleaseFromHold();
				isHolding = false;
			}
//			printf("Capture: received:\r\n");
//			for(int i = 0 ; i < MAX_SIZE_BUF_DATA; i++)
//			{
//				printf("%d   ",uwDiffCapHigh[i]);
//				printf("%d\r\n",uwDiffCapLow[i]);
//			}
//			printf("\r\n");printf("\r\n");
//			printf("CAP state: %d, diff_index %d\r\n",s_state, diff_index);
//			if(IR_processRawDataOk(uwDiffCapHigh, uwDiffCapLow,&irCode) == true)
//			{
//				printf("IR callback:...\r\n");
//			}
			memset((void*)uwDiffCapHigh, 0, MAX_SIZE_BUF_DATA*4);
			memset((void*)uwDiffCapLow, 0, MAX_SIZE_BUF_DATA*4);
			hadData = false;
			s_state = IC_STATE_START;
			diff_index = 0;
			irCode = 0;
//			printf("CAP state: %d, diff_index %d\r\n",s_state, diff_index);
		}
}

