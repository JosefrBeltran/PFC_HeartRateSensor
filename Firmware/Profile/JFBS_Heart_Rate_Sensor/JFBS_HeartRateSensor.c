/**************************************************************************************************
  Filename:       JFBS_HeartRateSensor.c
  Revised:        $Date: 2013-05-06 13:33:47 -0700 (Mon, 06 May 2013) $
  Revision:       $Revision: 34153 $

  Description:    This file contains the Simple GATT profile sample GATT service 
                  profile for use with the BLE sample application.

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

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#include "JFBS_HeartRateSensor.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SERVAPP_NUM_ATTR_SUPPORTED        8
#define JFBS_HEARTRATE_MEASURE_VALUE_POS  5  // Position of heart rate measurement value in attribute array

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// JFBS_Heartrate Service UUID: 0xC0C0                     
CONST uint8 JFBS_heartrateServUUID[ATT_UUID_SIZE] =         
{                                                     
    TI_BASE_UUID_128(JFBS_HEARTRATE_SERV_UUID ),             
};                                                    
                                                      
// Characteristic 1 UUID: 0xC0C1                      
CONST uint8 JFBS_heartratechar1UUID[ATT_UUID_SIZE] =        
{                                                     
    TI_BASE_UUID_128(JFBS_HEARTRATE_CHAR1_UUID ),            
};                                                    
// Characteristic 2 MEASURE VALUE UUID: 0xC0C2                      
CONST uint8 JFBS_heartratechar2UUID[ATT_UUID_SIZE] =        
{                                                     
    TI_BASE_UUID_128(JFBS_HEARTRATE_2_MEASURE_UUID ),            
};     

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static JFBS_heartrateCB_t *JFBS_heartrateCB = NULL;

/*********************************************************************
 * Service Attributes - variables
 */

// JFBS_Heartrate Service attribute                                                             
static CONST gattAttrType_t JFBS_heartrateService = { ATT_UUID_SIZE, JFBS_heartrateServUUID };     
                                                                                          
// JFBS_Heartrate Service Characteristic 1 Properties                                           
static uint8 JFBS_heartrateChar1Props = GATT_PROP_READ | GATT_PROP_WRITE;                       
static uint8 JFBS_heartrateChar1 = 0;     // Characteristic 1 Value                                                                                         
static uint8 JFBS_heartrateChar1UserDesp[22] = "JFBS_Heartrate Value\0";    //Service Characteristic 1 User Description                                                            
                                                                                                                           
                                                                                          
// JFBS_Heartrate  Characteristic 2   MEASURE VALUE                                          
static uint8 JFBS_heartrateChar2Props = GATT_PROP_NOTIFY;    //Service Characteristic 2 Properties                                                                                                                                                                                          
static uint8 JFBS_heartrateChar2 = 0;            // Characteristic 2 Value                                               
static gattCharCfg_t JFBS_heartrateChar2Config[GATT_MAX_NUM_CONN];  // Service Characteristic 2 Configuration.                                                                                        
static uint8 JFBS_heartrateChar2UserDesp[] = "JFBS_Heartrate Value Notification\0"; //Service Characteristic 2 User Description                                         
                                                                                                                            


