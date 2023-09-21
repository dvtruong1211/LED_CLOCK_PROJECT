#include "rf_transfer.h"
#include "gpio.h"
#include "tim.h"
#include "stm32f3xx_hal_tim.h"

#ifndef DISABLE_ALL_LOG
#define ENABLE_LOG_RF
#endif
#ifdef ENABLE_LOG_RF
#define log_info(format, ... ) {printf("RF: "); printf(format , ##__VA_ARGS__);}
#else
#define log_info(format, ... )
#endif


#define PIN_RF_OUT_SET      HAL_GPIO_WritePin(DATA_RF_GPIO_Port, DATA_RF_Pin, GPIO_PIN_SET)
#define PIN_RF_OUT_RESET    HAL_GPIO_WritePin(DATA_RF_GPIO_Port, DATA_RF_Pin, GPIO_PIN_RESET)
enum
{
    BIT_TYPE_0 = 0,
    BIT_TYPE_1,
    BIT_TYPE_f       //floating
};
enum
{
    WIDTH_0_IDX = 0,
    WIDTH_1_IDX
};
typedef struct
{
    uint32_t width_preamb[2];
    uint32_t width_bit[2][2];
} timeSilce_ev1527_t;
typedef enum
{
    STEP_PREAMB = 0,
    STEP_CODE
} steps_state_ev1527_t;

typedef enum
{
    LEVEL_STATE_LOW = 0,
    LEVEL_STATE_HIGH
} level_state_pin_ev1527_t;

typedef struct
{
    steps_state_ev1527_t step;
    level_state_pin_ev1527_t level;
} state_transfer_ev1527_t;



void rf_transfer_codeEv1527_steps();


state_transfer_ev1527_t state_transfer_ev1527 = {STEP_PREAMB, LEVEL_STATE_HIGH};
timeSilce_ev1527_t s_timeSlide_ev1527 = {
                    .width_preamb = {
                                        [WIDTH_0_IDX] = 9000,
                                        [WIDTH_1_IDX]=  300,
                                        },
                    .width_bit = {
                        [BIT_TYPE_0]={
                                        [WIDTH_0_IDX] = 900,
                                        [WIDTH_1_IDX] = 300,
                                        },
                        [BIT_TYPE_1]={
                                        [WIDTH_0_IDX] = 300,
                                        [WIDTH_1_IDX] = 900,
                                        },
                    }
            };



rf_infoTransfer_t s_infoTransfer;
bool codeEv1527Ready = false;

uint8_t  cnt = 0;


/*luu y:
 * doi voi cau hinh cho timer:
 * co the ghi gia tri vao thanh ghi counter hoac thanh ghi auto-reload
 * che do Without Auto-reload: update event thuc hien so sanh gia tri trong counter voi thanh ghi auto-reload
 * che do Auto-reload: update event so sanh counter voi thanh ghi shadow
 * 						(gia tri thanh ghi shadow duoc reload tu thanh ghi auto-reload
 * 						(da duoc ghi o thoi diem nao truoc do) khi ma co update event xay ra)*/

void timer_enableAfter(uint32_t us)
{
	log_info("start_timer 15...");
	__HAL_TIM_SET_AUTORELOAD(&htim15, us);
	__HAL_TIM_SET_COUNTER(&htim15, 0);
	HAL_TIM_Base_Start_IT(&htim15);

}
void timer_disable()
{

	HAL_TIM_Base_Stop_IT(&htim15);
	log_info("timer pause");
}

void rf_transfer_stop()
{
	HAL_TIM_Base_Stop_IT(&htim15);
	codeEv1527Ready = false;
  //log_info("stop transfer RF code")
}
void rf_transfer_setTimePulseWidth(uint32_t time_us)
{
	__HAL_TIM_SET_AUTORELOAD(&htim15, time_us);
	__HAL_TIM_SET_COUNTER(&htim15, 0);
}

void rf_transfer_resetStateTransferEv1527()
{
    state_transfer_ev1527.step = STEP_PREAMB;
    state_transfer_ev1527.level = LEVEL_STATE_HIGH;
}


