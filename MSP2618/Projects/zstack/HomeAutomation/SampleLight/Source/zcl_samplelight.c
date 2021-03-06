/**************************************************************************************************
  Filename:       zcl_sampleLight.c
  Revised:        $Date: 2009-03-18 15:56:27 -0700 (Wed, 18 Mar 2009) $
  Revision:       $Revision: 19453 $


  Description:    Zigbee Cluster Library - sample device application.


  Copyright 2006-2009 Texas Instruments Incorporated. All rights reserved.

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
**************************************************************************************************/

/*********************************************************************
  This device will be like a Light device.  This application is not
  intended to be a Light device, but will use the device description
  to implement this sample code.
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
   
#include "MT.h"
#include "MT_RPC.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"

#include "zcl_samplelight.h"

#include "onboard.h"
   
#include "OSAL_NV.h"
#include "ZDObject.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"

#include <stdio.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
byte zclSampleLight_TaskID;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * IEEE Addresses
 */
//#define LSR_LIGHT
//#define LSR_SWITCH
#define SMARTRF_LIGHT
//#define SMARTRF_SWITCH

#ifdef LSR_LIGHT
ZLongAddr_t ieeeAddr = {0x64, 0x00, 0x02, 0x00, 0x02, 0xCA, 0x25, 0x00}; //LSB first
//ZLongAddr_t ieeeAddr = {0x00, 0x25, 0xCA, 0x02, 0x00, 0x02, 0x00, 0x64};
#endif
#ifdef LSR_SWITCH
ZLongAddr_t ieeeAddr = {0x12, 0x00, 0x02, 0x00, 0x02, 0xCA, 0x25, 0x00}; //LSB first
//ZLongAddr_t ieeeAddr = {0x00, 0x25, 0xCA, 0x02, 0x00, 0x02, 0x00, 0x64};
#endif
#ifdef SMARTRF_LIGHT
ZLongAddr_t ieeeAddr = {0xFA, 0x12, 0x0A, 0x00, 0x00, 0x4B, 0x12, 0x00}; //LSB first
//ZLongAddr_t ieeeAddr = {0x00, 0x12, 0x4B, 0x00, 0x00, 0x0A, 0x12, 0xFA};
#endif
#ifdef SMARTRF_SWITCH
ZLongAddr_t ieeeAddr = {0x88, 0x12, 0x0A, 0x00, 0x00, 0x4B, 0x12, 0x00}; //LSB first
//ZLongAddr_t ieeeAddr = {0x00, 0x12, 0x4B, 0x00, 0x00, 0x0A, 0x12, 0x88};
#endif

/*********************************************************************
 * LOCAL VARIABLES
 */

static afAddrType_t zclSampleLight_DstAddr;

#define ZCLSAMPLELIGHT_BINDINGLIST       2
static cId_t bindingInClusters[ZCLSAMPLELIGHT_BINDINGLIST] =
{
  ZCL_CLUSTER_ID_GEN_ON_OFF,
  ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL
};

#define ZCLSAMPLESW_BINDINGLIST         2
static cId_t bindingOutClusters[ZCLSAMPLESW_BINDINGLIST] =
{
  ZCL_CLUSTER_ID_GEN_ON_OFF,
  ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL
};

// Test Endpoint to allow SYS_APP_MSGs
static endPointDesc_t sampleLight_TestEp =
{
  20,                                 // Test endpoint
  &zclSampleLight_TaskID,
  (SimpleDescriptionFormat_t *)NULL,  // No Simple description for this test endpoint
  (afNetworkLatencyReq_t)0            // No Network Latency req
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void zclSampleLight_HandleKeys( byte shift, byte keys );
static void zclSampleLight_BasicResetCB( void );
static void zclSampleLight_IdentifyCB( zclIdentify_t *pCmd );
static void zclSampleLight_IdentifyQueryRspCB( zclIdentifyQueryRsp_t *pRsp );
static void zclSampleLight_OnOffCB( zclOnOff_t *pCmd );
static void zclSampleLight_ProcessIdentifyTimeChange( void );
void zclSampleLight_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );

/*my functions*/
//static void zclSampleLight_UARTWrite( unsigned char* buff, int len );
//static void zclSampleLight_UARTWriteNr(unsigned char* text, int nr);
static void zclSampleLight_MoveToLevelCB( zclLCMoveToLevel_t *pCmd );
static void zclSampleLight_CtlMoveCB( zclLCMove_t *pCmd );
static void zclSampleLight_CtlStepCB( zclLCStep_t *pCmd );