/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t JFBS_heartrateAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = 
{
  // JFBS_Heartrate Service
  {                                                               
    { ATT_BT_UUID_SIZE, primaryServiceUUID },   /* type */        
    GATT_PERMIT_READ,                           /* permissions */ 
    0,                                          /* handle */      
    (uint8 *)&JFBS_heartrateService                   /* pValue */      
  },                                                              
  // Characteristic 1 Declaration                                 
  {                                                               
    { ATT_BT_UUID_SIZE, characterUUID },                          
    GATT_PERMIT_READ ,                                             
    0,                                                            
    &JFBS_heartrateChar1Props                                           
  },                                                              
                                                                  
  // Characteristic Value 1                                       
  {                                                               
    { ATT_UUID_SIZE, JFBS_heartratechar1UUID },                         
    GATT_PERMIT_READ | GATT_PERMIT_WRITE,                         
    0,                                                            
    &JFBS_heartrateChar1                                                
  },                                                              
                                                                  
  // Characteristic 1 User Description                            
  {                                                               
    { ATT_BT_UUID_SIZE, charUserDescUUID },                       
    GATT_PERMIT_READ,                                             
    0,                                                            
    JFBS_heartrateChar1UserDesp                                         
  },                                                              
                                                                  
                                                                  
  // Characteristic 2 - MEASURE Value - Declaration                                 
  {                                                               
    { ATT_BT_UUID_SIZE, characterUUID },                          
    GATT_PERMIT_READ,                                             
    0,                                                            
    &JFBS_heartrateChar2Props                                           
  },                                                              
                                                                  
  // Characteristic 2 MEASURE Value                                       
  {                                                               
    { ATT_UUID_SIZE, JFBS_heartratechar2UUID },                         
    0,                                                            
    0,                                                            
    &JFBS_heartrateChar2                                                
  },                                                              
                                                                  
  // Characteristic 2 MEASURE configuration                               
  {                                                               
    { ATT_BT_UUID_SIZE, clientCharCfgUUID },                      
    GATT_PERMIT_READ | GATT_PERMIT_WRITE,                         
    0,                                                            
    (uint8 *) &JFBS_heartrateChar2Config                                 
  },                                                              
                                                                  
  // Characteristic 2 MEASURE User Description                            
  {                                                               
    { ATT_BT_UUID_SIZE, charUserDescUUID },                       
    GATT_PERMIT_READ,                                             
    0,                                                            
    JFBS_heartrateChar2UserDesp      
  },         
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static uint8 JFBS_heartrate_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen );
static bStatus_t JFBS_heartrate_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset );
static void JFBS_heartrate_HandleConnStatusCB( uint16 connHandle, uint8 changeType );


/*********************************************************************
 * PROFILE CALLBACKS
 */
