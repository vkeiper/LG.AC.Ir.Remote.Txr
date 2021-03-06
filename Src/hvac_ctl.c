/**
  ******************************************************************************
  * File Name          : hvac_ctl.c
  * Description        : This file provides code for the control and monitor 
  *                      of LGAC unit via the reverse engineered IR remote codes
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 VGK AMFT Software Inc.  
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hvac_ctl.h"
#include "10kntc.h"
#include "lcd_log.h"
#include "adc.h"
#include "ir_rmt_txr.h"

#define DBGFROST1
//#define DBGFROST2
#define DBGFROST3  //shows when frost flt is asserted
#define DBGFROST4
#define DBGFROST5
#define DBGTSTAT1
#define DBGTSTAT2
#define DBGTSTAT3
#define DBGACSTATE1

static bool FrostCheck(void);
bool bWarmedUp(float val,float rng);


enum E_ACSTATE{
  EAC_INIT,
  EAC_MANUAL,
  EAC_TSTAT,
  EAC_REMOTE,
  EAC_OFF,
};


struct s_control ctldata_s;
struct s_timetype time_s;
char dbgstr[64];


void DoHvacSimpleMode(void)
{
	static uint32_t t_run,t_retry=0; 
	static bool bModeCool =TRUE;
	
	if(GetTick() > t_run +2000){
			t_run = GetTick();
			if(FrostCheck() && t_retry ==0){
				  bModeCool = FALSE;
				  BSP_LED_On(LED3);/*set FROST ERR LED*/
				  t_retry = GetTick();
					// change to fan only mode so compressor shuts off
				  // this allows frost to melt
				  ctldata_s.bModeChg = 2;

			}else{
				  /* Not in frost error & waiting to turn back on due to a previous frost error*/ 
				  /* 3 minute tiemout 1000mS * 60sec/min * 3min */
				  if(t_retry ==0 || GetTick() > t_retry +  (1000*60*3)){
						    t_retry =0;/*clear to allow for immediate reseed*/
								if(!bModeCool){
									  bModeCool = TRUE;
									  /* change to cool mode after timeout, there are 4 modes so 2 switches  */
										ctldata_s.bModeChg = 2;

								}else{
									  /*AC set point is 65 and no fault */
									  BSP_LED_Off(LED3);/*clear Fault LED*/
								}
					}/* we are in frost error or awaiting retry timer to expire */
					else{
						  /* We are not in frost error */
							/* 3 minute tiemout 1000mS * 60sec/min * 3min */
						  if(ctldata_s.cond_s.rdb >=32 &&
											(t_retry !=0 && (GetTick() < t_retry +  (1000*60*3)))){
										/*AC set point is 65 and no fault */
									  BSP_LED_Toggle(LED3);/*toggle during count down to CLEAR Fault LED*/
#ifdef DBGFROST5
										sprintf(&dbgstr[0],"WarmUp Period %dSecs Left\n",
												(((t_retry +  (1000*60*3))-GetTick())/1000));
										LCD_UsrLog("%s",dbgstr);
#endif
							}						
					}		
			}		
	}
}


//--method to toggle pins for SSR control
static bool FrostCheck(void)
{  
		static uint8_t cnt=0; 
			
    ctldata_s.cond_s.rdbraw = GetAdcConversion(hadc1);/*condensor temp*/
		ctldata_s.set1_s.rdbraw  = GetAdcConversion(hadc3);/*room air*/
		
    
    ctldata_s.cond_s.rdbC = (float)ProcessRT(ctldata_s.cond_s.rdbraw);
    ctldata_s.set1_s.rdbC = (float)ProcessRT(ctldata_s.set1_s.rdbraw);
    
    ctldata_s.cond_s.rdb = ctldata_s.cond_s.rdbC* 9/5 + 32;
    ctldata_s.set1_s.rdb = ctldata_s.set1_s.rdbC* 9/5 + 32;
    
    #ifdef DBGFROST2
				sprintf(&dbgstr[0],"RM %d %2.0fdegC %2.0fdegF\n",
            ctldata_s.set1_s.rdbraw,ctldata_s.set1_s.rdbC, ctldata_s.set1_s.rdb);
        LCD_UsrLog("%s",dbgstr);
		    
        sprintf(&dbgstr[0],"CD %d %2.2fdegC %2.2fdegF\n",
            ctldata_s.cond_s.rdbraw,ctldata_s.cond_s.rdbC, ctldata_s.cond_s.rdb);
        LCD_UsrLog("%s",dbgstr);
		#endif
		
		sprintf(&dbgstr[0],"Rm %2.0fF Cd %2.0fF",
            ctldata_s.set1_s.rdb,ctldata_s.cond_s.rdb );
		LCD_LOG_SetHeader((uint8_t*)&dbgstr);
    
  
  if( ctldata_s.cond_s.rdb <32 ){
    #ifdef DBGFROST3
        LCD_DbgLog("FROST EMINENT %d \n",cnt);
    #endif
		if(cnt++>=5){
			cnt=0;
			ctldata_s.bFrostErr = TRUE;
		}
		
  }else{
    //If we were in Frost Fault call to test for 
    if(ctldata_s.bFrostErr == TRUE){
				/*decrement fault count*/
				if(cnt)
						cnt--;
				/*test fault count for clear*/
				if(cnt==0){
					ctldata_s.bFrostErr = (bWarmedUp(ctldata_s.cond_s.rdb,ctldata_s.cond_s.rnghi) == TRUE ? FALSE : TRUE );
#ifdef DBGFROST4
					LCD_UsrLog("NO FROST count%d\n",cnt);
#endif
				}
    }
    
  }
	
	return ctldata_s.bFrostErr;
}


