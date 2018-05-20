/**
  ******************************************************************************
  * @file    Src/ir_rmt_txr.c 
  * @author  Vince Keiper
  * @version V1.0.0
  * @date    12-May-2018 
  * @brief   This app transmits commands to an LG AC unit
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lcd_log.h"
#include "ir_rmt_txr.h"

/** @addtogroup VGK_Apps
  * @{
  */

/** @addtogroup APP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




//typedef enum Ranks {FIRST, SECOND} Order;
//Order data = (Order)20;  // Must cast to defined type to prevent error

#define INITTX 0u
#define SOFTX 1u
#define TXDATA 2u
#define WAITTX 3u
#define EOFTX   4u
#define DONETX 5u

char dispstr[16];

typedef struct modrc5_frames_t{
   char title[32];
   const uint8_t adr1;
   const uint8_t adr2;
   const uint8_t cmd;
   const uint8_t chk;
   const uint16_t sofOn;
   const uint16_t sofOff;
   const uint16_t eofOn;
   const uint8_t  onmltp;/*A "1" off time is the pulse time multiplied by 3*/    
} ModRc5Frame_t;

ModRc5Frame_t FramePwr = {
    "Power-ON/OFF",
    0x81,
    0x66,
    0x81,/*Power ON\OFF, each tx rotates on\off state*/
    (uint8_t)~0x81,
    9000u,/*9mS 9000uS*/
    4500u,/*4.5mS 4500uS*/
    564u,/*pulse length*/
    3u
}; 
ModRc5Frame_t FrameMode = {
    "Power-ON/OFF",
    0x81,
    0x66,
    0xD9,/*Mode, each tx rotates mode */
    (uint8_t)~0xD9,
    9000u,/*9mS 9000uS*/
    4500u,/*4.5mS 4500uS*/
    564u,/*pulse length*/
    3u
}; 
ModRc5Frame_t FrameTempUp = {
    "Temp +",
    0x81,
    0x66,
    0xAE,/*Temperature setpoint increase */
    (uint8_t)~0xAE,
    9000u,/*9mS 9000uS*/
    4500u,/*4.5mS 4500uS*/
    564u,/*pulse length*/
    3u
}; 
ModRc5Frame_t FrameTempDn = {
    "Temp -",
    0x81,
    0x66,
    0x51,/*Temperature setpoint decrease */
    (uint8_t)~0x51,
    9000u,/*9mS 9000uS*/
    4500u,/*4.5mS 4500uS*/
    564u,/*pulse length*/
    3u
}; 
ModRc5Frame_t FrameFanSpd = {
    "Fan Speed ",
    0x81,
    0x66,
    0x99,/*Fan Speed rotate */
    (uint8_t)~0x99,
    9000u,/*9mS 9000uS*/
    4500u,/*4.5mS 4500uS*/
    564u,/*pulse length*/
    3u
}; 

ModRc5Frame_t FrameTimer = {
    "NOT SET",
    0x81,
    0x66,
    0x00,
    (uint8_t)~0x00,
    9000u,/*9mS 9000uS*/
    4500u,/*4.5mS 4500uS*/
    564u,/*pulse length*/
    3u
}; 

/*Init an array of pointers to all the frame objects */
ModRc5Frame_t *pFrames[] = {
    &FramePwr,
    &FrameMode,
    &FrameTempUp,
    &FrameTempDn,
    &FrameFanSpd,
    &FrameTimer
};    



#define FRMPWR   0u
#define FRMMODE  1u
#define FRMTMPUP 2u
#define FRMTMPDN 3u
#define FRMFAN   4u



/**
  * @brief  Run IR rmt state machine 
  * @param  None
  * @retval None
  */
void Do_Ir_Rmt_Txr(void)
{ 
  uint8_t ADR1 = 0x81, ADR2 = 0x66, CMDID = 0x81, CHK = ~CMDID;
  static uint16_t bitsSent=0;
  static uint8_t FrmIdx =0;
    
  ADR1 = (uint8_t)pFrames[FrmIdx]->adr2;
  
  /*build data tx frame*/
  uint32_t DataToTx = 0u;
  DataToTx |= ADR1<<24; 
  DataToTx |= ADR2<<16;
  DataToTx |= CMDID<<8; 
  DataToTx |= CHK;
    
  //enum IrStates {INIT,SOF,DATA,WAIT,EOF,DONE};
  //enum IrStates IrState;  

  static uint8_t TxState = INITTX,lastState;
	
	static uint32_t t_run=0;
    
	
	if(HAL_GetTick() < t_run +250){
		return;
	}else{
		t_run = HAL_GetTick();
	}
	
	
  switch(TxState){

      case INITTX:
          
          /* Initialize LCD Log module */
          LCD_LOG_Init();
          
          /* Show Header and Footer texts */
          LCD_LOG_SetHeader((uint8_t*)"IR Remote Tx'r");
          //sprintf(dispstr,"%4s Ver %d.%d.%d %d/%d/%d %d:%d",AUTHRABR,MAJVER,
          //          MINVER,BLDVER,YERVER,MONVER,DAYVER,HRSVER,MNSVER);
          //LCD_LOG_SetFooter((uint8_t*)dispstr);
          //LCD_UsrLog ("Author %s \n",AUTHRFULL);
      
          TxState = SOFTX;
			
          //HAL_TIM_Base_Start_IT(&htim1); //Start the timer
					//HAL_TIM_Base_Start_IT(&htim10); //Start the timer

          
      break;
      
      case SOFTX:
        LCD_UsrLog ("TX SOF \n" );
        LCD_UsrLog ("DATA= 0x%8x \n",DataToTx );
        
      //TODO:/* Setup TMR to send 38Khz carrier for 9mS then pause for 4.5mS */
      
        TxState = TXDATA;
      
    
      break;
      
      case TXDATA:
        LCD_UsrLog ("TX DATA QTY:%d  BIT=%d  \n",bitsSent,(DataToTx<<bitsSent & 0x80000000) ? 1 : 0 );
      //TODO: Add code to send carrier for 564uS then test bit and if 1 pause 564uS else pause 564uS*3  
      
        bitsSent++;
        lastState = TxState;
      
        if(bitsSent >31){
            TxState = EOFTX;
        }else{
            TxState = WAITTX;
        }
      
          //BSP_LED_Toggle(LED3);
      break;
      /** The WAIT occurs in each state, the state before kicks off the process of sending the carrier
       *  freq for required amount of time. Then we come back to wait for the pause time depending on state 
       *  and OR each bit value within the TXDATA state
       */
      case WAITTX:
        LCD_UsrLog ("WAIT DATA to TX \n" );
        TxState = lastState;
        
      break;
      
      case EOFTX:
          LCD_UsrLog ("TX EOF \n" );
          TxState = DONETX;
        
          
      break;
      
      case DONETX:
        /* Clear Old logs */
        //LCD_LOG_ClearTextZone();
        TxState = SOFTX;
        bitsSent=0;
        FrmIdx=0;
      
      break;
      

      default:
      break;
      
  }      
    
  
  
}

/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
