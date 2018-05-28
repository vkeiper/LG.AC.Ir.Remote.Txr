/**
  ******************************************************************************
  * @file    BSP/Inc/main.h 
  * @author  MCD Application Team
  * @version V1.2.8
  * @date    17-February-2017 
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_ts.h"
#include "stm32f429i_discovery_io.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_gyroscope.h"
#ifdef EE_M24LR64
#include "stm32f429i_discovery_eeprom.h"
#endif /*EE_M24LR64*/

#define MAJVER 0u
#define MINVER 0u
#define BLDVER 0u
#define YERVER 18u
#define MONVER 05u
#define DAYVER 14u
#define HRSVER 00u
#define MNSVER 52u
#define AUTHRFULL "Vince Keiper"
#define AUTHRABR "V.K. "



#define AIN_TEMP_ROOM_Pin GPIO_PIN_6
#define AIN_TEMP_ROOM_GPIO_Port GPIOF
#define AIN_TEMP_ACOIL_Pin GPIO_PIN_5
#define AIN_TEMP_ACOIL_GPIO_Port GPIOA


#define DI_MANMODE_Pin GPIO_PIN_2
#define DI_MANMODE_GPIO_Port GPIOE
#define DI_DMD_Pin GPIO_PIN_3
#define DI_DMD_GPIO_Port GPIOE
#define DO_MAINS_SSR_Pin GPIO_PIN_4
#define DO_MAINS_SSR_GPIO_Port GPIOE
#define DO_CIRC_SSR_Pin GPIO_PIN_5
#define DO_CIRC_SSR_GPIO_Port GPIOE
#define DO_EVAP_SSR_Pin GPIO_PIN_6
#define DO_EVAP_SSR_GPIO_Port GPIOE
#define DO_IRLED_Pin GPIO_PIN_2
#define DO_IRLED_GPIO_Port GPIOD
#define IRLED_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOD_CLK_ENABLE()  
#define IRLED_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOD_CLK_DISABLE()  


/* Exported types ------------------------------------------------------------*/
typedef struct
{
  void   (*DemoFunc)(void);
  uint8_t DemoName[50]; 
  uint32_t DemoIndex;
}BSP_DemoTypedef;

extern const unsigned char stlogo[];
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define COUNT_OF_EXAMPLE(x)    (sizeof(x)/sizeof(BSP_DemoTypedef))
/* Exported functions ------------------------------------------------------- */
void Joystick_demo (void);
void Touchscreen_demo (void);
void LCD_demo (void);
void MEMS_demo (void);
void Log_demo(void);
#ifdef EE_M24LR64
void EEPROM_demo (void);
#endif /*EE_M24LR64*/
void SD_demo (void);
void Touchscreen_Calibration (void);
uint16_t Calibration_GetX(uint16_t x);
uint16_t Calibration_GetY(uint16_t y);
uint8_t IsCalibrationDone(void);
uint8_t CheckForUserInput(void);
void Toggle_Leds(void);

//void _Error_Handler(char *, int);

//#define Error_Handler() _Error_Handler(__FILE__, __LINE__)


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