//--method to restart the AC after warmup period
bool bWarmedUp(float val,float rng)
{
   bool retval = FALSE;
   //build in hysterisis for turn back on
    if( val >rng){
        retval = TRUE;
        LCD_UsrLog("No Frost >HYST");
    }else{
      retval = FALSE;
        LCD_UsrLog("CONDENSER <32F Hold Off");
    }

    return retval;
}

void SetAcState(void)
{
	  static enum E_ACSTATE state = EAC_INIT;
	  //char LogEntry[64];

    //-- read tstat demand for tstat demanded cool state
    if (HAL_GPIO_ReadPin(DI_DMD_GPIO_Port,DI_DMD_Pin) == GPIO_PIN_RESET && 
			HAL_GPIO_ReadPin(DI_MANMODE_GPIO_Port,DI_MANMODE_Pin) == GPIO_PIN_RESET)
    
    {
      ctldata_s.bTstatCoolDmd = TRUE;
    }else{
      ctldata_s.bTstatCoolDmd = FALSE;
    }
    
    //#ifdef DBGACSTATE1
        //--status dbg print
        LCD_UsrLog("MAN AUX %d MAN AC %d pinSTAT %d TSTAT VAR %d FRST %d \n",
						(uint8_t)ctldata_s.manstate_s.auxfan,
						(uint8_t)ctldata_s.manstate_s.acpump,
						HAL_GPIO_ReadPin(DO_CIRC_SSR_GPIO_Port,DO_CIRC_SSR_Pin) == GPIO_PIN_SET ? 1 : 0,
            (uint8_t)ctldata_s.bTstatCoolDmd,
            (uint8_t)ctldata_s.bFrostErr);
    //#endif

    // Test if mode change is required and do it
    if( ctldata_s.ctlmode_e == ECTLMD_MANUAL ){
      state = EAC_MANUAL;
    }else if( ctldata_s.ctlmode_e == ECTLMD_TSTAT ){
       state = EAC_TSTAT;
       ctldata_s.manstate_s.acpump = FALSE;
       ctldata_s.manstate_s.auxfan = FALSE;
    }else if( ctldata_s.ctlmode_e == ECTLMD_REMOTE ){
       state = EAC_REMOTE;
       ctldata_s.manstate_s.acpump = FALSE;
       ctldata_s.manstate_s.auxfan = FALSE;
    }else{
       state = EAC_OFF;
       ctldata_s.manstate_s.acpump = FALSE;
       ctldata_s.manstate_s.auxfan = FALSE;
    }

    // State MAchine for AC PUMP & AUXFAN for all modes
    switch(state){
        case EAC_INIT:
            state = EAC_TSTAT;
            #ifdef DBGACSTATE1   
              LCD_UsrLog("[AC STATE] INIT\n");
            #endif
        break;

        case EAC_MANUAL:
            // Set ac pump state
            if( ctldata_s.manstate_s.acpump == TRUE){ 
                HAL_GPIO_WritePin(DO_MAINS_SSR_GPIO_Port,DO_MAINS_SSR_Pin,GPIO_PIN_SET);
            }else{
                HAL_GPIO_WritePin(DO_MAINS_SSR_GPIO_Port,DO_MAINS_SSR_Pin,GPIO_PIN_RESET);
            }
            
            //Set auxfan ON state
            if( ctldata_s.manstate_s.auxfan == TRUE){ 
                HAL_GPIO_WritePin(DO_CIRC_SSR_GPIO_Port,DO_CIRC_SSR_Pin,GPIO_PIN_SET);;
            }else{
                HAL_GPIO_WritePin(DO_CIRC_SSR_GPIO_Port,DO_CIRC_SSR_Pin,GPIO_PIN_RESET);;
            }

            #ifdef DBGACSTATE1  
                LCD_UsrLog("[AC STATE] MANUAL\n");
            #endif
        break;

        case EAC_TSTAT:
            //Test For ACMAIN ON state
            if(ctldata_s.bTstatCoolDmd == TRUE && ctldata_s.bFrostErr == FALSE){
                HAL_GPIO_WritePin(DO_MAINS_SSR_GPIO_Port,DO_MAINS_SSR_Pin,GPIO_PIN_SET);
                
            }else if(ctldata_s.bTstatCoolDmd == FALSE  || ctldata_s.bFrostErr == TRUE){
                HAL_GPIO_WritePin(DO_MAINS_SSR_GPIO_Port,DO_MAINS_SSR_Pin,GPIO_PIN_RESET);
            }
            // Test for AUXFAN ON state
            if(ctldata_s.bTstatCoolDmd == TRUE || ctldata_s.bFrostErr == TRUE){
              HAL_GPIO_WritePin(DO_CIRC_SSR_GPIO_Port,DO_CIRC_SSR_Pin,GPIO_PIN_SET);;
            }
            else if( ctldata_s.bTstatCoolDmd == FALSE  && ctldata_s.bFrostErr == FALSE){
                HAL_GPIO_WritePin(DO_CIRC_SSR_GPIO_Port,DO_CIRC_SSR_Pin,GPIO_PIN_RESET);;
            }
            #ifdef DBGACSTATE1   
                LCD_UsrLog("[AC STATE] TSTAT\n");
            #endif
        break;
        
        case EAC_REMOTE:
            // AUXFAN always on in remote mode
            HAL_GPIO_WritePin(DO_CIRC_SSR_GPIO_Port,DO_CIRC_SSR_Pin,GPIO_PIN_SET);;  
            
            // Test if pump should be turned on
            // If NO frost_flt && AC pump off && temp goes 2deg > demand turn pump on
            if( !ctldata_s.bFrostErr && 
                (ctldata_s.set1_s.rdb >= ctldata_s.set1_s.dmd + ctldata_s.set1_s.rnghi && 
                (HAL_GPIO_ReadPin(DO_MAINS_SSR_GPIO_Port,DO_MAINS_SSR_Pin) == GPIO_PIN_RESET))){
									
								HAL_GPIO_WritePin(DO_MAINS_SSR_GPIO_Port,DO_MAINS_SSR_Pin,GPIO_PIN_SET);
							//LogEntry = String(time_s.str) + " [AC STATE] REMOTE DMD PUMPON " + String(ctldata_s.set1_s.dmd) + " RDB " + String(ctldata_s.set1_s.rdb) +
							//  " RNGLO " + String(ctldata_s.set1_s.rnglo) + " RNGH " + String(ctldata_s.set1_s.rnghi) +"\n";
							//LogEntry.toCharArray(dbgstr, sizeof(dbgstr));
							//Serial.println(dbgstr);
						 // LOG pump cycle to SD card
			      }

            //Test if pump should be turn off
            // If frost_flt OR AC pump on and temp goes 2deg > demand turn pump on
            if( ctldata_s.bFrostErr || 
                (ctldata_s.set1_s.rdb <= ctldata_s.set1_s.dmd - ctldata_s.set1_s.rnglo && 
                (HAL_GPIO_ReadPin(DO_MAINS_SSR_GPIO_Port,DO_MAINS_SSR_Pin) == GPIO_PIN_SET))){
              HAL_GPIO_WritePin(DO_MAINS_SSR_GPIO_Port,DO_MAINS_SSR_Pin,GPIO_PIN_RESET);
//							LogEntry = String(time_s.str) + " [AC STATE] REMOTE DMD PUMPOFF " + String(ctldata_s.set1_s.dmd) + " RDB " + String(ctldata_s.set1_s.rdb) +
//								" RNGLO " + String(ctldata_s.set1_s.rnglo) + " RNGH " + String(ctldata_s.set1_s.rnghi) + "\n";
//							LogEntry.toCharArray(dbgstr, sizeof(dbgstr));
//							Serial.println(dbgstr);
			 
			  
            }   
            
        break;

        case EAC_OFF:
            HAL_GPIO_WritePin(DO_CIRC_SSR_GPIO_Port,DO_CIRC_SSR_Pin,GPIO_PIN_RESET);;  
            HAL_GPIO_WritePin(DO_MAINS_SSR_GPIO_Port,DO_MAINS_SSR_Pin,GPIO_PIN_RESET);
            #ifdef DBGACSTATE1   
                LCD_UsrLog("[AC STATE] OFF\n");
            #endif
        break;
        
        default:
        break;
    };

    //set vars indicating pin states
    ctldata_s.bAcCooling = HAL_GPIO_ReadPin(DO_MAINS_SSR_GPIO_Port,DO_MAINS_SSR_Pin) == GPIO_PIN_SET ? TRUE : FALSE ;
    ctldata_s.bAuxFan = HAL_GPIO_ReadPin(DO_CIRC_SSR_GPIO_Port,DO_CIRC_SSR_Pin) == GPIO_PIN_SET ? TRUE : FALSE ;
}

void calc_uptime(uint32_t time)
{
	time_s.tm_secs = time % 60UL;
	time /= 60UL;
	time_s.tm_mins = time % 60UL;
	time /= 60UL;
	time_s.tm_hrs = time % 24UL;
	time /= 24UL;
	time_s.tm_days = time;

	sprintf(time_s.str, "Dd:Hh:Mm:Ss %2d:%2d:%2d:%2d", time_s.tm_days, time_s.tm_hrs, time_s.tm_mins, time_s.tm_secs);
	LCD_LOG_SetFooter((uint8_t*)&time_s.str);
}


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT VGK AMFT Software Inc *****END OF FILE****/
