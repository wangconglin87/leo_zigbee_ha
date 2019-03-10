/*********************************************************************
* INCLUDES
*/
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "SampleApp.h"
#include "SampleAppHw.h"

#include "OnBoard.h"

// This list should be filled with Application specific Cluster IDs.
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
	SAMPLEAPP_PERIODIC_CLUSTERID,
	SAMPLEAPP_FLASH_CLUSTERID,
	LEO_CLUSTERID
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
	SAMPLEAPP_ENDPOINT,              //  int Endpoint;
	SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
	SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
	SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
	SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
	SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
	(cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
	SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
	(cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in SampleApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t SampleApp_epDesc;

/*********************************************************************
* EXTERNAL VARIABLES
*/

/*********************************************************************
* EXTERNAL FUNCTIONS
*/

/*********************************************************************
* LOCAL VARIABLES
*/
uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
// This variable will be received when
// SampleApp_Init() is called.
devStates_t SampleApp_NwkState;

uint8 SampleApp_TransID;  // This is the unique message ID (counter)

afAddrType_t SampleApp_Periodic_DstAddr;
afAddrType_t SampleApp_Flash_DstAddr;

aps_Group_t SampleApp_Group;

uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;

/*********************************************************************
* LOCAL FUNCTIONS
*/
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void Handle_UartEvent(uint8 port, uint8 event);

/*********************************************************************
* NETWORK LAYER CALLBACKS
*/

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*********************************************************************
* @fn      SampleApp_Init
*
* @brief   Initialization function for the Generic App Task.
*          This is called during initialization and should contain
*          any application specific initialization (ie. hardware
*          initialization/setup, table initialization, power up
*          notificaiton ... ).
*
* @param   task_id - the ID assigned by OSAL.  This ID should be
*                    used to send messages and set timers.
*
* @return  none
*/
void SampleApp_Init( uint8 task_id )
{
	SampleApp_TaskID = task_id;
	SampleApp_NwkState = DEV_INIT;
	SampleApp_TransID = 0;
	
	  // Setup for the periodic message's destination address
  // Broadcast to everyone
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;
	
	// Fill out the endpoint description.
	SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
	SampleApp_epDesc.task_id = &SampleApp_TaskID;
	SampleApp_epDesc.simpleDesc
		= (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
	SampleApp_epDesc.latencyReq = noLatencyReqs;
	
	// Register the endpoint description with the AF
	afRegister( &SampleApp_epDesc );
	
    halUARTCfg_t uartconf;
    uartconf.baudRate = HAL_UART_BR_115200;
    uartconf.callBackFunc = Handle_UartEvent;
    uartconf.configured = TRUE;
    uartconf.flowControl = FALSE;
    uartconf.flowControlThreshold = 64;
    uartconf.idleTimeout = 6;
    uartconf.rx.maxBufSize = 128;
    uartconf.tx.maxBufSize = 128;
    uartconf.intEnable = TRUE;
    HalUARTOpen(HAL_UART_PORT_0, &uartconf);
}

/*********************************************************************
* @fn      SampleApp_ProcessEvent
*
* @brief   Generic Application Task event processor.  This function
*          is called to process all events for the task.  Events
*          include timers, messages and any other user defined events.
*
* @param   task_id  - The OSAL assigned task ID.
* @param   events - events to process.  This is a bit map and can
*                   contain more than one event.
*
* @return  none
*/
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
	afIncomingMSGPacket_t *MSGpkt;
	(void)task_id;  // Intentionally unreferenced parameter
	
	if ( events & SYS_EVENT_MSG )
	{
		MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
		while ( MSGpkt )
		{
			switch ( MSGpkt->hdr.event )
			{
			// Received when a messages is received (OTA) for this endpoint
			case AF_INCOMING_MSG_CMD:
				SampleApp_MessageMSGCB( MSGpkt );
				break;
				
			default:
				break;
			}
			
			// Release the memory
			osal_msg_deallocate( (uint8 *)MSGpkt );
			
			// Next - if one is available
			MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
		}
		
		// return unprocessed events
		return (events ^ SYS_EVENT_MSG);
	}
	
	return 0;
}

/*********************************************************************
* LOCAL FUNCTIONS
*/

/*********************************************************************
* @fn      SampleApp_MessageMSGCB
*
* @brief   Data message processor callback.  This function processes
*          any incoming data - probably from other devices.  So, based
*          on cluster ID, perform the intended action.
*
* @param   none
*
* @return  none
*/
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
	//uint16 flashTime;
	
	switch ( pkt->clusterId )
	{
	case LEO_CLUSTERID:
		HalUARTWrite(HAL_UART_PORT_0, pkt->cmd.Data, pkt->cmd.DataLength);
		break;
	}
}


/*********************************************************************
*********************************************************************/
static void Handle_UartEvent(uint8 port, uint8 event) {
    if (port == HAL_UART_PORT_0) {
        switch (event) {
        case HAL_UART_RX_FULL:
        case HAL_UART_RX_ABOUT_FULL:
        case HAL_UART_RX_TIMEOUT: {
            uint16 rxbuflen = Hal_UART_RxBufLen(HAL_UART_PORT_0);
            uint8* rxbuf = osal_mem_alloc(rxbuflen);
			
            if (rxbuf != NULL) {
                HalUARTRead(HAL_UART_PORT_0, rxbuf, rxbuflen);
                //HalUARTWrite(HAL_UART_PORT_0, rxbuf, rxbuflen);
                AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
							   LEO_CLUSTERID,
							   rxbuflen,
							   rxbuf,
							   &SampleApp_TransID,
							   AF_DISCV_ROUTE,
							   AF_DEFAULT_RADIUS);
            }
			
            osal_mem_free(rxbuf);
        }
        break;
		
        default:
            break;
        }
    }
}
