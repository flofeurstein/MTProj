/***************************************************************************************************
  Filename:       MT_HAGATE.c
  Revised:        $Date: 2013-06-18 $
  Revision:       $Revision: 1 $

  Description:    

  

 ***************************************************************************************************/

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include "ZComDef.h"
#include "MT.h"        /* This is here because all the SPI_CMD_XXX are defined in this header */
#include "MT_HAGATE.h"
#include "MT_AF.h"     /* This is here because this MT_HAGATE makes some routine call to AF */
#include "MT_RPC.h"
#include "zcl_general.h"

#if defined( APP_TP )
 #include "TestProfile.h"
#endif
#if defined( APP_TP2 )
 #include "TestProfile2.h"
 #include "nwk_util.h"
#endif

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/
#define HAGATE_NR_REPORT_ATTR           2


/***************************************************************************************************
 * LOCAL FUNCTIONS
 ***************************************************************************************************/
#if defined (MT_HAGATE_FUNC)
void MT_HaGateMsg(uint8 *pBuf);
void MT_HaGateUserCmd(byte *pData);
void MT_HaGateLightOnOff(uint8 cmd1, uint8* pBuf);
void MT_HaGateLightToggle(uint8* pBuf);
void MT_HaGateLightMoveToLevel(uint8* pBuf);
#endif

#if defined (MT_HAGATE_FUNC)
/***************************************************************************************************
 * @fn      MT_HaGateCommandProcessing
 *
 * @brief  Process all the APP commands that are issued by test tool
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  status
 ***************************************************************************************************/
uint8 MT_HaGateCommandProcessing(uint8 *pBuf)
{
  uint8 status = MT_RPC_SUCCESS;
  uint8 cmd1 = pBuf[MT_RPC_POS_CMD1];

  switch (cmd1)
  {
    case MT_HAGATE_LIGHT_ON:
      MT_HaGateLightOnOff(cmd1, pBuf);
      break;

    case MT_HAGATE_LIGHT_OFF:
      MT_HaGateLightOnOff(cmd1, pBuf);
      break;
  
  case MT_HAGATE_LIGHT_TOGGLE:
      MT_HaGateLightToggle(pBuf);
      //MT_HaGateMsg(pBuf);
      break;

    case MT_HAGATE_LIGHT_DIM_LEVEL:
      MT_HaGateLightMoveToLevel(pBuf);
      break;

    default:
      status = MT_RPC_ERR_COMMAND_ID;
      break;
  }

  return status;
}

void MT_HaGateLightOnOff(uint8 cmd1, uint8* pBuf)
{
  afAddrType_t dstAddr;
  //osal_cpyExtAddr(dstAddr.addr.extAddr, ((mtHaGateMsg_t*)pBuf)->IEEEAddr);
  dstAddr.addrMode = (afAddrMode_t)AddrNotPresent;//Addr64Bit;
  dstAddr.endPoint = 0;//SAMPLELIGHT_ENDPOINT;
  dstAddr.addr.shortAddr = 0;
  
  //ZCL_DATATYPE_STRUCT;
  //ZCL_DATATYPE_ARRAY;
  
//  zclReportCmd_t reportCmd;
//  uint8 
//  
//  reportCmd.numAttr = HAGATE_NR_REPORT_ATTR;
  
  if(cmd1 == MT_HAGATE_LIGHT_ON)
  {
//    zcl_SendReportCmd( SAMPLELIGHT_ENDPOINT, &dstAddr,
//                             ZCL_CLUSTER_ID_GEN_ON_OFF, &reportCmd,
//                             ZCL_FRAME_SERVER_CLIENT_DIR, false, 0 );
    zclGeneral_SendOnOff_CmdOn(SAMPLELIGHT_ENDPOINT, &dstAddr, false, 0);
  }
  else
  {
//    zcl_SendReportCmd( SAMPLELIGHT_ENDPOINT, &dstAddr,
//                             ZCL_CLUSTER_ID_GEN_ON_OFF, &reportCmd,
//                             ZCL_FRAME_SERVER_CLIENT_DIR, false, 0 );
    zclGeneral_SendOnOff_CmdOff(SAMPLELIGHT_ENDPOINT, &dstAddr, false, 0);
  }
}

