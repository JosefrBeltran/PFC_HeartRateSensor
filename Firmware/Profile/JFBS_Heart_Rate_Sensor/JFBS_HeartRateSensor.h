/**************************************************************************************************
  Filename:       JFBS_HeartRateSensor.h
  Revised:        $Date: 2010-08-06 08:56:11 -0700 (Fri, 06 Aug 2010) $
  Revision:       $Revision: 23333 $

  Description:    This file contains the JFBS_HeartRateSensor profile definitions and
                  prototypes.

  Copyright 2010 - 2013 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
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
**************************************************************************************************/

#ifndef JFBS_HEARTRATE_H
#define JFBS_HEARTRATE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
   
   #include "stdint.h"

/*********************************************************************
 * CONSTANTS
 */

// Profile Parameters
#define JFBS_HEARTRATE_CHAR1                       0  // RW uint8 - Profile Characteristic 1 value 
#define JFBS_HEARTRATE_2_MEASURE                   1  // RW uint8 - Profile Characteristic 2 value
#define JFBS_HEARTRATE_2_MEASURE_CHAR_CFG          3  //

  
// UUID for JFBS_heartrate service
#define JFBS_HEARTRATE_SERV_UUID               0xC0C0
/*  The 16 bit UUID listen above is only a part of the 
  * full TI random 128 bit UUID:
  * F000C0C0-0451-4000-B000-000000000000. */

  
// JFBS_Heartrate Service characteristic UUID
#define JFBS_HEARTRATE_CHAR1_UUID                 0xC0C1
#define JFBS_HEARTRATE_2_MEASURE_UUID             0xC0C2
 
 // Callback events
#define HEARTRATE_MEAS_NOTI_ENABLED         1
#define HEARTRATE_MEAS_NOTI_DISABLED        2
#define HEARTRATE_COMMAND_SET               3 

/*********************************************************************
 * TYPEDEFS
 */
// Heart Rate Service callback function
typedef void (*JFBS_heartrateCB_t)(uint8 event);
  
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*JFBS_heartrateChange_t)( uint8 paramID );

typedef struct
{
  JFBS_heartrateChange_t        pfnJFBS_HeartrateChange;  // Called when characteristic value changes
} JFBS_heartrateCBs_t;

    

/*********************************************************************
 * API FUNCTIONS 
 */


/*
 * JFBS_Heartrate_AddService- Initializes the Simple GATT Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */

extern bStatus_t JFBS_Heartrate_AddService( uint32 services );

/*
 * JFBS_Heartrate_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t JFBS_Heartrate_RegisterAppCBs( JFBS_heartrateCB_t *appCallbacks );

/*
 * JFBS_Heartrate_SetParameter - Set a Simple GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t JFBS_Heartrate_SetParameter( uint8 param, uint8 len, void *value );
  
/*
 * JFBS_Heartrate_GetParameter - Get a Simple GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t JFBS_Heartrate_GetParameter( uint8 param, void *value );

/*
 * @fn          HeartRate_HandleConnStatusCB
 *
 * @brief       Heart Rate Service link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none
 */
extern void JFBS_heartrate_HandleConnStatusCB( uint16 connHandle, uint8 changeType );

/*
 * @fn          JFBS_HeartRate_MeasureNotify
 *
 * @brief       Send a notification containing a heart rate
 *              measurement.
 *
 * @param       connHandle - connection handle
 * @param       pNoti - pointer to notification structure
 *
 * @return      Success or Failure
 */
extern bStatus_t JFBS_HeartRate_MeasureNotify( uint16 connHandle, attHandleValueNoti_t *pNoti );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* JFBS_HEARTRATE_H */