// Functions to process ZCL Foundation incoming Command/Response messages 
static void zclSampleLight_ProcessIncomingMsg( zclIncomingMsg_t *msg );
#ifdef ZCL_READ
static uint8 zclSampleLight_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg );
#endif
#ifdef ZCL_WRITE
static uint8 zclSampleLight_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg );
#endif
static uint8 zclSampleLight_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg );
#ifdef ZCL_DISCOVER
static uint8 zclSampleLight_ProcessInDiscRspCmd( zclIncomingMsg_t *pInMsg );
#endif

/*********************************************************************
 * ZCL General Profile Callback table
 */
static zclGeneral_AppCallbacks_t zclSampleLight_CmdCallbacks =
{
  zclSampleLight_BasicResetCB,              // Basic Cluster Reset command
  zclSampleLight_IdentifyCB,                // Identify command  
  zclSampleLight_IdentifyQueryRspCB,        // Identify Query Response command
  zclSampleLight_OnOffCB,                   // On/Off cluster command
  zclSampleLight_MoveToLevelCB,             // Level Control Move to Level command
  zclSampleLight_CtlMoveCB,                 // Level Control Move command
  zclSampleLight_CtlStepCB,                 // Level Control Step command
  NULL,                                     // Group Response commands
  NULL,                                     // Scene Store Request command
  NULL,                                     // Scene Recall Request command
  NULL,                                     // Scene Response command
  NULL,                                     // Alarm (Response) command
  NULL,                                     // RSSI Location commands
  NULL,                                     // RSSI Location Response commands
};

/*********************************************************************
 * @fn          zclSampleLight_Init
 *
 * @brief       Initialization function for the zclGeneral layer.
 *
 * @param       none
 *
 * @return      none
 */
void zclSampleLight_Init( byte task_id )
{
  zclSampleLight_TaskID = task_id;

  // This app is part of the Home Automation Profile
  zclHA_Init( &zclSampleLight_SimpleDesc );

  // Register the ZCL General Cluster Library callback functions
  zclGeneral_RegisterCmdCallbacks( SAMPLELIGHT_ENDPOINT, &zclSampleLight_CmdCallbacks );

  // Register the application's attribute list
  zcl_registerAttrList( SAMPLELIGHT_ENDPOINT, SAMPLELIGHT_MAX_ATTRIBUTES, zclSampleLight_Attrs );

  // Register the Application to receive the unprocessed Foundation command/response messages
  zcl_registerForMsg( zclSampleLight_TaskID );
  
  // Register for all key events - This app will handle all key events
  RegisterForKeys( zclSampleLight_TaskID );

  // Register for a test endpoint
  afRegister( &sampleLight_TestEp );
  
  ZDO_RegisterForZDOMsg( zclSampleLight_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( zclSampleLight_TaskID, Match_Desc_rsp );
  
  osal_nv_write(ZCD_NV_EXTADDR, 0, Z_EXTADDR_LEN, ieeeAddr);
}

/*********************************************************************
 * @fn          zclSample_event_loop
 *
 * @brief       Event Loop Processor for zclGeneral.
 *
 * @param       none
 *
 * @return      none
 */
uint16 zclSampleLight_event_loop( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  
  volatile int command;
  
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( zclSampleLight_TaskID )) )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZCL_INCOMING_MSG:
          // Incoming ZCL Foundation command/response messages
          zclSampleLight_ProcessIncomingMsg( (zclIncomingMsg_t *)MSGpkt );
          break;
        
        case ZDO_CB_MSG:
          zclSampleLight_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;
        
        case KEY_CHANGE:
          zclSampleLight_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        default:
          break;
      }
      
      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT )
  {
    if ( zclSampleLight_IdentifyTime > 0 )
      zclSampleLight_IdentifyTime--;
    zclSampleLight_ProcessIdentifyTimeChange();

    return ( events ^ SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT );
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * @fn      zclSampleSw_ProcessZDOMsgs()
 *
 * @brief   Process response messages
 *
 * @param   none
 *
 * @return  none
 */
void zclSampleLight_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
      {
        // Light LED
        HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
      }
#if defined(BLINK_LEDS)
      else
      {
        // Flash LED to show failure
        HalLedSet ( HAL_LED_4, HAL_LED_MODE_FLASH );
      }
#endif
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp )
        {
          if ( pRsp->status == ZSuccess && pRsp->cnt )
          {
            zclSampleLight_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            zclSampleLight_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            // Take the first endpoint, Can be changed to search through endpoints
            zclSampleLight_DstAddr.endPoint = pRsp->epList[0];

            // Light LED
            HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
          }
          osal_mem_free( pRsp );
        }
      }
      break;
  }
}