// JFBS_HeartRateSensor Service Callbacks
CONST gattServiceCBs_t JFBS_heartrateCBs =
{
  JFBS_heartrate_ReadAttrCB,  // Read callback function pointer
  JFBS_heartrate_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */



bStatus_t utilExtractUuid16 ( gattAttribute_t *pAttr, uint16_t *pUuid )
  {
    bStatus_t status = SUCCESS;

    if (pAttr->type.len == ATT_BT_UUID_SIZE )
    {
      // 16-bit UUID direct
      *pUuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
  #ifdef GATT_TI_UUID_128_BIT
    }
    else if (pAttr->type.len == ATT_UUID_SIZE)
    {
      // 16-bit UUID extracted bytes 12 and 13
      *pUuid = BUILD_UINT16( pAttr->type.uuid[12], pAttr->type.uuid[13]);
  #endif
    } else {
      *pUuid = 0xFFFF;
      status = FAILURE;
    }

    return status;
  }

/*********************************************************************
 * @fn      JFBS_Heartrate_AddService
 *
 * @brief   Initializes the JFBS_Heartrate service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */

bStatus_t JFBS_Heartrate_AddService( uint32 services )                                      
  {                                                                                     
    uint8 status;                                                                       
    
    // Initialize Client Characteristic Configuration attributes                        
    GATTServApp_InitCharCfg( INVALID_CONNHANDLE, JFBS_heartrateChar2Config );  
                                                                                                            
                                                                                        
    // Register GATT attribute list and CBs with GATT Server App                        
    status = GATTServApp_RegisterService( JFBS_heartrateAttrTbl,                              
                                          GATT_NUM_ATTRS( JFBS_heartrateAttrTbl ),            
                                          &JFBS_heartrateCBs );                                                                                                                       
    return ( status );                                                                  
  }            

/*********************************************************************
 * @fn      JFBS_Heartrate_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t JFBS_Heartrate_RegisterAppCBs( JFBS_heartrateCB_t *appCallbacks )
{
  if ( appCallbacks )
  {
    JFBS_heartrateCB = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

  
/*********************************************************************
 * @fn      JFBS_Heartrate_SetParameter
 *
 * @brief   Set a JFBS_Heartrate parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t JFBS_Heartrate_SetParameter( uint8 param, uint8 len, void *value )                       
{                                                                                             
  bStatus_t ret = SUCCESS;                                                                    
  switch ( param )                                                                            
  {                                                                                           
    case JFBS_HEARTRATE_CHAR1:                                                                      
      if ( len == sizeof ( uint8 ) )                                                          
      {                                                                                       
        JFBS_heartrateChar1 = *((uint8*)value);                                                     
      }                                                                                       
      else                                                                                    
      {                                                                                       
        ret = bleInvalidRange;                                                                
      }                                                                                       
      break;                                                                                  
                                                                                              
    case JFBS_HEARTRATE_2_MEASURE:                                                                      
      if ( len == sizeof ( uint8 ) )                                                          
      {                                                                                       
        JFBS_heartrateChar2 = *((uint8*)value);                                                     
                                                                                              
        // See if Notification has been enabled                                               
        GATTServApp_ProcessCharCfg( JFBS_heartrateChar2Config, &JFBS_heartrateChar2, FALSE,               
                                    JFBS_heartrateAttrTbl, GATT_NUM_ATTRS( JFBS_heartrateAttrTbl ),       
                                    INVALID_TASK_ID);                   
      }                
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn      JFBS_Heartrate_GetParameter
 *
 * @brief   Get a JFBS_Heartrate parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t  JFBS_Heartrate_GetParameter( uint8 param, void *value )        
{                                                                  
  bStatus_t ret = SUCCESS;                                         
  switch ( param )                                                 
  {                                                                
    case JFBS_HEARTRATE_CHAR1:                                           
      *((uint8*)value) = JFBS_heartrateChar1;                            
      break;                                                       
                                                                   
    case JFBS_HEARTRATE_2_MEASURE:                                           
      *((uint8*)value) = JFBS_heartrateChar2;                            
      break;                                                       
                                                                   
    default:                                                       
      ret = INVALIDPARAMETER;                                      
      break;                                                       
  }                                                                
                                                                   
  return ( ret );                                                  
}   

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
bStatus_t JFBS_HeartRate_MeasureNotify( uint16 connHandle, attHandleValueNoti_t *pNoti )
{
  uint16 value = GATTServApp_ReadCharCfg( connHandle, JFBS_heartrateChar2Config );

  // If notifications enabled
  if ( value & GATT_CLIENT_CFG_NOTIFY )
  {
    // Set the handle
    pNoti->handle = JFBS_heartrateAttrTbl[JFBS_HEARTRATE_MEASURE_VALUE_POS].handle;
  
    // Send the notification
    return GATT_Notification( connHandle, pNoti, FALSE );
  }

  return bleIncorrectMode;
}

/*********************************************************************
 * @fn          JFBS_heartrate_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 *
 * @return      Success or Failure
 */
static bStatus_t  JFBS_heartrate_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,        
                                           uint8 *pValue, uint8 *pLen, uint16 offset,   
                                           uint8 maxLen)                 
{                                                                                       
  uint16 uuid;
  bStatus_t status = SUCCESS;                                                           
                                                                                        
  // If attribute permissions require authorization to read, return error               
  if ( gattPermitAuthorRead( pAttr->permissions ) )                                     
  {                                                                                     
    // Insufficient authorization                                                       
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );                                             
  }                                                                                     
                                                                                        
  // Make sure it's not a blob operation (no attributes in the profile are long)        
  if ( offset > 0 )                                                                     
  {                                                                                     
    return ( ATT_ERR_ATTR_NOT_LONG );                                                   
  }                                                                                     
                                                                                        

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE) {                                      
    // Invalid handle                                                                   
    *pLen = 0;                                                                          
    return ATT_ERR_INVALID_HANDLE;                                                      
  }                                                                                     
                                                                                        
  switch ( uuid )                                                                       
  {                                                                                     
    // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;            
    // gattserverapp handles those reads                                                
                                                                                        
    // characteristics 1 has read permissions                                           
    // characteristic 2 does not have read permissions, but because it                  
    //   can be sent as a notification, it is included here                             
  case JFBS_HEARTRATE_CHAR1_UUID:                                                             
  case JFBS_HEARTRATE_2_MEASURE_UUID:                                                             
    *pLen = 1;                                                                          
    pValue[0] = *pAttr->pValue;                                                         
    break;                                                                              
                                                                                        
  default:                                                                              
    // Should never get here! (characteristics 3 and 4 do not have read permissions)    
    *pLen = 0;                                                                          
    status = ATT_ERR_ATTR_NOT_FOUND;                                                    
    break;                                                                              
  }                                                                                     
  return ( status );                                                                    
}      