void MT_HaGateLightToggle(uint8* pBuf)
{
  afAddrType_t dstAddr;
  //osal_cpyExtAddr(dstAddr.addr.extAddr, ((mtHaGateMsg_t*)pBuf)->IEEEAddr);
  dstAddr.addrMode = (afAddrMode_t)AddrNotPresent;//Addr64Bit;
  dstAddr.endPoint = 0;//SAMPLELIGHT_ENDPOINT;
  dstAddr.addr.shortAddr = 0;
  
//  zclReportCmd_t reportCmd;
//
//  zcl_SendReportCmd( SAMPLELIGHT_ENDPOINT, &dstAddr,
//                             ZCL_CLUSTER_ID_GEN_ON_OFF, &reportCmd,
//                             ZCL_FRAME_SERVER_CLIENT_DIR, false, 0 );
  zclGeneral_SendOnOff_CmdToggle(SAMPLELIGHT_ENDPOINT, &dstAddr, false, 0);
}
 
void MT_HaGateLightMoveToLevel(uint8* pBuf)
{
  afAddrType_t dstAddr;
  zclLCMoveToLevel_t* moveToLevel;
  osal_cpyExtAddr(dstAddr.addr.extAddr, ((mtHaGateMsg_t*)pBuf)->IEEEAddr);
  dstAddr.addrMode = (afAddrMode_t)Addr64Bit;
  dstAddr.endPoint = SAMPLELIGHT_ENDPOINT;
  moveToLevel = (zclLCMoveToLevel_t*)(((mtHaGateMsg_t*)pBuf)->msgData);
  
//  zclReportCmd_t reportCmd;
//  
//  zcl_SendReportCmd( SAMPLELIGHT_ENDPOINT, &dstAddr,
//                             COMMAND_LEVEL_MOVE_TO_LEVEL, &reportCmd,
//                             ZCL_FRAME_SERVER_CLIENT_DIR, false, 0 );

  //zclGeneral_SendLevelControlMoveToLevel(SAMPLELIGHT_ENDPOINT, &dstAddr, moveToLevel->level, moveToLevel->transitionTime, false, 0);
}