/*********************************************************************
 * @fn      zclSampleLight_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_4
 *                 HAL_KEY_SW_3
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void zclSampleLight_HandleKeys( byte shift, byte keys )
{
  zAddrType_t dstAddr;
  
  
  (void)shift;  // Intentionally unreferenced parameter

/*  if ( keys & HAL_KEY_SW_1 )
  {
    // Using this as the "Light Switch"
#ifdef ZCL_ON_OFF
    zclGeneral_SendOnOff_CmdToggle( SAMPLESW_ENDPOINT, &zclSampleSw_DstAddr, false, 0 );
#endif
  }*/
  
  if ( keys & HAL_KEY_SW_2 )
  {
    // Initiate an End Device Bind Request, this bind request will
    // only use a cluster list that is important to binding.
    dstAddr.addrMode = afAddr16Bit;
    dstAddr.addr.shortAddr = 0;   // Coordinator makes the match
    ZDP_EndDeviceBindReq( &dstAddr, NLME_GetShortAddr(),
                           SAMPLELIGHT_ENDPOINT,
                           ZCL_HA_PROFILE_ID,
                           ZCLSAMPLELIGHT_BINDINGLIST, bindingInClusters,
                           ZCLSAMPLESW_BINDINGLIST, bindingOutClusters,   // No Outgoing clusters to bind
                           TRUE );
  }

  if ( keys & HAL_KEY_SW_3 )
  {
    /*dstAddr.addrMode = afAddr16Bit;
    dstAddr.addr.shortAddr = 0;   // Coordinator makes the match
    ZDP_EndDeviceBindReq( &dstAddr, NLME_GetShortAddr(),
                           SAMPLELIGHT_ENDPOINT,
                           ZCL_HA_PROFILE_ID,
                           0, NULL, // No Incoming clusters to bind
                           ZCLSAMPLESW_BINDINGLIST, bindingOutClusters,   
                           TRUE );*/
  }

  if ( keys & HAL_KEY_SW_4 )
  {
  }

}

/*********************************************************************
 * @fn      zclSampleLight_ProcessIdentifyTimeChange
 *
 * @brief   Called to process any change to the IdentifyTime attribute.
 *
 * @param   none
 *
 * @return  none
 */
static void zclSampleLight_ProcessIdentifyTimeChange( void )
{
  if ( zclSampleLight_IdentifyTime > 0 )
  {
    osal_start_timerEx( zclSampleLight_TaskID, SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT, 1000 );
    HalLedBlink ( HAL_LED_4, 0xFF, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME );
  }
  else
  {
    if ( zclSampleLight_OnOff )
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_ON );
    else
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );
    osal_stop_timerEx( zclSampleLight_TaskID, SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT );
  }
}

/*********************************************************************
 * @fn      zclSampleLight_BasicResetCB
 *
 * @brief   Callback from the ZCL General Cluster Library
 *          to set all the Basic Cluster attributes to default values.
 *
 * @param   none
 *
 * @return  none
 */
static void zclSampleLight_BasicResetCB( void )
{
  // Reset all attributes to default values
}

/*********************************************************************
 * @fn      zclSampleLight_IdentifyCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an Identity Command for this application.
 *
 * @param   srcAddr - source address and endpoint of the response message
 * @param   identifyTime - the number of seconds to identify yourself
 *
 * @return  none
 */
static void zclSampleLight_IdentifyCB( zclIdentify_t *pCmd )
{
  zclSampleLight_IdentifyTime = pCmd->identifyTime;
  zclSampleLight_ProcessIdentifyTimeChange();
}

/*********************************************************************
 * @fn      zclSampleLight_IdentifyQueryRspCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an Identity Query Response Command for this application.
 *
 * @param   srcAddr - requestor's address
 * @param   timeout - number of seconds to identify yourself (valid for query response)
 *
 * @return  none
 */
static void zclSampleLight_IdentifyQueryRspCB(  zclIdentifyQueryRsp_t *pRsp )
{
  // Query Response (with timeout value)
  (void)pRsp;
}

