/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define LED_MCU_Pin GPIO_PIN_13
#define LED_MCU_GPIO_Port GPIOC
#define CAP_IR_Pin GPIO_PIN_0
#define CAP_IR_GPIO_Port GPIOA
#define PHOTO_SEN_Pin GPIO_PIN_3
#define PHOTO_SEN_GPIO_Port GPIOA
#define DATA_RF_Pin GPIO_PIN_4
#define DATA_RF_GPIO_Port GPIOA
#define A0_138_Pin GPIO_PIN_5
#define A0_138_GPIO_Port GPIOA
#define A1_138_Pin GPIO_PIN_6
#define A1_138_GPIO_Port GPIOA
#define A2_138_Pin GPIO_PIN_7
#define A2_138_GPIO_Port GPIOA
#define DS595_Pin GPIO_PIN_0
#define DS595_GPIO_Port GPIOB
#define STCP_595_Pin GPIO_PIN_1
#define STCP_595_GPIO_Port GPIOB
#define SHCP_595_Pin GPIO_PIN_10
#define SHCP_595_GPIO_Port GPIOB
#define BTN_MODE_Pin GPIO_PIN_12
#define BTN_MODE_GPIO_Port GPIOB
#define BTN_PLUS_Pin GPIO_PIN_13
#define BTN_PLUS_GPIO_Port GPIOB
#define BTN_MINUS_Pin GPIO_PIN_14
#define BTN_MINUS_GPIO_Port GPIOB
#define BTN_OK_Pin GPIO_PIN_15
#define BTN_OK_GPIO_Port GPIOB
#define LED2_RED_Pin GPIO_PIN_3
#define LED2_RED_GPIO_Port GPIOB
#define LED2_GRE_Pin GPIO_PIN_4
#define LED2_GRE_GPIO_Port GPIOB
#define LED2_BLU_Pin GPIO_PIN_5
#define LED2_BLU_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_9
#define BUZZER_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */
void SystemManagementTask(void);
/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
