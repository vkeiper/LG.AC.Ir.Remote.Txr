/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
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

/* Includes ------------------------------------------------------------------*/
#include "tim.h"
#include "ir_rmt_txr.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim10;


/* TIM1 init function */
void TIM1_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 755;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    //_Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    //_Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    //_Error_Handler(__FILE__, __LINE__);
  }

}


///* TIM10 init function */
//void TIM10_Init(void)
//{
//  TIM_OC_InitTypeDef sConfigOC;

//  htim10.Instance = TIM10;
//  htim10.Init.Prescaler = 0;
//  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim10.Init.Period = 63167;
//  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
//  {
//    //_Error_Handler(__FILE__, __LINE__);
//		BSP_LED_On(LED4);
//  }

//  if (HAL_TIM_OC_Init(&htim10) != HAL_OK)
//  {
//    //_Error_Handler(__FILE__, __LINE__);
//		BSP_LED_On(LED4);
//  }

//  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
//  sConfigOC.Pulse = 65535;
//  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
//  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
//  if (HAL_TIM_OC_ConfigChannel(&htim10, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
//  {
//    //_Error_Handler(__FILE__, __LINE__);
//  }

//  HAL_TIM_MspPostInit(&htim10);
//	
//}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if(tim_baseHandle->Instance==TIM1)
  {
    /* Peripheral clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();
		
		    /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
		
		/* Enable the timer clock */
		//HAL_TIM_Base_Start_IT(tim_baseHandle);
  }
//		else if(tim_baseHandle->Instance==TIM10)
//  {
//  /* USER CODE BEGIN TIM10_MspInit 0 */

//  /* USER CODE END TIM10_MspInit 0 */
//    /* TIM10 clock enable */
//    __HAL_RCC_TIM10_CLK_ENABLE();

//    /* TIM10 interrupt Init */
//    HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 3, 0);
//    HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
//  /* USER CODE BEGIN TIM10_MspInit 1 */

//  /* USER CODE END TIM10_MspInit 1 */
//  }
}
//void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
//{

//  GPIO_InitTypeDef GPIO_InitStruct;
//  if(timHandle->Instance==TIM10)
//  {
//  /* USER CODE BEGIN TIM10_MspPostInit 0 */

//  /* USER CODE END TIM10_MspPostInit 0 */
//  
//    /**TIM10 GPIO Configuration    
//    PF6     ------> TIM10_CH1 
//    */
//    GPIO_InitStruct.Pin = IR_LED_OUT_Pin;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF3_TIM10;
//    HAL_GPIO_Init(IR_LED_OUT_GPIO_Port, &GPIO_InitStruct);

//  /* USER CODE BEGIN TIM10_MspPostInit 1 */

//  /* USER CODE END TIM10_MspPostInit 1 */
//  }

//}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM10)
  {
  /* USER CODE BEGIN TIM10_MspDeInit 0 */

  /* USER CODE END TIM10_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM10_CLK_DISABLE();

    /* TIM10 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
  /* USER CODE BEGIN TIM10_MspDeInit 1 */

  /* USER CODE END TIM10_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
//void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
		
//}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	  // This callback is automatically called by the HAL on the UEV event
	  static uint8_t state=0;
	
	  if(htim->Instance == TIM1){
//				if(pinmask == 0u){
//					BSP_LED_Off(LED3);
//				}else{
//					BSP_LED_Toggle(LED3);
//				}
//				
				tickcnt++;
				
				/* run state machine*/
				Do_Ir_Rmt_Txr();
	  }
}

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