/*********************************************************************
 * @fn      zclSampleLight_OnOffCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an On/Off Command for this application.
 *
 * @param   pCmd - COMMAND_ON, COMMAND_OFF or COMMAND_TOGGLE
 *
 * @return  none
 */
static void zclSampleLight_OnOffCB( zclOnOff_t *pCmd )
{
  // Turn on the light
  if ( pCmd->cmdID == COMMAND_ON ){
    zclSampleLight_OnOff = LIGHT_ON;
    //MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_HAGATE), MT_HAGATE_LIGHT_ON, 0, 0);
    //zclSampleLight_UARTWrite("LightOn\r\n", 10);
  }
  // Turn off the light
  else if ( pCmd->cmdID == COMMAND_OFF ){
    zclSampleLight_OnOff = LIGHT_OFF;
    //zclSampleLight_UARTWrite("LightOff\r\n", 11);
  }
  // Toggle the light
  else
  {
    if ( zclSampleLight_OnOff == LIGHT_OFF )
      zclSampleLight_OnOff = LIGHT_ON;
    else
      zclSampleLight_OnOff = LIGHT_OFF;
  }
  
  // In this sample app, we use LED4 to simulate the Light
  if ( zclSampleLight_OnOff == LIGHT_ON ){
    HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_HAGATE), MT_HAGATE_LIGHT_ON, sizeof(zclOnOff_t), (uint8*)pCmd);
    //zclSampleLight_UARTWrite("\r\n", 2);
  }
  else{
    HalLedSet( HAL_LED_4, HAL_LED_MODE_OFF );
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_HAGATE), MT_HAGATE_LIGHT_OFF, sizeof(zclOnOff_t), (uint8*)pCmd);
    //zclSampleLight_UARTWrite("\r\n", 2);
  }
}

/*********************************************************************
 * @fn      zclSampleLight_MoveToLevelCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an MoveToLevel Command for this application.
 *
 * @param   pCmd - new level command
 *
 * @return  none
 */
static void zclSampleLight_MoveToLevelCB(zclLCMoveToLevel_t *pCmd){
  //char buff[20];
  //sprintf(buff, "moveToLevel %d \r\n", pCmd->level);
  //zclSampleLight_UARTWrite((unsigned char*)buff, 20);
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_HAGATE), MT_HAGATE_LIGHT_DIM_LEVEL, sizeof(zclLCMoveToLevel_t), (uint8*)pCmd);
  //zclSampleLight_UARTWrite("\r\n", 2);
  //osal_start_timerEx( zclSampleLight_TaskID, SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT, 1000 );
  //void HalLedBlink ( uint8 leds, uint8 numBlinks, uint8 percent, uint16 period )
  HalLedBlink ( HAL_LED_4, 0xff, ((pCmd->level)/3 + 10), 10 );
  //zclSampleLight_UARTWrite("moveToLevel\r\n", 14);
}

/*********************************************************************
 * @fn      zclSampleLight_CtlMoveCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an level control move Command for this application.
 *
 * @param   pCmd - move command
 *
 * @return  none
 */
static void zclSampleLight_CtlMoveCB(zclLCMove_t *pCmd){
  //zclSampleLight_UARTWrite("ctlMove\r\n", 10);
}

/*********************************************************************
 * @fn      zclSampleLight_CtlStepCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an level control step Command for this application.
 *
 * @param   pCmd - step command
 *
 * @return  none
 */
static void zclSampleLight_CtlStepCB(zclLCStep_t *pCmd){
  //zclSampleLight_UARTWrite("ctlStep\r\n", 10);
}

/****************************************************************************** 
 * 
 *  Functions for processing ZCL Foundation incoming Command/Response messages
 *
 *****************************************************************************/

/*********************************************************************
 * @fn      zclSampleLight_ProcessIncomingMsg
 *
 * @brief   Process ZCL Foundation incoming message
 *
 * @param   pInMsg - pointer to the received message
 *
 * @return  none
 */
