/******************************************************************************

 @file  hal_key.c

 @brief This file contains the interface to the HAL KEY Service.

 Group: WCS, BTS
 Target Device: CC2540, CC2541

 ******************************************************************************
 
 Copyright (c) 2006-2016, Texas Instruments Incorporated
 All rights reserved.

 IMPORTANT: Your use of this Software is limited to those specific rights
 granted under the terms of a software license agreement between the user
 who downloaded the software, his/her employer (which must be your employer)
 and Texas Instruments Incorporated (the "License"). You may not use this
 Software unless you agree to abide by the terms of the License. The License
 limits your use, and you acknowledge, that the Software may not be modified,
 copied or distributed unless embedded on a Texas Instruments microcontroller
 or used solely and exclusively in conjunction with a Texas Instruments radio
 frequency transceiver, which is integrated into your product. Other than for
 the foregoing purpose, you may not use, reproduce, copy, prepare derivative
 works of, modify, distribute, perform, display or sell this Software and/or
 its documentation for any purpose.

 YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
 PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
 NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
 TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
 NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
 LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
 INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
 OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
 OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

 Should you have any questions regarding your right to use this Software,
 contact Texas Instruments Incorporated at www.TI.com.

 ******************************************************************************
 Release Name: ble_sdk_1.4.2.2
 Release Date: 2016-06-09 06:57:09
 *****************************************************************************/
/*********************************************************************
 NOTE: If polling is used, the hal_driver task schedules the KeyRead()
       to occur every 100ms.  This should be long enough to naturally
       debounce the keys.  The KeyRead() function remembers the key
       state of the previous poll and will only return a non-zero
       value if the key state changes.

 NOTE: If interrupts are used, the KeyRead() function is scheduled
       25ms after the interrupt occurs by the ISR.  This delay is used
       for key debouncing.  The ISR disables any further Key interrupt
       until KeyRead() is executed.  KeyRead() will re-enable Key
       interrupts after executing.  Unlike polling, when interrupts
       are enabled, the previous key state is not remembered.  This
       means that KeyRead() will return the current state of the keys
       (not a change in state of the keys).

 NOTE: If interrupts are used, the KeyRead() fucntion is scheduled by
       the ISR.  Therefore, the joystick movements will only be detected
       during a pushbutton interrupt caused by S1 or the center joystick
       pushbutton.

 NOTE: When a switch like S1 is pushed, the S1 signal goes from a normally
       high state to a low state.  This transition is typically clean.  The
       duration of the low state is around 200ms.  When the signal returns
       to the high state, there is a high likelihood of signal bounce, which
       causes a unwanted interrupts.  Normally, we would set the interrupt
       edge to falling edge to generate an interrupt when S1 is pushed, but
       because of the signal bounce, it is better to set the edge to rising
       edge to generate an interrupt when S1 is released.  The debounce logic
       can then filter out the signal bounce.  The result is that we typically
       get only 1 interrupt per button push.  This mechanism is not totally
       foolproof because occasionally, signal bound occurs during the falling
       edge as well.  A similar mechanism is used to handle the joystick
       pushbutton on the DB.  For the EB, we do not have independent control
       of the interrupt edge for the S1 and center joystick pushbutton.  As
       a result, only one or the other pushbuttons work reasonably well with
       interrupts.  The default is the make the S1 switch on the EB work more
       reliably.

*********************************************************************/

/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/

#include "hal_types.h"
#include "hal_key.h"
#include "osal.h"

/***************************************************************************************************
 *                                              TYPEDEFS
 ***************************************************************************************************/

typedef struct {
  bool Inited;
  uint8 Port;
} HAL_TYPE_KEY_CONFIG;

#define HAL_KEY_P0_INIT(__Idx__) st(P0SEL &= ~BV(__Idx__);P0DIR &= ~BV(__Idx__); P0INP &= ~BV(__Idx__);)
#define HAL_KEY_P1_INIT(__Idx__) st(P1SEL &= ~BV(__Idx__);P1DIR &= ~BV(__Idx__); P1INP &= ~BV(__Idx__);)


#define HAL_KEY_P0_SET(__Idx__, __STATUS__) st(if(__STATUS__) { P0 |= BV(__Idx__); } else { P0 &= ~BV(__Idx__); })
#define HAL_KEY_P1_SET(__Idx__,__STATUS__) st(if(__STATUS__) { P1 |= BV(__Idx__); } else { P1 &= ~BV(__Idx__); })

#define HAL_KEY_P0_GET(__Idx__) ((P0 >> __Idx__) & 0x01 == 0x01)
#define HAL_KEY_P1_GET(__Idx__) ((P1 >> __Idx__) & 0x01 == 0x01)

/***************************************************************************************************
 *                                           GLOBAL VARIABLES
 ***************************************************************************************************/

HAL_TYPE_KEY_CONFIG HAL_KEY_CONFIG[HAL_KEY_CHANNEL_NUM]  = {
  {.Inited = false, .Port = 0x00 },
  {.Inited = false, .Port = 0x01 },
  {.Inited = false, .Port = 0x02 },
  {.Inited = false, .Port = 0x03 },
  {.Inited = false, .Port = 0x04 },
  {.Inited = false, .Port = 0x05 },
  {.Inited = false, .Port = 0x06 },
  {.Inited = false, .Port = 0x07 },
  {.Inited = false, .Port = 0x10 },
  {.Inited = false, .Port = 0x11 },
  {.Inited = false, .Port = 0x12 },
  {.Inited = false, .Port = 0x13 },
  {.Inited = false, .Port = 0x14 },
  {.Inited = false, .Port = 0x15 },
  {.Inited = false, .Port = 0x16 },
  {.Inited = false, .Port = 0x17 }
};
/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/


/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      HalKeyGet
 *
 * @brief   
 *
 * @param   
 * @return  
 ***************************************************************************************************/
bool HalKeyGet (uint8 Idx, bool* Status)
{
  if(Idx >= HAL_KEY_CHANNEL_NUM) return false;
  uint8 Port = HAL_KEY_CONFIG[Idx].Port;
  uint8 Portx = Port >> 4;
  uint8 Portx_x = Port & 0x0F;
  uint8 s = BV(Portx_x);
  switch(Portx){
    case 0:
      if(HAL_KEY_CONFIG[Idx].Inited == false){
        P0DIR &= ~s; 
        P0INP &= ~s;
        P0 |= s;
        HAL_KEY_CONFIG[Idx].Inited = true;
      }
      *Status = ((P0 >> Portx_x) & 0x01);
      break;
    case 1:
      if(HAL_KEY_CONFIG[Idx].Inited == false){
        P1DIR &= ~s; 
        P1INP &= ~s;
        P1 |= s;
        HAL_KEY_CONFIG[Idx].Inited = true;
      }
      *Status = ((P1 >> Portx_x) & 0x01);
      break;
  }
  return true;
}

/***************************************************************************************************
***************************************************************************************************/