/*xu li trong ngat timer, moi lan ngat la nap gia tri do rong xung moi cho pin out*/
void rf_transfer_codeEv1527_steps()
{
    static uint32_t bitIndex = 0x00800000;      //truyen bit MSB trong ma code truoc
    switch (state_transfer_ev1527.step)
    {
        case STEP_PREAMB:
        {
            switch (state_transfer_ev1527.level)
            {
                case LEVEL_STATE_HIGH:
                {
                    PIN_RF_OUT_SET;             //set level pulse muc 1
                    rf_transfer_setTimePulseWidth(s_timeSlide_ev1527.width_preamb[WIDTH_1_IDX]);
                    state_transfer_ev1527.level = LEVEL_STATE_LOW;
                    break;
                }
                case LEVEL_STATE_LOW:
                {
                    PIN_RF_OUT_RESET;            //set level pulse muc 0
                    rf_transfer_setTimePulseWidth(s_timeSlide_ev1527.width_preamb[WIDTH_0_IDX]);
                    state_transfer_ev1527.level = LEVEL_STATE_HIGH;
                    state_transfer_ev1527.step = STEP_CODE;
                    bitIndex = 0x00800000;
                    break;
                }
            }
            break;
        }

        case STEP_CODE:
        {
            switch (state_transfer_ev1527.level)
            {
                case LEVEL_STATE_HIGH:
                {
                    uint8_t bitType;
                    bitType = (s_infoTransfer.rfCode & bitIndex) ? BIT_TYPE_1: BIT_TYPE_0;
                    PIN_RF_OUT_SET;             //set level pulse muc 1
                    rf_transfer_setTimePulseWidth(s_timeSlide_ev1527.width_bit[bitType][WIDTH_1_IDX]);
                    state_transfer_ev1527.level = LEVEL_STATE_LOW;
                    break;
                }
                case LEVEL_STATE_LOW:
                {
                    uint8_t bitType;
                    bitType = (s_infoTransfer.rfCode & bitIndex) ? BIT_TYPE_1: BIT_TYPE_0;
                    PIN_RF_OUT_RESET;            //set level pulse muc 0
                    rf_transfer_setTimePulseWidth(s_timeSlide_ev1527.width_bit[bitType][WIDTH_0_IDX]);
                    state_transfer_ev1527.level = LEVEL_STATE_HIGH;
                    bitIndex = bitIndex>>1;
                    if(bitIndex == 0)
                    {
                        state_transfer_ev1527.step = STEP_PREAMB;
                           if(++cnt >= s_infoTransfer.timesTransfer)     //du so lan khung truyen
                            {
                                cnt = 0;
                                rf_transfer_stop();
                            }
                    }
                    break;
                }
            }

            break;
        }
    }
}

/*truyen di 1 ma code: bao gom ca loai rf (ev/pt), so luong khung truyen,
    khi goi ham nay thi moi chi gui ma code vao queue, trong rf_transfer_task moi thuc su truyen di
    */
void rf_transfer_code(rf_infoTransfer_t code_info)
{
	 if(codeEv1527Ready == false)
	 {
		 s_infoTransfer = code_info;
		 if(s_infoTransfer.typeDevice == RF_EV1527)
		 {
			log_info("transfer code EV1527: %d\r\n",(int)s_infoTransfer.rfCode);
			codeEv1527Ready = true;
			timer_enableAfter(1000);        //1ms
			rf_transfer_resetStateTransferEv1527();
			PIN_RF_OUT_RESET;
		 }
	 }
	 else log_info("bus busy!!!\r\n");

}
void rf_lightOnTest()
{
    rf_infoTransfer_t  info;
    info.rfCode =  4196677;
    info.typeDevice = RF_EV1527;
    info.timesTransfer = 5;             //cong tac Homatic de = 5
    rf_transfer_code(info);
}
void rf_lightOffTest()
{
    rf_infoTransfer_t  info;
    info.rfCode =  2099525;
    info.typeDevice = RF_EV1527;
    info.timesTransfer = 5;
    rf_transfer_code(info);
}
void rf_transferCodeEv1527(uint32_t code)
{
	rf_infoTransfer_t  info;
	info.rfCode =  code;
	info.typeDevice = RF_EV1527;
	info.timesTransfer = 5;
	rf_transfer_code(info);
}