static void zclSampleLight_ProcessIncomingMsg( zclIncomingMsg_t *pInMsg)
{
  switch ( pInMsg->zclHdr.commandID )
  {
#ifdef ZCL_READ
    case ZCL_CMD_READ_RSP:
      zclSampleLight_ProcessInReadRspCmd( pInMsg );
      break;
#endif
#ifdef ZCL_WRITE    
    case ZCL_CMD_WRITE_RSP:
      zclSampleLight_ProcessInWriteRspCmd( pInMsg );
      break;
#endif
#ifdef ZCL_REPORT
    // See ZCL Test Applicaiton (zcl_testapp.c) for sample code on Attribute Reporting
    case ZCL_CMD_CONFIG_REPORT:
      //zclSampleLight_ProcessInConfigReportCmd( pInMsg );
      break;
    
    case ZCL_CMD_CONFIG_REPORT_RSP:
      //zclSampleLight_ProcessInConfigReportRspCmd( pInMsg );
      break;
    
    case ZCL_CMD_READ_REPORT_CFG:
      //zclSampleLight_ProcessInReadReportCfgCmd( pInMsg );
      break;
    
    case ZCL_CMD_READ_REPORT_CFG_RSP:
      //zclSampleLight_ProcessInReadReportCfgRspCmd( pInMsg );
      break;
    
    case ZCL_CMD_REPORT:
      //zclSampleLight_ProcessInReportCmd( pInMsg );
      break;
#endif   
    case ZCL_CMD_DEFAULT_RSP:
      zclSampleLight_ProcessInDefaultRspCmd( pInMsg );
      break;
#ifdef ZCL_DISCOVER     
    case ZCL_CMD_DISCOVER_RSP:
      zclSampleLight_ProcessInDiscRspCmd( pInMsg );
      break;
#endif  
    default:
      break;
  }
  
  if ( pInMsg->attrCmd )
    osal_mem_free( pInMsg->attrCmd );
}

#ifdef ZCL_READ
/*********************************************************************
 * @fn      zclSampleLight_ProcessInReadRspCmd
 *
 * @brief   Process the "Profile" Read Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclSampleLight_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclReadRspCmd_t *readRspCmd;
  uint8 i;

  readRspCmd = (zclReadRspCmd_t *)pInMsg->attrCmd;
  for (i = 0; i < readRspCmd->numAttr; i++)
  {
    // Notify the originator of the results of the original read attributes 
    // attempt and, for each successfull request, the value of the requested 
    // attribute
  }

  return TRUE; 
}
#endif // ZCL_READ

#ifdef ZCL_WRITE
/*********************************************************************
 * @fn      zclSampleLight_ProcessInWriteRspCmd
 *
 * @brief   Process the "Profile" Write Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclSampleLight_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclWriteRspCmd_t *writeRspCmd;
  uint8 i;

  writeRspCmd = (zclWriteRspCmd_t *)pInMsg->attrCmd;
  for (i = 0; i < writeRspCmd->numAttr; i++)
  {
    // Notify the device of the results of the its original write attributes
    // command.
  }

  return TRUE; 
}
#endif // ZCL_WRITE

/*********************************************************************
 * @fn      zclSampleLight_ProcessInDefaultRspCmd
 *
 * @brief   Process the "Profile" Default Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclSampleLight_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg )
{
  // zclDefaultRspCmd_t *defaultRspCmd = (zclDefaultRspCmd_t *)pInMsg->attrCmd;
   
  // Device is notified of the Default Response command.
  (void)pInMsg;
  
  return TRUE; 
}

#ifdef ZCL_DISCOVER
/*********************************************************************
 * @fn      zclSampleLight_ProcessInDiscRspCmd
 *
 * @brief   Process the "Profile" Discover Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclSampleLight_ProcessInDiscRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclDiscoverRspCmd_t *discoverRspCmd;
  uint8 i;
  
  discoverRspCmd = (zclDiscoverRspCmd_t *)pInMsg->attrCmd;
  for ( i = 0; i < discoverRspCmd->numAttr; i++ )
  {
    // Device is notified of the result of its attribute discovery command.
  }
  
  return TRUE;
}
#endif // ZCL_DISCOVER

/*********************************************************************
 * @fn      zclSampleLight_UARTWrite
 *
 * @brief   write buffer to uart
 *
 * @param   buff - buffer to write to uart
 * @param   len - length of buffer
 *
 * @return  none
 */
/*static void zclSampleLight_UARTWrite(unsigned char* buff, int len){
  HalUARTWrite(HAL_UART_PORT_0, buff, len);
}


static void zclSampleLight_UARTWriteNr(unsigned char* text, int nr){
  char buff[10];
  sprintf(buff, "%s %d \r\n", text, nr);
  zclSampleLight_UARTWrite((unsigned char*)buff, 10);
}*/

/****************************************************************************
****************************************************************************/