//void abc(uint8 cmd1, uint8* pBuf)
//{
//  uint8 retValue = ZFailure;
//  uint8 endpoint;
//  endPointDesc_t *epDesc;
//  mtHaGateMsg_t *msg;
//  uint8 cmdId, dataLen;
//
//  /* parse header */
//  dataLen = pBuf[MT_RPC_POS_LEN];
//  cmdId = pBuf[MT_RPC_POS_CMD1];
//  pBuf += MT_RPC_FRAME_HDR_SZ;
//
//  /* Get the endpoint and skip past it.*/
//  endpoint = *pBuf++;
//  dataLen--;
//
//  /* Look up the endpoint */
//  epDesc = afFindEndPointDesc( endpoint );
//
//  if (epDesc)
//  {
//    /* Build and send the message to the APP */
//    msg = (mtHaGateMsg_t *)osal_msg_allocate(sizeof(mtHaGateMsg_t) + (dataLen));
//    if ( msg )
//    {
//      /* Build and send message up the app */
//     /* msg->hdr.event = MT_SYS_APP_MSG;
//      msg->endpoint = endpoint;
//      msg->appDataLen = dataLen;
//      msg->appData = (uint8*)(msg+1);
//      osal_memcpy( msg->appData, pBuf, dataLen);
//      osal_msg_send( *(epDesc->task_id), (uint8 *)msg );
//      typedef struct
//{
//  uint8             deviceId;
//  uint16            nwkAddr;
//  ZLongAddr_t       IEEEAddr;
//  uint8             endpoint;
//  uint8             dataLen;
//  uint8             *msgData;
//} mtHaGateMsg_t;
//      */
//      //msg->deviceId = 0;
//      //msg->nwkAddr = 0;
//      msg->endpoint = endpoint;
//      msg->dataLen = dataLen;
//      msg->msgData = (uint8*)(msg+1);
//      osal_memcpy( msg->msgData, pBuf, dataLen);
//      //osal_msg_send( *(epDesc->task_id), (uint8 *)msg );
//      
//      /* Info for response */
//      retValue = ZSuccess;
//    }
//  }
//  
//  /*uild and send a ZTOOL msg
// * @param   uint8 cmdType - include type and subsystem
// *          uint8 cmdId - command ID
// *          byte dataLen
// *          byte *pData
// *
// * @return  void
// * MT_BuildAndSendZToolResponse(uint8 cmdType, uint8 cmdId, uint8 dataLen, uint8 *pData)
// ***************************************************************************************************/
//  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_HAGATE), cmdId, 1, pBuf);
//}
//
///***************************************************************************************************
// * @fn      MT_HaGateMsg
// *
// * @brief   Process HAGATE_MSG command
// *
// * @param   pBuf - pointer to the received buffer
// *
// * @return  void
// ***************************************************************************************************/
//void MT_HaGateMsg(uint8 *pBuf)
//{
//  uint8 retValue = ZFailure;
//  uint8 endpoint;
//  endPointDesc_t *epDesc;
//  mtHaGateMsg_t *msg;
//  uint8 cmdId, dataLen;
//
//  /* parse header */
//  dataLen = pBuf[MT_RPC_POS_LEN];
//  cmdId = pBuf[MT_RPC_POS_CMD1];
//  pBuf += MT_RPC_FRAME_HDR_SZ;
//
//  /* Get the endpoint and skip past it.*/
//  endpoint = *pBuf++;
//  dataLen--;
//
//  /* Look up the endpoint */
//  epDesc = afFindEndPointDesc( endpoint );
//
//  if (epDesc)
//  {
//    /* Build and send the message to the APP */
//    msg = (mtHaGateMsg_t *)osal_msg_allocate(sizeof(mtHaGateMsg_t) + (dataLen));
//    if ( msg )
//    {
//      /* Build and send message up the app */
//     /* msg->hdr.event = MT_SYS_APP_MSG;
//      msg->endpoint = endpoint;
//      msg->appDataLen = dataLen;
//      msg->appData = (uint8*)(msg+1);
//      osal_memcpy( msg->appData, pBuf, dataLen);
//      osal_msg_send( *(epDesc->task_id), (uint8 *)msg );*/
//
//      /* Info for response */
//      retValue = ZSuccess;
//    }
//  }
//
//  /* Build and send back the response */
//  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_APP), cmdId, 1, &retValue);
//}
//
///***************************************************************************************************
// * @fn      MT_HaGateUserCmd
// *
// * @brief   Process APP_MSG command
// *
// * @param   pBuf - pointer to the received buffer
// *
// * @return  void
// ***************************************************************************************************/
//void MT_HaGateUserCmd(uint8 *pBuf)
//{
//
//  uint8 retValue, cmdId;
//
//#if defined (APP_TGEN) || defined (NWK_TEST) || defined (APP_TP) || defined (APP_TP2) || defined (OSAL_TOTAL_MEM) || defined (APP_DEBUG)
//  uint16 app_cmd;
//  uint8 srcEp;
//  uint16 param1;
//  uint16 param2;
//#endif
//#if defined (OSAL_TOTAL_MEM)
//  uint8 pData[2];
//#endif
//
//  /* parse header */
//  cmdId = pBuf[MT_RPC_POS_CMD1];
//  pBuf += MT_RPC_FRAME_HDR_SZ;
//
//  retValue = INVALID_TASK;     //should be changed later
//
//#if defined (APP_TGEN) || defined (NWK_TEST) || defined (APP_TP) || defined (APP_TP2) || defined (OSAL_TOTAL_MEM) || defined (APP_DEBUG)
//
//  srcEp = *pBuf++;
//
//  app_cmd = BUILD_UINT16( pBuf[0] , pBuf[1] );
//  pBuf = pBuf + sizeof( uint16 );
//
//  param1 = BUILD_UINT16( pBuf[0] , pBuf[1] );
//  pBuf = pBuf + sizeof( uint16 );
//
//  param2 = BUILD_UINT16( pBuf[0] , pBuf[1] );
//
//  switch ( app_cmd )
//  {
//
//#if defined (APP_TGEN)
//    case TGEN_START:
//      TrafficGenApp_SendCmdMSG( param1, param2, TRAFFICGENAPP_CMD_START );
//      retValue = ZSUCCESS;
//      break;
//
//    case TGEN_STOP:
//      TrafficGenApp_SendCmdMSG( param1, param2, TRAFFICGENAPP_CMD_STOP );
//      retValue = ZSUCCESS;
//      break;
//
//    case TGEN_COUNT:
//      retValue = TrafficGenApp_CountPkt( param1, param2 );
//      return;
//      break;
//#endif
//
//#if defined (NWK_TEST)
//    case HW_TEST:
//      HwApp_Start( HI_UINT16(param1), LO_UINT16(param1), HI_UINT16(param2),
//                    1000, LO_UINT16(param2), 3, 0 );
//      break;
//
//    case HW_DISPLAY_RESULT:
//      HwApp_TestInfo();
//      break;
//
//    case HW_SEND_STATUS:
//      HwApp_SendStats();
//      break;
//#endif
//
//#if defined( APP_TP ) || defined ( APP_TP2 )
//  #if defined( APP_TP )
//    case TP_SEND_NODATA:
//      retValue = TestProfileApp_SendNoData( srcEp, (byte)param1 );
//      break;
//  #endif // APP_TP
//
//    case TP_SEND_BUFFERTEST:
//      retValue = TestProfileApp_SendBufferReq( srcEp, (byte)param1 );
//      break;
//
//  #if defined( APP_TP )
//    case TP_SEND_UINT8:
//      retValue = TestProfileApp_SendUint8( srcEp, (byte)param1 );
//      break;
//
//    case TP_SEND_INT8:
//      retValue = TestProfileApp_SendInt8( srcEp, (byte)param1 );
//      break;
//
//    case TP_SEND_UINT16:
//      retValue = TestProfileApp_SendUint16( srcEp, (byte)param1 );
//      break;
//
//    case TP_SEND_INT16:
//      retValue = TestProfileApp_SendInt16( srcEp, (byte)param1 );
//      break;
//
//    case TP_SEND_SEMIPREC:
//      retValue = TestProfileApp_SendSemiPrec( srcEp, (byte)param1 );
//      break;
//
//    case TP_SEND_FREEFORM:
//      retValue = TestProfileApp_SendFreeFormReq( srcEp, (byte)param1 );
//      break;
//
//  #else // APP_TP
//    case TP_SEND_FREEFORM:
//      retValue = TestProfileApp_SendFreeFormReq(srcEp, (byte)param1, (byte)param2);
//      break;
//  #endif
//
//  #if defined( APP_TP )
//    case TP_SEND_ABS_TIME:
//      retValue = TestProfileApp_SendAbsTime( srcEp, (byte)param1 );
//      break;
//
//    case TP_SEND_REL_TIME:
//      retValue = TestProfileApp_SendRelativeTime( srcEp, (byte)param1 );
//      break;
//
//    case TP_SEND_CHAR_STRING:
//      retValue = TestProfileApp_SendCharString( srcEp, (byte)param1 );
//      break;
//
//    case TP_SEND_OCTET_STRING:
//      retValue = TestProfileApp_SendOctetString( srcEp, (byte)param1 );
//      break;
//  #endif // APP_TP
//
//    case TP_SET_DSTADDRESS:
//      retValue = TestProfileApp_SetDestAddress(HI_UINT16(param1), LO_UINT16(param1), param2);
//      break;
//
//  #if defined( APP_TP2 )
//    case TP_SEND_BUFFER_GROUP:
//      retValue = TestProfileApp_SendBufferGroup( srcEp, (byte)param1 );
//      break;
//  #endif // APP_TP
//
//    case TP_SEND_BUFFER:
//      retValue = TestProfileApp_SendBuffer( srcEp, (byte)param1 );
//      break;
//
//  #if defined( APP_TP )
//    case TP_SEND_MULT_KVP_8BIT:
//      TestProfileApp_SendMultiKVP_8bit( srcEp, (byte)param1 );
//      retValue = ZSuccess;
//      break;
//
//    case TP_SEND_MULT_KVP_16BIT:
//      TestProfileApp_SendMultiKVP_16bit( srcEp, (byte)param1 );
//      retValue = ZSuccess;
//      break;
//
//    case TP_SEND_MULT_KVP_TIME:
//      TestProfileApp_SendMultiKVP_Time( srcEp, (byte)param1 );
//      retValue = ZSuccess;
//      break;
//
//    case TP_SEND_MULT_KVP_STRING:
//      TestProfileApp_SendMultiKVP_String( srcEp, (byte)param1 );
//      retValue = ZSuccess;
//      break;
//
//    case TP_SEND_MULTI_KVP_STR_TIME:
//      retValue = ZSuccess;
//      TestProfileApp_SendMultiKVP_String_Time( srcEp, (byte)param1 );
//      break;
//  #endif // APP_TP
//
//    case TP_SEND_COUNTED_PKTS:
//      TestProfileApp_SendCountedPktsReq(HI_UINT16(param1), LO_UINT16(param1), param2);
//      retValue = ZSuccess;
//      break;
//
//    case TP_SEND_RESET_COUNTER:
//      TestProfileApp_CountedPakts_ResetCounterReq( (byte)param1 );
//      retValue = ZSuccess;
//      break;
//
//    case TP_SEND_GET_COUNTER:
//      TestProfileApp_CountedPakts_GetCounterReq( srcEp, (byte)param1 );
//      retValue = ZSuccess;
//      break;
//
//    case TP_SET_PERMIT_JOIN:
//      if ( ZG_BUILD_RTR_TYPE && ZG_DEVICE_RTR_TYPE )
//      {
//        NLME_PermitJoiningRequest( (byte)param1 );
//        retValue = ZSuccess;
//      }
//      else
//      {
//        retValue = ZFailure;
//      }
//      break;
//
//  #if defined ( APP_TP2 )
//    case TP_ADD_GROUP:
//      retValue = TestProfileApp_SetGroup( srcEp, param1 );
//      break;
//
//    case TP_REMOVE_GROUP:
//      retValue = TestProfileApp_RemoveGroup( srcEp, param1 );
//      break;
//
//    case TP_SEND_UPDATE_KEY:
//      retValue = TestProfileApp_UpdateKey( srcEp, (uint8)param1, param2 );
//      break;
//
//    case TP_SEND_SWITCH_KEY:
//      retValue = TestProfileApp_SwitchKey(  srcEp, (uint8)param1, param2 );
//      break;
//
//    case TP_SEND_BUFFERTEST_GROUP:
//      retValue = TestProfileApp_SendBufferGroupReq( srcEp, (byte)param1, (byte)param2 );
//      break;
//
//    case TP_SEND_ROUTE_DISC_REQ:
//      retValue = TestProfileApp_SendRouteDiscReq( srcEp, param1,
//                                  HI_UINT16( param2 ), LO_UINT16( param2 ) );
//      break;
//
//    case TP_SEND_ROUTE_DISCOVERY:
//      if ( ZG_BUILD_RTR_TYPE && ZG_DEVICE_RTR_TYPE )
//      {
//        retValue = TestProfileApp_SendRouteDiscovery( param1,
//                                    HI_UINT16( param2 ), LO_UINT16( param2 ) );
//      }
//      break;
//
//    case TP_SEND_NEW_ADDR:
//      retValue = TestProfileApp_ChangeShortAddr( param1, LO_UINT16(param2) );
//      break;
//
//    case TP_SEND_NWK_UPDATE:
//      /* Send out a Network Update command. */
//      retValue = NLME_SendNetworkUpdate( NWK_BROADCAST_SHORTADDR, NWKUPDATE_PANID_UPDATE,
//                                        _NIB.extendedPANID, _NIB.nwkUpdateId+1, param1 );
//      break;
//
//    case TP_NWK_ADDR_CONFLICT:
//      retValue = NLME_SendNetworkStatus( NWK_BROADCAST_SHORTADDR_DEVZCZR, param1,
//                                         NWKSTAT_ADDRESS_CONFLICT, FALSE );
//      break;
//
// #if (ZG_BUILD_JOINING_TYPE)
//    case TP_AK_SETUP_PARTNER:
//      retValue = TestProfileApp_AppKeySetupPartner( srcEp, param1, param2 );
//      break;
//
//    case TP_AK_REQ_KEY:
//      retValue = TestProfileApp_AppKeyRequest( srcEp, param1, param2 );
//      break;
//
//    case TP_AK_PARTNER_NWKADDR:
//      retValue = TestProfileApp_SetPartnerNwkAddr( srcEp, param1, param2 );
//      break;
//
//    case TP_AK_PARTNER_EXTADDR7654:
//       retValue = TestProfileApp_SetPartnerExtAddr7654( srcEp, param1, param2 );
//      break;
//
//    case TP_AK_PARTNER_EXTADDR3210:
//      retValue = TestProfileApp_SetPartnerExtAddr3210( srcEp, param1, param2 );
//      break;
//
//    case TP_AK_PARTNER_SET:
//      retValue = TestProfileApp_SetPartner( srcEp, param1, param2 );
//      break;
//#endif /* ZG_BUILD_JOINING_TYPE */
//
//#if (ZG_BUILD_COORDINATOR_TYPE)
//    case TP_AK_TYPE_SET:
//      retValue = TestProfileApp_AppKeyTypeSet( srcEp, param1, param2 );
//      break;
//#endif /* ZG_BUILD_COORDINATOR_TYPE */
//
//#if defined ( ZIGBEE_FRAGMENTATION )
//    case TP_FRAG_SKIP_BLOCK:
//      retValue = TestProfileApp_FragSkipBlock( (uint8)param1 );
//      break;
//#endif
//
//    case TP_APS_REMOVE:
//      retValue = TestProfileApp_APSRemove( param1, param2 );
//      break;
//#endif // APP_TP2
//
//#endif  // APP_TP || APP_TP2
//
//#if defined ( OSAL_TOTAL_MEM )
//    case OSAL_MEM_STACK_HIGH_WATER:
//    case OSAL_MEM_HEAP_HIGH_WATER:
//      if ( app_cmd == OSAL_MEM_STACK_HIGH_WATER)
//      {
//        param1 = osal_stack_used();
//      }
//      else
//      {
//        param1 = osal_heap_high_water();
//      }
//
//      pData[0] = LO_UINT16( param1 );
//      pData[1] = HI_UINT16( param1 );
//
//      MT_BuildAndSendZToolResponse((MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_APP), cmdId, 2, pData);
//      return;
//#endif
//
//#if defined ( APP_DEBUG )
//    case DEBUG_GET:
//      DebugApp_SendQuery( param1 );
//      retValue = ZSUCCESS;
//      break;
//#endif
//
//#if defined ( APP_TP2 )
//    case TP_SEND_BCAST_RSP:
//      retValue = TestProfileApp_SendBcastRsp( srcEp, (byte)param1 );
//      break;
//#endif
//
//    default:
//      break;
//  }
//#endif // (APP_TGEN) || (NWK_TEST) || (APP_TP) || (APP_TP2) || (OSAL_TOTAL_MEM) || (APP_DEBUG)
//
//  /* Build and send back the response */
//  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_APP), cmdId, 1, &retValue);
//}

#endif /* MT_HAGATE_FUNC */

/***************************************************************************************************
 ***************************************************************************************************/