/*********************************************************************
 * @fn      JFBS_heartrate_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 *
 * @return  Success or Failure
 */
static bStatus_t JFBS_heartrate_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,        
                                            uint8 *pValue, uint8 len, uint16 offset)                               
{ 
  uint16 uuid;
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
                                                                                         
  // If attribute permissions require authorization to write, return error               
  if ( gattPermitAuthorWrite( pAttr->permissions ) )                                     
  {                                                                                      
    // Insufficient authorization                                                        
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );                                              
  }                                                                                      

 
  if (utilExtractUuid16(pAttr,&uuid) == FAILURE) {                                       
    // Invalid handle                                                                    
    return ATT_ERR_INVALID_HANDLE;  
    
  }                                                                                      
   
  if ( pAttr->type.len == ATT_BT_UUID_SIZE ) {
    // 16-bit UUID                                                                         
    uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);  
    /*
#ifdef GATT_TI_UUID_128_BIT
        } else if (pAttr->type.len == ATT_UUID_SIZE) {
        // 16-bit UUID extracted bytes 12 and 13
        uuid = BUILD_UINT16( pAttr->type.uuid[12], pAttr->type.uuid[13]);
    #endif
    */
        
    switch ( uuid )                                                                        
    {
      case JFBS_HEARTRATE_CHAR1_UUID:

        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != 1 )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        } else {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;        
          *pCurValue = pValue[0];

          if( pAttr->pValue == &JFBS_heartrateChar1 )
          {
            notifyApp = JFBS_HEARTRATE_CHAR1;        
          }
        }
             
        break;
        
      case GATT_CLIENT_CHAR_CFG_UUID:                                                        
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,             
                                            offset, GATT_CLIENT_CFG_NOTIFY );

        if ( status == SUCCESS )
          status=SUCCESS;
        {
 /*         uint16 charCfg = BUILD_UINT16( pValue[0], pValue[1] );

          (*heartRateServiceCB)( (charCfg == GATT_CFG_NO_OPERATION) ?
                                  HEARTRATE_MEAS_NOTI_DISABLED :
                                  HEARTRATE_MEAS_NOTI_ENABLED );
*/
        }
        

        break;                                                                               
                                                                                         
      default:                                                                               
        // Should never get here! (characteristics 2 and 4 do not have write permissions)    
        status = ATT_ERR_ATTR_NOT_FOUND;                                                     
        break;   
    }
  } else {
      status = ATT_ERR_INVALID_HANDLE; 
    }   
  
  // If a charactersitic value changed then callback function to notify application of change
    /*
  if ( (notifyApp != 0xFF ) && JFBS_heartrateCB && JFBS_heartrateCB->pfnJFBS_HeartrateChange )
  {
    JFBS_heartrateCB->pfnJFBS_HeartrateChange( notifyApp );  
  }
  */
  
  return ( status );                                                                     
}  



/*********************************************************************
 * @fn          JFBS_heartrate_HandleConnStatusCB
 *
 * @brief       Simple Profile link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none
 */
static void JFBS_heartrate_HandleConnStatusCB( uint16 connHandle, uint8 changeType )
{ 
  // Make sure this is not loopback connection
  if ( connHandle != LOOPBACK_CONNHANDLE )
  {
    // Reset Client Char Config if connection has dropped
    if ( ( changeType == LINKDB_STATUS_UPDATE_REMOVED )      ||
         ( ( changeType == LINKDB_STATUS_UPDATE_STATEFLAGS ) && 
           ( !linkDB_Up( connHandle ) ) ) )
    { 
      GATTServApp_InitCharCfg( connHandle, JFBS_heartrateChar2Config );
    }
  }
}

/*********************************************************************
*********************************************************************/
