/**************************************************************************************************
  Filename:       app.h
  Revised:        $Date: 2010-08-01 14:03:16 -0700 (Sun, 01 Aug 2010) $
  Revision:       $Revision: 23256 $

  Description:    
**************************************************************************************************/
#if defined ( APP_UART_MODULE )

#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */
#define APP_WAKEUP_INTERRUPT_CHANNEL   HAL_INTERRUPT_CHANNEL_P0_2
#define NPI_UART_BR                    HAL_UART_BR_115200
/*********************************************************************
 * FUNCTIONS
 */

extern void APP_Init( uint8 );
extern uint16 APP_ProcessEvent( uint8, uint16 );
extern void APP_BleCmd_Send( uint8*, uint8 );
extern void (*APP_BleCmd_Receive)(uint8*, uint8);
extern bool (*APP_Info_Set)(uint8*, uint8);
extern void (*APP_Info_Get)(uint8**, uint8*);
extern void APP_Enter_Sleep();
extern void APP_Enter_Wakeup();
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* APP */

#endif