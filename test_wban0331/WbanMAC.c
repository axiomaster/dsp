/*********************************************************************
 *
 *                  WbanMAC C File
 *
 *********************************************************************
 * FileName:        WbanMAC.c
 * Company:         Xidian University
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Yueyang Song       2013.10.12
 *Yueyang Song 		 2013.11.18		v1.2
 *Yueyang Song		 2014.11.22		v2.0
 *Yanle Lei			 2014.11.22		v2.0
 ********************************************************************/
#include "CpuTimers.h"
#include "WbanTasks.h"
#include "WbanMAC.h"
#include "WbanSecurity.h"
#include "WbanPHY.h"
#include "AES.h"

BYTE mac_bufferTemp[mac_PACKET_BUFFER_SIZE];
WORD mac_bufferTempLength;		//用于指示bufferTemp[]中的有效数据的多少
DataQueue mac_dataQueue;
MAC_TASKS_PENDING mac_TasksPending;
MAC_FRAME_CONTROL mac_frameControl;
WORD mac_packetHeader, mac_packetFCS;
MAC_BIB macBIB;
CURRENT_PACKET mac_currentPacket;
BYTE mac_TxBuffer[mac_PACKET_BUFFER_SIZE];
BYTE mac_CW, mac_BC;
double mac_CP;
WORD mac_i, mac__i;
unsigned char **expandedkey; //安全用到的参数
BYTE mac_s;
BYTE mac_slotNumber;

/**********************************************************************
**函数原型:		void MACInit()
**入口参数:   	void
**返   回 	值：		void
**说      	明：		初始化MAC层
***********************************************************************/
void MACInit()
{
#ifdef I_AM_NODE
	printf( "当前节点角色为NODE\n" );
#endif
#ifdef I_AM_HUB
	printf( "当前节点角色为HUB\n" );
#endif

	InitCpuTimers();
	PrimitiveInit();
	params.MLME_RESET_request.setDefaultBIB = TRUE;
	MACTasks(MLME_RESET_request);
}

/**********************************************************************
**函数原型:		BYTE MACGet()
**入口参数:   	void
**返   回 	值：		BYTE
**说      	明：		获取由物理层向MAC层传输的psdu的字节
***********************************************************************/
BYTE MACGet(){
	params.PL_DATA_indication.psduLength--;
	return *params.PL_DATA_indication.psdu++;
}

/**********************************************************************
**函数原型:		WBAN_PRIMITIVE MACTasks(WBAN_PRIMITIVE inputPrimitive)
**入口参数:   	WBAN_PRIMITIVE inputPrimitive
**返   回 	值：		WBAN_PRIMITIVE
**说      	明：		MAC层原语处理程序，根据不同的原语，作出相应的处理
***********************************************************************/
WBAN_PRIMITIVE MACTasks(WBAN_PRIMITIVE inputPrimitive){
#ifdef I_AM_HUB
	uint64_t temp;
	BYTE mac_c;
#endif
	BOOL result = FALSE;

	switch(inputPrimitive){
	case NO_PRIMITIVE:
		break;

	case MLME_RESET_request:
#ifdef DEBUG
		printf("\n开始原语MIME_RESET_request配置... @MACTasks(...) @%ld\n", TickGet().Val);
#endif
		if(params.MLME_RESET_request.setDefaultBIB == TRUE){
#ifdef I_AM_HUB
			macBIB.abbrAddr.v = HUB_ABBR_ADDR_DEFAULT;
			macBIB.BANID.v = mac_BANID_DEFAULT;					//目前只支持一个BAN
			macBIB.listNodeSum = 0;
			macBIB.band = WBAN_BAND;
//			macBIB.channelNumber = 0;
			macBIB.scramblerSeed = SCRAMBLER_SEED_0;
			macBIB.AccessIndicator = mac_BEACON_WITH_SUPERFRAME_CSMA;
			for(mac_i = 0; mac_i < mac_mMaxBANSize;mac_i++){
				macBIB.nodeList[mac_i].isUsed = FALSE;
			}
			for(mac_i = 0; mac_i < mac_mMaxABBRADDRSize;mac_i++){
				macBIB.abbraddrList[mac_i].isUsed = FALSE;
						}
			for(mac_i = 0; mac_i < mac_mMaxBANSize; mac_i++){
				macBIB.macTxDSN[mac_i] = 0;
				macBIB.macRxDSN[mac_i] = 0;
			}

			macBIB.allocationSlotLength = mac_INIT_ALLOCATION_SLOT_LENGTH_DEFAULT;
			macBIB.beaconPeriodLength = mac_INIT_BEACON_PERIOD_LENGTH_DEFAULT;
			macBIB.BeaconStartTime.Val = TickGet().Val;
			temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
			macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
			macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
			macBIB.EAP2Start.Val = mac_EAP2_START_DEFAULT * temp;
			macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
			macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;
			mac_slotNumber = mac_RAP1_END_DEFAULT;

			macBIB.capability.Val = 0;
			//配置Capability
			macBIB.capability.bits.CSMACA = 1;
			macBIB.capability.bits.TypeIPollingAccess = 1;
			macBIB.capability.bits.DataRate0 = 1;
			macBIB.capability.bits.DataRate1 = 1;
			macBIB.capability.bits.DataRate2 = 1;
			macBIB.capability.bits.DataRate3 = 1;
			macBIB.capability.bits.ScheduledAccess = 1;
			macBIB.capability.bits.Fragmentation_Reassembly = 1;
			macBIB.capability.bits.DataSubtypes = 8;
			macBIB.capability.bits.NodeAlwaysActive_HubClockPPM = 1;
#ifdef DEBUG
			printf( "BAN地址：0x%x\n", macBIB.BANID.v );
			printf( "频段：0x%x\n", macBIB.band );
			printf( "信道号：0x%x\n", macBIB.channelNumber );
			printf( "时隙长度：%d\n", macBIB.allocationSlotLength );
			printf( "超帧长度：%d\n", macBIB.beaconPeriodLength );
			printf( "RAP1开始时刻：%ld ", macBIB.RAP1Start.Val );
			printf( "RAP1结束时刻：%ld\n", macBIB.RAP1End.Val );
			printf( "RAP2开始时刻：%ld ", macBIB.RAP2Start.Val );
			printf( "RAP2结束时刻：%ld\n", macBIB.RAP2End.Val );
			printf( "nodeList列表清零\n" );
#endif
#endif
#ifdef I_AM_NODE
			macBIB.abbrAddr.v = mac_UNCONNECTED_BROADCAST_NID;		//Unconnected_Broadcast_NID
			macBIB.capability.Val = 0;
			//配置Capability
			macBIB.capability.bits.CSMACA = 1;
			macBIB.capability.bits.TypeIPollingAccess = 1;
			macBIB.capability.bits.DataRate0 = 1;
			macBIB.capability.bits.DataRate1 = 1;
			macBIB.capability.bits.DataRate2 = 1;
			macBIB.capability.bits.DataRate3 = 1;
			macBIB.capability.bits.ScheduledAccess = 1;
			macBIB.capability.bits.Fragmentation_Reassembly = 1;
			macBIB.capability.bits.DataSubtypes = 8;
			macBIB.capability.bits.NodeAlwaysActive_HubClockPPM = 1;

			macBIB.macTxDSN = 0;
			macBIB.macRxDSN = 0;
#endif
		}
		macBIB.informationDataRate = DATA_RATE_485_7;
		macBIB.macBSN = 0;
		macBIB.fullAddr.v[0] = getMACFullAddr(0);
		macBIB.fullAddr.v[1] = getMACFullAddr(1);
		macBIB.fullAddr.v[2] = getMACFullAddr(2);
		macBIB.fullAddr.v[3] = getMACFullAddr(3);
		macBIB.fullAddr.v[4] = getMACFullAddr(4);
		macBIB.fullAddr.v[5] = getMACFullAddr(5);
		macBIB.IntervalEnd.Val = 0;
		macBIB.IntervalStart.Val = 0;
		macBIB.beaconPeriodLength = 0xFF;
		macBIB.BeaconStartTime.Val = 0;

		macBIB.isEncrypt = FALSE;

		mac_packetHeader = mac_PACKET_HEADER_START;
		mac_packetFCS = mac_PACKET_FCS_START;

		mac_currentPacket.lastPacketStatus = mac_NONE;
		mac_currentPacket.retry = 0;

		params.MLME_RESET_confirm.status = mac_SUCCESS;
#ifdef DEBUG
		printf( "简明地址：0x%x\n", macBIB.abbrAddr.v );
		printf( "完全地址（倒序排列）：");
		for( mac_i = 0; mac_i < 6;mac_i++ )
			printf( "0x%x ", macBIB.fullAddr.v[mac_i] );
		printf( "\n" );
		printf( "发送速率：0x%x\n", macBIB.informationDataRate );
		printf( "支持的功能：0x%x ", macBIB.capability.bytes.b0 );
		printf( "0x%x ", macBIB.capability.bytes.b1 );
		printf( "0x%x ", macBIB.capability.bytes.b2 );
		printf( "0x%x\n", macBIB.capability.bytes.b3 );
		printf( "macBSN, macTxDSN, macRxDSN清零\n" );
#endif
		return MLME_RESET_confirm;

	case MLME_SCAN_request:
		params.MLME_SCAN_confirm.Status = Scan(params.MLME_SCAN_request.ScanType, params.MLME_SCAN_request.ChannelFreBand, params.MLME_SCAN_request.ChannelNum);
#ifdef DEBUG
		printf("信道扫描结果为 0x%x\n", params.MLME_SCAN_confirm.Status);
#endif
		return MLME_SCAN_confirm;

#ifdef I_AM_HUB
	case MLME_START_request:
#ifdef DEBUG
		printf("开始更新macBIB... @MACTasks(...) @%ld\n", TickGet().Val);
#endif
		macBIB.BANID.v = params.MLME_START_request.BANID.v;
		if(params.MLME_START_request.isDefault == TRUE){
			macBIB.allocationSlotLength = mac_ALLOCATION_SLOT_LENGTH_DEFAULT;
			macBIB.beaconPeriodLength = mac_BEACON_PERIOD_LENGTH_DEFAULT;
			temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
			macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
			macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
			macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
			macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;

			params.MLME_START_request.RAP1Start = mac_RAP1_START_DEFAULT;
			params.MLME_START_request.RAP1End = mac_RAP1_END_DEFAULT;
			params.MLME_START_request.RAP2Start = mac_RAP2_START_DEFAULT;
			params.MLME_START_request.RAP2End = mac_RAP2_END_DEFAULT;
		}
		else{
			macBIB.allocationSlotLength = params.MLME_START_request.allocationSlotLength;
			macBIB.beaconPeriodLength = params.MLME_START_request.beaconPeriodLength;
			temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
			macBIB.RAP1Start.Val = params.MLME_START_request.RAP1Start * temp;
			macBIB.RAP1End.Val = params.MLME_START_request.RAP1End * temp;
			macBIB.RAP2Start.Val = params.MLME_START_request.RAP2Start * temp;
			macBIB.RAP2End.Val = params.MLME_START_request.RAP2End * temp;
		}
#ifdef DEBUG
		printf("开始装配Beacon帧...\n");
#endif
		//帧头包装
		mac_packetHeader = mac_PACKET_HEADER_START;
		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = ++macBIB.macBSN;
		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = mac_BROADCAST_NID;
		mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
		mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

		//payload包装
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];
		mac_TxBuffer[mac_packetHeader++] = macBIB.beaconPeriodLength;
		mac_TxBuffer[mac_packetHeader++] = macBIB.allocationSlotLength;
		mac_TxBuffer[mac_packetHeader++] = params.MLME_START_request.RAP1End;
		mac_TxBuffer[mac_packetHeader++] = params.MLME_START_request.RAP2Start;
		mac_TxBuffer[mac_packetHeader++] = params.MLME_START_request.RAP2End;
		mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b0;
		mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b1;
		mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b2;
		mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b3;
		mac_TxBuffer[mac_packetHeader++] = params.MLME_START_request.RAP1Start;
		//帧尾包装，校验和，暂缺
		mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
		mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

		macBIB.macBeaconPayload[0] = macBIB.fullAddr.v[0];
		macBIB.macBeaconPayload[1] = macBIB.fullAddr.v[1];
		macBIB.macBeaconPayload[2] = macBIB.fullAddr.v[2];
		macBIB.macBeaconPayload[3] = macBIB.fullAddr.v[3];
		macBIB.macBeaconPayload[4] = macBIB.fullAddr.v[4];
		macBIB.macBeaconPayload[5] = macBIB.fullAddr.v[5];
		macBIB.macBeaconPayload[6] = macBIB.beaconPeriodLength;
		macBIB.macBeaconPayload[7] = macBIB.allocationSlotLength;
		macBIB.macBeaconPayload[8] = params.MLME_START_request.RAP1End;
		macBIB.macBeaconPayload[9] = params.MLME_START_request.RAP2Start;
		macBIB.macBeaconPayload[10] = params.MLME_START_request.RAP2End;
		macBIB.macBeaconPayload[11] = macBIB.capability.bytes.b0;
		macBIB.macBeaconPayload[12] = macBIB.capability.bytes.b1;
		macBIB.macBeaconPayload[13] = macBIB.capability.bytes.b2;
		macBIB.macBeaconPayload[14] = macBIB.capability.bytes.b3;
		macBIB.macBeaconPayload[15] = params.MLME_START_request.RAP1Start;

		mac_TasksPending.bit.ackSending = 1;	 //与ack处理方法相同
		params.MLME_START_confirm.status = mac_SUCCESS;

		return MLME_START_confirm;

	case HME_MLME_CONNECT_request:
		// 根据原语中AccessIndicator，选择工作模式
		macBIB.BeaconStartTime = TickGet();
		mac_s = getCurrentSlotNumber();
		//macBIB.informationDataRate = params.HME_MLME_CONNECT_request.informationDataRate;
		params.HME_MLME_CONNECT_request.AccessIndicator = macBIB.AccessIndicator;
		switch(params.HME_MLME_CONNECT_request.AccessIndicator){
		case mac_NONBEACON_WITHOUT_SUPERFRAME:			//没有超帧不带信标模式  csma接入
			break;
		case mac_NONBEACON_WITH_SUPERFRAME:	            //超帧不带信标模式        无自由接入
			break;
		case mac_BEACON_WITH_SUPERFRAME_ALOHA:          //超帧带信标    aloha接入
			break;
	    case mac_BEACON_WITH_SUPERFRAME_CSMA:      	    //超帧带信标  csma接入
	        macBIB.PollPostWindow = mac_s + mac_POLLPOSTWINDOW_DEFAULT;
	        macBIB.Next = mac_NEXT_DEFAULT;
	        macBIB.SecurityLevel = params.HME_MLME_CONNECT_request.SecurityLevel;
	        macBIB.BANID.v = params.HME_MLME_CONNECT_request.BANID.v;
#ifdef DEBUG
	        printf("CurrentSlotNumber %d\n", mac_s);
	        printf("macBIB.PollPostWindow %d\n", macBIB.PollPostWindow);
#endif
	        if( macBIB.PollPostWindow > mac_INIT_BEACON_PERIOD_LENGTH_DEFAULT){
#ifdef DEBUG
	        printf( "**macBIB.PollPostWindow is bigger than mac_INIT_BEACON_PERIOD_LENGTH_DEFAULT(255)\n " );
#endif
               return HME_MLME_CONNECT_request;
	        }
	        else
#ifdef DEBUG
	        printf("\n开始T_Poll成帧...\n");
#endif
			mac_packetHeader = mac_PACKET_HEADER_START;

			mac_TxBuffer[mac_packetHeader++] = (macBIB.SecurityLevel << 3);
			mac_TxBuffer[mac_packetHeader] = 0x00 << 7;                     //Accessmode
			mac_TxBuffer[mac_packetHeader] |= (0x00 << 6);					//MoreData
			mac_TxBuffer[mac_packetHeader] |= (0x01 << 4);					//ControlFrame
			mac_TxBuffer[mac_packetHeader] |= 0x07;							//TPollFrame
			mac_packetHeader++;

			mac_TxBuffer[mac_packetHeader++] = mac_POLLPOSTWINDOW_DEFAULT;
			mac_TxBuffer[mac_packetHeader++] = (mac_NEXT_DEFAULT & 0x07);
			mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_BROADCAST_NID;
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

			//写T-Poll的payload

			mac_TxBuffer[mac_packetHeader++] = mac_s;
			//mac_TxBuffer[mac_packetHeader++] = 0x00;

	/*		uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
			WORD f = (WORD)((TickGet().Val - macBIB.BeaconStartTime.Val - mac_s * temp) / mac_CLK_Freq);
			mac_TxBuffer[mac_packetHeader++] = (f & 0xFF);
			mac_TxBuffer[mac_packetHeader++] = ((f >> 8) & 0xFF);
	*/
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = mac_INIT_ALLOCATION_SLOT_LENGTH_DEFAULT;
			//帧尾包装，校验和，暂缺
			mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

			mac_TasksPending.bit.packetWaitingConnectionRequest = 1;
			return  MACFrameTransmission(  mac_T_POLL_FRAME, mac_DIRECT, mac_TxBuffer, 13 );

	    default:
	    	;
#ifdef DEBUG
	    	printf( "**ERROR: Can't find this case! @HME_MLME_CONNECT_request " );
#endif
		}


		break;

	case HME_MLME_CONNECT_response:
		mac_c = getIndexFromNodeList( params.HME_MLME_CONNECT_response.AssocAbbrID );

		if(params.HME_MLME_CONNECT_response.ConnectionStatus == 0){

		}
		else{
#ifdef DEBUG
	    	printf( "网络层指示请求连接失败，开始配置Connection Assignment Frame " );
#endif
	    	mac_packetHeader = mac_PACKET_HEADER_START;
	    	mac_TxBuffer[mac_packetHeader] = (0x01 << 1);                         //mac_I_ACK = 0x01,
	    	mac_TxBuffer[mac_packetHeader] |= (macBIB.SecurityLevel << 3);
	    	mac_packetHeader++;

			mac_TxBuffer[mac_packetHeader] = (mac_MANAGEMENT_FRAME << 4);          //Management Frame
			mac_TxBuffer[mac_packetHeader] |= mac_CONNECTION_ASSIGNMENT_FRAME;	  //Connection Assignment Frame
			mac_TxBuffer[mac_packetHeader] |= (0x01 << 7);                       //Last Frame
			mac_packetHeader++;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = 0x00;

			mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[getIndexFromNodeList(params.HME_MLME_CONNECT_response.AssocAbbrID)].abbrAddr.v;
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;
			//写connection assignment的payload
			mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[0];        //RecipientAdress
			mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[1];
			mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[2];
			mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[3];
			mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[4];
			mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[5];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];                       //SenderAddress
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];
			mac_TxBuffer[mac_packetHeader] = (macBIB.AccessIndicator & 0x03);              //Mode_Status
			mac_TxBuffer[mac_packetHeader++] |= (params.HME_MLME_CONNECT_response.ConnectionStatus << 2);
			mac_TxBuffer[mac_packetHeader++] = macBIB.macBSN;                           //Current Superframe Number
			mac_TxBuffer[mac_packetHeader++] = mac_RAP1_END_DEFAULT;               //Earliest RAP1 End
			mac_TxBuffer[mac_packetHeader++] = mac_EAP2_START_DEFAULT;                           //EAP2 Start
			mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b0;              //MAC capability
			mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b1;
			mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b2;
			mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b3;             //PHY capability
			mac_TxBuffer[mac_packetHeader++] = 0x00;                                   //Connetion Change Indicator
			mac_TxBuffer[mac_packetHeader++] = 0x11;                                   //Requested Ack Data Rates
			mac_TxBuffer[mac_packetHeader++] = 0x00;                                   //Assigned Wakeup Period
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = 0x00;                                   //Assigned Wakeup Phase
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			//帧尾包装，校验和，暂缺
			mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

			DeNodeFromNodeList(params.HME_MLME_CONNECT_response.AssocAbbrID);
			return MACFrameTransmission( mac_CONNECTION_ASSIGNMENT_FRAME, mac_DIRECT, mac_TxBuffer, 36 );
		}

	    //BYTE mac_c;
	    //mac_c = getIndexFromNodeList( params.HME_MLME_CONNECT_response.AssocAbbrID );
	    if(mac_slotNumber + macBIB.uplinkRequestIE.AllocationLength > mac_RAP2_START_DEFAULT){
#ifdef DEBUG
	    		printf( "时隙超出，请求连接失败，开始配置Connection Assignment Frame " );
#endif
	    		mac_packetHeader = mac_PACKET_HEADER_START;
	    		mac_TxBuffer[mac_packetHeader] = (0x01 << 1);                          //mac_I_ACK = 0x01,
	    		mac_packetHeader++;

	    		mac_TxBuffer[mac_packetHeader] = (mac_MANAGEMENT_FRAME << 4);          //Management Frame
	    		mac_TxBuffer[mac_packetHeader] |= mac_CONNECTION_ASSIGNMENT_FRAME;	  //Connection Assignment Frame
	    		mac_TxBuffer[mac_packetHeader] |= (0x01 << 7);                       //Last Frame
				mac_packetHeader++;
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;

				mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[getIndexFromNodeList(params.HME_MLME_CONNECT_response.AssocAbbrID)].abbrAddr.v;
				mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
				mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;
				//写connection assignment的payload
				mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[0]; //RecipientAdress
				mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[1];
				mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[2];
				mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[3];
				mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[4];
				mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[5];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];                       //SenderAddress
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];
				mac_TxBuffer[mac_packetHeader] = (macBIB.AccessIndicator & 0x03);          //Mode_Status
				mac_TxBuffer[mac_packetHeader++] |= (0x04 << 2);
				mac_TxBuffer[mac_packetHeader++] = macBIB.macBSN;                           //Current Superframe Number
				mac_TxBuffer[mac_packetHeader++] = mac_RAP1_END_DEFAULT;               //Earliest RAP1 End
				mac_TxBuffer[mac_packetHeader++] = mac_EAP2_START_DEFAULT;                           //EAP2 Start
				mac_TxBuffer[mac_packetHeader++] = mac_Minimum_CAP_Length;
				mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b0;              //MAC capability
				mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b1;
				mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b2;
				mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b3;                     //PHY capability
				mac_TxBuffer[mac_packetHeader++] = 0x00;                                   //Connetion Change Indicator
				mac_TxBuffer[mac_packetHeader++] = 0x11;                                   //Requested Ack Data Rates
				mac_TxBuffer[mac_packetHeader++] = 0x00;                                   //Assigned Wakeup Period
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;                                   //Assigned Wakeup Phase
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				//帧尾包装，校验和，暂缺
				mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
				mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

				DeNodeFromNodeList(params.HME_MLME_CONNECT_response.AssocAbbrID);
				return MACFrameTransmission( mac_CONNECTION_ASSIGNMENT_FRAME, mac_DIRECT, mac_TxBuffer, 36 );
	    	}
	    	else{
#ifdef DEBUG
	    		printf( "请求连接成功，开始配置Connection Assignment Frame " );
#endif
		    	macBIB.SecurityLevel = params.HME_MLME_CONNECT_response.SecurityLevel;
		    	mac_packetHeader = mac_PACKET_HEADER_START;
		    	mac_TxBuffer[mac_packetHeader] = (0x01 << 1);                         //mac_I_ACK = 0x01,
		    	mac_TxBuffer[mac_packetHeader] |= (macBIB.SecurityLevel << 3);
		    	mac_packetHeader++;

		    	mac_TxBuffer[mac_packetHeader] = (mac_MANAGEMENT_FRAME << 4);        //Management Frame
		    	mac_TxBuffer[mac_packetHeader] |= mac_CONNECTION_ASSIGNMENT_FRAME;	 //Connection Assignment Frame
		    	mac_TxBuffer[mac_packetHeader] |= (0x01 << 7);                       //Last Frame
		    	mac_packetHeader++;
		    	mac_TxBuffer[mac_packetHeader++] = 0x00;
		    	mac_TxBuffer[mac_packetHeader++] = 0x00;

		    	mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].abbrAddr.v;
		        mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

		    	//写connection assignment的payload
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[0];         //RecipientAdress
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[1];
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[2];
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[3];
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[4];
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[mac_c].fullAddr.v[5];
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];                       //SenderAddress
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];
		    	mac_TxBuffer[mac_packetHeader] = (macBIB.AccessIndicator & 0x03);              //Mode_Status
		    	mac_TxBuffer[mac_packetHeader++] |= (params.HME_MLME_CONNECT_response.ConnectionStatus << 2);
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.macBSN;                              //Current Superframe Number
		    	mac_TxBuffer[mac_packetHeader++] = mac_RAP1_END_DEFAULT;                      //Earliest RAP1 End
		    	mac_TxBuffer[mac_packetHeader++] = mac_EAP2_START_DEFAULT;                             //EAP2 Start
		    	mac_TxBuffer[mac_packetHeader++] = mac_Minimum_CAP_Length;
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b0;              //MAC capability
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b1;
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b2;
		    	mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b3;             //PHY capability
		    	mac_TxBuffer[mac_packetHeader++] = 0x00;                                   //Connetion Change Indicator
		    	mac_TxBuffer[mac_packetHeader++] = 0x11;                                   //Requested Ack Data Rates
		    	mac_TxBuffer[mac_packetHeader++] = 0x00;                                   //Assigned Wakeup Phase
		    	mac_TxBuffer[mac_packetHeader++] = 0x00;
		    	mac_TxBuffer[mac_packetHeader++] = 0x01;                                   //Assigned Wakeup Period
		    	mac_TxBuffer[mac_packetHeader++] = 0x00;

		    	mac_TxBuffer[mac_packetHeader++] = 0x01;                                   //ElementID
		    	mac_TxBuffer[mac_packetHeader++] = 0x03;                                   //Length
		    	mac_TxBuffer[mac_packetHeader++] = 0x07;                                   //AllocationAssignment
		    	mac_TxBuffer[mac_packetHeader++] = mac_slotNumber;
		    	mac_TxBuffer[mac_packetHeader++] = AllocationSlot( macBIB.nodeList[mac_c].abbrAddr, macBIB.uplinkRequestIE.AllocationLength);

		    	//帧尾包装，校验和，暂缺
		    	mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
		    	mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

		    	return MACFrameTransmission( mac_CONNECTION_ASSIGNMENT_FRAME, mac_DIRECT, mac_TxBuffer, 41 );
	    }

	case MLME_DISCONNECT_request:
		mac_packetHeader = mac_PACKET_HEADER_START;
		mac_TxBuffer[mac_packetHeader] = (0x01 << 1);                          //mac_I_ACK = 0x01,
		mac_packetHeader++;

		mac_TxBuffer[mac_packetHeader] = (0x00 << 4);                          //Management Frame
		mac_TxBuffer[mac_packetHeader] |= 0x0A;	                             //Disconnection Frame
		mac_packetHeader++;

		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = 0x00;

		mac_TxBuffer[mac_packetHeader++] = params.MLME_DISCONNECT_request.AssocAbbrID.v;
		mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
		mac_TxBuffer[mac_packetHeader++] = params.MLME_DISCONNECT_request.BANID.v;

		//写Disconnection的payload
		mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[getIndexFromNodeList(params.MLME_DISCONNECT_request.AssocAbbrID)].fullAddr.v[0];
		mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[getIndexFromNodeList(params.MLME_DISCONNECT_request.AssocAbbrID)].fullAddr.v[1];
		mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[getIndexFromNodeList(params.MLME_DISCONNECT_request.AssocAbbrID)].fullAddr.v[2];
		mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[getIndexFromNodeList(params.MLME_DISCONNECT_request.AssocAbbrID)].fullAddr.v[3];
		mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[getIndexFromNodeList(params.MLME_DISCONNECT_request.AssocAbbrID)].fullAddr.v[4];
		mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[getIndexFromNodeList(params.MLME_DISCONNECT_request.AssocAbbrID)].fullAddr.v[5];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];                            //SenderAddress
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];
		//帧尾包装，校验和，暂缺
//		mac_currentPacket.length = 21;
		mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
		mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

/*		mac_currentPacket.frameType = mac_MANAGEMENT_FRAME;
		mac_currentPacket.frameSubtype = mac_DISCONNECTION_FRAME;
		mac_TasksPending.bit.frameMngCtrlSending = 1;
*/
		return  MACFrameTransmission(  mac_DISCONNECTION_FRAME, mac_DIRECT, mac_TxBuffer, 21 );
//		break;

#endif

#ifdef I_AM_NODE
	case NME_MLME_CONNECT_request:
		if(macBIB.isAuthenticated == TRUE){
			mac_packetHeader = mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader] = (0x01 << 1);                      //mac_I_ACK = 0x01,
			mac_TxBuffer[mac_packetHeader] |= (macBIB.SecurityLevel << 3);
			mac_packetHeader++;

			mac_TxBuffer[mac_packetHeader] = (macBIB.AccessMode << 7);
			if(GetDataQueueLength() == 0){
				mac_TxBuffer[mac_packetHeader] |= (0x00 << 6);
					}
			else{
				mac_TxBuffer[mac_packetHeader] |= (0x01 << 6);                  //MoreData
				}
				mac_TxBuffer[mac_packetHeader] |= (0x00 << 4);                  //Management Frame
				mac_TxBuffer[mac_packetHeader] |= 0x08;	                        //Connection Request Frame
				mac_TxBuffer[mac_packetHeader] |= (0x01 << 7);                  //Last Frame
				mac_packetHeader++;

				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;

				mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;
				mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;
				mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;
				//写connection request的payload
				mac_TxBuffer[mac_packetHeader++] = macBIB.hubFullAddr.v[0];                         //RecipientAdress
				mac_TxBuffer[mac_packetHeader++] = macBIB.hubFullAddr.v[1];
				mac_TxBuffer[mac_packetHeader++] = macBIB.hubFullAddr.v[2];
				mac_TxBuffer[mac_packetHeader++] = macBIB.hubFullAddr.v[3];
				mac_TxBuffer[mac_packetHeader++] = macBIB.hubFullAddr.v[4];
				mac_TxBuffer[mac_packetHeader++] = macBIB.hubFullAddr.v[5];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];         //SenderAddress
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];
				mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b0;   //MAC capability
				mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b1;
				mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b2;
				mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b3;   //PHY capability
				mac_TxBuffer[mac_packetHeader++] = 0x00;                         //Connetion Change Indicator
				mac_TxBuffer[mac_packetHeader++] = 0x11;                         //Requested Ack Data Rates
				mac_TxBuffer[mac_packetHeader++] = 0x01;                         //Requested Wakeup Period
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;                         //Requested Wakeup Phase
				mac_TxBuffer[mac_packetHeader++] = 0x00;

				mac_TxBuffer[mac_packetHeader++] = 0x01;                         //ElementID
				mac_TxBuffer[mac_packetHeader++] = 0x06;                         //Length
				mac_TxBuffer[mac_packetHeader] = params.NME_MLME_CONNECT_request.FrameSubtype;   //Allocation Length
				mac_TxBuffer[mac_packetHeader] |= (params.NME_MLME_CONNECT_request.UserPriority << 4);
				mac_packetHeader++;
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = GetAllocationLength(params.NME_MLME_CONNECT_request.DataArrivalRateIndicator );
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				//帧尾包装，校验和，暂缺
			    mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
				mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

				return MACFrameTransmission( mac_CONNECTION_REQUEST_FRAME, mac_DIRECT, mac_TxBuffer, 39 );   //传输给物理层

		}
		else{
			mac_packetHeader = mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader] = (0x01 << 1);                      //mac_I_ACK = 0x01,
			mac_TxBuffer[mac_packetHeader] |= (macBIB.SecurityLevel << 3);
			mac_packetHeader++;

			mac_TxBuffer[mac_packetHeader] = (macBIB.AccessMode << 7);
			if(GetDataQueueLength() == 0){
				mac_TxBuffer[mac_packetHeader] |= (0x00 << 6);
					}
			else{
				mac_TxBuffer[mac_packetHeader] |= (0x01 << 6);                  //MoreData
				}
				mac_TxBuffer[mac_packetHeader] |= (0x00 << 4);                  //Management Frame
				mac_TxBuffer[mac_packetHeader] |= 0x08;	                        //Connection Request Frame
				mac_TxBuffer[mac_packetHeader] |= (0x01 << 7);                  //Last Frame
				mac_packetHeader++;

				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;

				mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;
				mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;
				mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;
				//写connection request的payload
				mac_TxBuffer[mac_packetHeader++] = 0x00;                         //RecipientAdress
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];         //SenderAddress
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
				mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];
				mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b0;   //MAC capability
				mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b1;
				mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b2;
				mac_TxBuffer[mac_packetHeader++] = macBIB.capability.bytes.b3;   //PHY capability
				mac_TxBuffer[mac_packetHeader++] = 0x00;                         //Connetion Change Indicator
				mac_TxBuffer[mac_packetHeader++] = 0x11;                         //Requested Ack Data Rates
				mac_TxBuffer[mac_packetHeader++] = 0x01;                         //Requested Wakeup Period
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;                         //Requested Wakeup Phase
				mac_TxBuffer[mac_packetHeader++] = 0x00;

				mac_TxBuffer[mac_packetHeader++] = 0x01;                         //ElementID
				mac_TxBuffer[mac_packetHeader++] = 0x06;                         //Length
				mac_TxBuffer[mac_packetHeader] = params.NME_MLME_CONNECT_request.FrameSubtype;   //Allocation Length
				mac_TxBuffer[mac_packetHeader] |= (params.NME_MLME_CONNECT_request.UserPriority << 4);
				mac_packetHeader++;
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				mac_TxBuffer[mac_packetHeader++] = GetAllocationLength(params.NME_MLME_CONNECT_request.DataArrivalRateIndicator );
				mac_TxBuffer[mac_packetHeader++] = 0x00;
				//帧尾包装，校验和，暂缺
				mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
				mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

			return MACFrameTransmission( mac_CONNECTION_REQUEST_FRAME, mac_PROB, mac_TxBuffer, 39 );   //传输给物理层
		}

	case MLME_DISCONNECT_request:
		mac_packetHeader = mac_PACKET_HEADER_START;
		mac_TxBuffer[mac_packetHeader] = (0x01 << 1);                          //mac_I_ACK = 0x01,
		mac_packetHeader++;

		mac_TxBuffer[mac_packetHeader] |= (0x00 << 4);                          //Management Frame
		mac_TxBuffer[mac_packetHeader] |= 0x0A;	                             //Disconnection Frame
		mac_packetHeader++;

		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = 0x00;

		mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;
		mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
		mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

		//写Disconnection的payload
		mac_TxBuffer[mac_packetHeader++] = macBIB.hubFullAddr.v[0];                        //RecipientAdress
		mac_TxBuffer[mac_packetHeader++] = macBIB.hubFullAddr.v[1];
		mac_TxBuffer[mac_packetHeader++] = macBIB.hubFullAddr.v[2];
		mac_TxBuffer[mac_packetHeader++] = macBIB.hubFullAddr.v[3];
		mac_TxBuffer[mac_packetHeader++] = macBIB.hubFullAddr.v[4];
		mac_TxBuffer[mac_packetHeader++] = macBIB.hubFullAddr.v[5];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];                            //SenderAddress
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];
		//帧尾包装，校验和，暂缺
		mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
		mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

		return MACFrameTransmission( mac_DISCONNECTION_FRAME, mac_DIRECT, mac_TxBuffer, 21 );
#endif

	case MACS_DATA_request:
#ifdef DEBUG
		printf("\n开始原语MACS_DATA_request的配置... @MACTasks(...) @%ld\n", TickGet().Val);
#endif
		if( macBIB.isEncrypt == FALSE ){
			if( params.MACS_DATA_request.securityLevel == mac_LEVEL2 ){
				macBIB.isEncrypt = TRUE;
			}
		}
		//帧头包装
		mac_packetHeader = mac_PACKET_HEADER_START;
		mac_bufferTemp[mac_packetHeader] = (params.MACS_DATA_request.ackPolicy << 1);
		mac_bufferTemp[mac_packetHeader] |= (params.MACS_DATA_request.securityLevel << 3);
		mac_packetHeader++;

		mac_bufferTemp[mac_packetHeader++] = params.MACS_DATA_request.frameTypeSubtype;
#ifdef I_AM_NODE
		macBIB.macTxDSN = params.MACS_DATA_request.TxDSN;
		mac_bufferTemp[mac_packetHeader++] = macBIB.macTxDSN;
#endif
#ifdef I_AM_HUB
		macBIB.macTxDSN[getIndexFromNodeList(params.MACS_DATA_request.RecipientID)] = params.MACS_DATA_request.TxDSN;
		mac_bufferTemp[mac_packetHeader++] = macBIB.macTxDSN[getIndexFromNodeList(params.MACS_DATA_request.RecipientID)];
#endif

		if(macBIB.isEncrypt == TRUE){
			//加密算法需要凑齐16的整数倍，为了解密后数据的有效位数，所以占用这4个bit
			if( (mac_bufferTempLength + mac_DATA_HEAD_LENGTH + mac_DATA_FCS_LENGTH) > ((mac_bufferTempLength + mac_DATA_HEAD_LENGTH + mac_DATA_FCS_LENGTH) / 16) * 16 ){
				WORD t_bufferLen = ( ((mac_bufferTempLength + mac_DATA_HEAD_LENGTH + mac_DATA_FCS_LENGTH) / 16 + 1) * 16 - (mac_bufferTempLength + mac_DATA_HEAD_LENGTH + mac_DATA_FCS_LENGTH) );
				mac_bufferTemp[mac_packetHeader] = (t_bufferLen << 4);
			}
			else{
				mac_bufferTemp[mac_packetHeader] = 0;
			}
		}
		else{
			mac_bufferTemp[mac_packetHeader] = 0;
		}

		mac_bufferTemp[mac_packetHeader] |= (params.MACS_DATA_request.non_finalFragment << 3);
		mac_bufferTemp[mac_packetHeader++] |= params.MACS_DATA_request.fragmentNumber;

		mac_bufferTemp[mac_packetHeader++] = params.MACS_DATA_request.RecipientID.v;
		mac_bufferTemp[mac_packetHeader++] = macBIB.abbrAddr.v;
		mac_bufferTemp[mac_packetHeader] = params.MACS_DATA_request.BANID.v;

		//帧尾包装，校验和，暂缺
		mac_packetHeader = mac_PACKET_FCS_START;
		mac_bufferTemp[mac_packetHeader++] = mac_FCS_LSB;
		mac_bufferTemp[mac_packetHeader] = mac_FCS_MSB;

#ifdef DEBUG
		printf( "装帧完毕\n" );
#endif
		//加入数据队列
		EnDataQueue(mac_bufferTemp, mac_bufferTempLength + mac_DATA_HEAD_LENGTH, (params.MACS_DATA_request.frameTypeSubtype & 0x0F), params.MACS_DATA_request.informationDataRate, params.MACS_DATA_request.ackPolicy);
#ifdef DEBUG
		printf( "加入数据队列...\n" );
#endif
		return NO_PRIMITIVE;

	case PL_DATA_indication:
		mac_TasksPending.bit.dataIndication = 0;

		result = CRC_16_CCITT_check(params.PL_DATA_indication.psdu, params.PL_DATA_indication.psduLength);
		if (result == FALSE){
#ifdef DEBUG1
			printf("CRC校验结果：FAIL!\n");
#endif
			return NO_PRIMITIVE;
		}
		else{
#ifdef DEBUG1
			printf("CRC校验结果：OK!\n");
#endif
		}

		mac_frameControl.bytes.b1 = MACGet();
		mac_frameControl.bytes.b2 = MACGet();
		mac_frameControl.bytes.b3 = MACGet();
		mac_frameControl.bytes.b4 = MACGet();
		params.MACS_DATA_indication.RecipientID.v = MACGet();
		params.MACS_DATA_indication.SenderID.v = MACGet();
		params.MACS_DATA_indication.BANID.v = MACGet();

#ifdef DEBUG
		printf( "FrameSubtype: 0x%x\n", (mac_frameControl.bits.FrameType_Subtype & 0x0F) );
		printf( "FrameType: 0x%x\n",(mac_frameControl.bits.FrameType_Subtype & 0xF0) );
		printf( "RecipientID: 0x%x\n",params.MACS_DATA_indication.RecipientID.v );
		printf( "SenderID: 0x%x\n",params.MACS_DATA_indication.SenderID.v );
		printf( "BANID: 0x%x\n",params.MACS_DATA_indication.BANID.v );
#endif
		//判断收到的帧的地址是不是本机  这部分需要更新
#ifdef I_AM_NODE
		if(mac_currentPacket.frameType_Subtype == mac_CONNECTION_REQUEST_FRAME && mac_frameControl.bits.FrameType_Subtype == mac_I_ACK_FRAME){
			macBIB.abbrAddr.v = params.MACS_DATA_indication.RecipientID.v;
		}
		else if(mac_frameControl.bits.FrameType_Subtype == mac_CONNECTION_ASSIGNMENT_FRAME){
			macBIB.abbrAddr.v = params.MACS_DATA_indication.RecipientID.v;
		}
		if((params.MACS_DATA_indication.RecipientID.v == macBIB.abbrAddr.v) ||
				(params.MACS_DATA_indication.RecipientID.v == mac_UNCONNECTED_BROADCAST_NID) ||
				(params.MACS_DATA_indication.RecipientID.v == mac_UNCONNECTED_NID) ||
				(params.MACS_DATA_indication.RecipientID.v == mac_LOCAL_BROADCAST_NID) ||
				(params.MACS_DATA_indication.RecipientID.v >= 0xF7 && params.MACS_DATA_indication.RecipientID.v <= 0xFD) ||
				(params.MACS_DATA_indication.RecipientID.v == mac_BROADCAST_NID) &&
				(params.MACS_DATA_indication.BANID.v == macBIB.BANID.v) &&
				(mac_frameControl.bits.ProtocolVersion == 0)){

		}
		else{
			return NO_PRIMITIVE;
		}
#endif

#ifdef I_AM_HUB
		if((params.MACS_DATA_indication.RecipientID.v == macBIB.abbrAddr.v) &&
				(params.MACS_DATA_indication.BANID.v == macBIB.BANID.v) &&
				(mac_frameControl.bits.ProtocolVersion == 0)){

		}
		else{
			return NO_PRIMITIVE;
		}
#endif

		//修改解密后的有效位数，如果不加密或加密位数为16的倍数，则位数不变
		params.PL_DATA_indication.psduLength = params.PL_DATA_indication.psduLength - mac_frameControl.bits.Reserved_leftBYTEs;

#ifdef DEBUG
		printf("\n接收到的数据长度修正为：%d\n", params.PL_DATA_indication.psduLength + mac_DATA_HEAD_LENGTH);
#endif

		switch( mac_frameControl.bits.FrameType_Subtype ){
		case mac_BEACON_FRAME:
		case mac_CONNECTION_REQUEST_FRAME:
		case mac_CONNECTION_ASSIGNMENT_FRAME:
		case mac_DISCONNECTION_FRAME:
		case mac_COMMAND_FRAME:
		case mac_B_ACK_FRAME:
		case mac_I_ACK_POLL_FRAME:
		case mac_B_ACK_POLL_FRAME:
		case mac_POLL_FRAME:
		case mac_T_POLL_FRAME:
		case mac_WAKEUP_FRAME:
		case mac_B2_FRAME:
			return MACMngCtrlFrameReception( mac_frameControl.bits.FrameType_Subtype );

		case mac_DATA_FRAME_USER_PRIORITY_0:
		case mac_DATA_FRAME_USER_PRIORITY_1:
		case mac_DATA_FRAME_USER_PRIORITY_2:
		case mac_DATA_FRAME_USER_PRIORITY_3:
		case mac_DATA_FRAME_USER_PRIORITY_4:
		case mac_DATA_FRAME_USER_PRIORITY_5:
		case mac_DATA_FRAME_USER_PRIORITY_6:
		case mac_DATA_FRAME_EMERGENCY:
			return MACDataFrameReception();

		case mac_I_ACK_FRAME:
			switch( mac_currentPacket.frameType_Subtype ){
			case mac_SECURITY_ASSOCIATION_FRAME:
			case mac_SECURITY_DISASSOCIATION_FRAME:
			case mac_PTK_FRAME:
			case mac_GTK_FRAME:
				return SecurityIAckFrameReception( mac_currentPacket.frameType_Subtype );

			case mac_BEACON_FRAME:
			case mac_CONNECTION_REQUEST_FRAME:
			case mac_CONNECTION_ASSIGNMENT_FRAME:
			case mac_DISCONNECTION_FRAME:
			case mac_COMMAND_FRAME:
			case mac_B_ACK_FRAME:
			case mac_I_ACK_POLL_FRAME:
			case mac_B_ACK_POLL_FRAME:
			case mac_POLL_FRAME:
			case mac_T_POLL_FRAME:
			case mac_WAKEUP_FRAME:
			case mac_B2_FRAME:
			case mac_DATA_FRAME_USER_PRIORITY_0:
			case mac_DATA_FRAME_USER_PRIORITY_1:
			case mac_DATA_FRAME_USER_PRIORITY_2:
			case mac_DATA_FRAME_USER_PRIORITY_3:
			case mac_DATA_FRAME_USER_PRIORITY_4:
			case mac_DATA_FRAME_USER_PRIORITY_5:
			case mac_DATA_FRAME_USER_PRIORITY_6:
			case mac_DATA_FRAME_EMERGENCY:
				return MACIAckFrameReception( mac_currentPacket.frameType_Subtype );
			default:
				;
#ifdef DEBUG
				printf( "**ERROR: Can't find this case! @PL_DATA_indication -> mac_I_ACK_FRAME" );
#endif
			}
			break;

		case mac_SECURITY_ASSOCIATION_FRAME:
		case mac_SECURITY_DISASSOCIATION_FRAME:
		case mac_PTK_FRAME:
		case mac_GTK_FRAME:
			return SecurityMngFrameReception( mac_frameControl.bits.FrameType_Subtype );

		default:
			;
#ifdef DEBUG
			printf( "**ERROR: Can't find this case! @PL_DATA_indication" );
#endif
		}
		break;

	default:
		;//error：没有匹配的原语操作
#ifdef DEBUG
		printf( "**ERROR: Can't find this case! @MACTasks(...) " );
#endif
	}
	return NO_PRIMITIVE;
}

/******************************************************************
**函数原型:		WBAN_PRIMITIVE MACFrameTransmission( BYTE frameTypeSubtype, BYTE txType, BTTE* TxBuffer, WORD length )

               ******对于程序中遇到的数据类型，已在Wban.h中定义，如下：******
					#include <stdint.h>
					#define BYTE	uint8_t
					#define WORD	uint16_t
					#define DWORD	uint32_t
					#define BOOL	uint8_t
					#define TRUE	1
					#define FALSE 	0
               **************************************************************

**入口参数:   	BYTE frameTypeSubtype:
					可用参数值列表（在WbanMAC.h中已定义）：
					mac_BEACON_FRAME
					mac_SECURITY_ASSOCIATION_FRAME
					mac_SECURITY_DISASSOCIATION_FRAME
					mac_PTK_FRAME
					mac_GTK_FRAME
					mac_CONNECTION_REQUEST_FRAME
					mac_CONNECTION_ASSIGNMENT_FRAME
					mac_DISCONNECTION_FRAME
					mac_COMMAND_FRAME

					mac_I_ACK_FRAME
					mac_B_ACK_FRAME
					mac_I_ACK_POLL_FRAME
					mac_B_ACK_POLL_FRAME
					mac_POLL_FRAME
					mac_T_POLL_FRAME
					mac_WAKEUP_FRAME
					mac_B2_FRAME

					mac_DATA_FRAME

				BYTE txType:
					可用参数值列表（在WbanMAC.h中已定义）：
					mac_DIRECT
					mac_CSMACA
					mac_PROB
					mac_SCHEDULE

				BTTE* TxBuffer 指明待发送的帧存放的位置，如果是数据类型的帧则置0

				WORD length 指明待发送的帧的有效数据长度，如果是数据类型的帧则置0

**返 回 值：	WBAN_PRIMITIVE
				发送流程处理成功后返回 PL_DATA_request， 回到大循环后交给PHY层接着处理
				其他情况返回 NO_PRIMITIVE,并返回大循环

**说    明：	本函数为MAC层的发送函数，经过装帧后需要在
				1. 数据类型的帧需要区分竞争发送mac_CSMACA和调度发送mac_SCHEDULE
				2. 如果要采用安全认证的连接方式，T-Poll帧后的第一个SECURITY ASSOCIATION帧需要采用以概率发送mac_PROB
				3. 如果不采用安全认证的连接方式，T-Poll帧后的Connect Request帧需要采用以概率发送mac_PROB
				4. 其他管理类型的帧和控制类型的帧均采用直接发送方式mac_DIRECT
				5. 管理类型的帧和控制类型的帧需要先在mac_TxBuffer中装帧完毕，才能调用此发送函数

******************************************************************/
WBAN_PRIMITIVE MACFrameTransmission( BYTE frameTypeSubtype, BYTE txType, BYTE* TxBuffer, WORD length )
{
	BYTE frameType;
	WBAN_PRIMITIVE primitive = NO_PRIMITIVE;
	//输入合法性检查开始
	BOOL txTypeFlag = TRUE;
	//判断数据有效长度是否合法，mac_PACKET_BUFFER_SIZE在WbanTasks.h中定义
	if( length > mac_PACKET_BUFFER_SIZE )
		txTypeFlag = FALSE;

	switch( frameTypeSubtype ){
		case mac_BEACON_FRAME:
		case mac_SECURITY_DISASSOCIATION_FRAME:
		case mac_PTK_FRAME:
		case mac_GTK_FRAME:
		case mac_CONNECTION_ASSIGNMENT_FRAME:
		case mac_DISCONNECTION_FRAME:
		case mac_COMMAND_FRAME:
			frameType = mac_MANAGEMENT_FRAME;
			if( txType != mac_DIRECT )
				txTypeFlag = FALSE;
			break;
		case mac_I_ACK_FRAME:
		case mac_B_ACK_FRAME:
		case mac_I_ACK_POLL_FRAME:
		case mac_B_ACK_POLL_FRAME:
		case mac_POLL_FRAME:
		case mac_T_POLL_FRAME:
		case mac_WAKEUP_FRAME:
		case mac_B2_FRAME:
			frameType = mac_CONTROL_FRAME;
	//利用二进制数的与或特性进行检查
			if( txType != mac_DIRECT )
				txTypeFlag = FALSE;
			break;
		case mac_CONNECTION_REQUEST_FRAME:
		case mac_SECURITY_ASSOCIATION_FRAME:
			frameType = mac_MANAGEMENT_FRAME;
			if( txType != mac_DIRECT && txType !=  mac_PROB )
				txTypeFlag = FALSE;
			break;
		case mac_DATA_FRAME:
			frameType = mac_DATA_FRAME;
			if( txType != mac_CSMACA && txType != mac_SCHEDULE )
				txTypeFlag = FALSE;
			break;
		default:
			txTypeFlag = FALSE;
	}
	if( txTypeFlag == FALSE ){
#ifdef DEBUG
		printf( "**ERROR: sendFrame( ... )输入不合法！" );
#endif
		return NO_PRIMITIVE;
	}
	//输入合法性检查结束

	//根据不同的帧类型和发送方式，进入相应的程序中运行发送程序
	if( frameType == mac_DATA_FRAME){
		if( txType == mac_CSMACA ){
			primitive = DataTypeFrameTxCSMA();
			//数据类型的帧的地址和长度在DataTypeFrameTxCSMA()中直接获得
			//取代原来版本的函数dataTransmitProcess()
			//包含阶段检查和剩余时间检查
			//在TransmitIt()中包含阶段检查和剩余时间检查
		}
		else if( txType == mac_SCHEDULE ){
			primitive = DataTypeFrameTxSchedule();
			//数据类型的帧的地址和长度在DataTypeFrameTxSchedule()中直接获得
			//取代原来版本的函数TransmitItScheduledProcess()
			//包含阶段检查和剩余时间检查
		}
	}
	else if( frameType == mac_CONTROL_FRAME ){
		if( txType == mac_DIRECT ){
			primitive = MngCtrlTypeFrameTxDirect( frameTypeSubtype, TxBuffer, length );
			//取代原来版本的函数frameMngCtrlSendingProcess()
			//缺少部分管理类型的帧和控制类型的帧的处理
		}
	}
	else if( frameType == mac_MANAGEMENT_FRAME ){
		if( txType == mac_DIRECT ){
			primitive = MngCtrlTypeFrameTxDirect( frameTypeSubtype, TxBuffer, length );
			//取代原来版本的函数frameMngCtrlSendingProcess()
			//缺少部分管理类型的帧和控制类型的帧的处理
		}
		else if( txType == mac_PROB ){
			primitive = MngCtrlTypeFrameTxPROB( frameTypeSubtype, TxBuffer, length );
		}
	}

	return primitive;

	//1. 获取当前的系统时刻，算出距离本 超帧Beacon起始时刻的时间间隔
	//判断当前时刻在哪一阶段（RAP? EAP? MAP?）
	//判断当前时刻距离阶段末期剩余时间是否够一帧传输


	//2. 根据不同的发送方式（CSMA方式, 以概率方式，调度方式），制定相应的传输策略，比如CSMA获取BC值，以概率获取CP值


	//3. 配置currentPacket的各个域


	//4. 配置params.PL_DATA_REQUEST的各个域


	//5. 是否需要ACK判断
}


/**********************************************************************
**函数原型:		WBAN_PRIMITIVE DataTypeFrameTxCSMA()
**入口参数:   	void
**返   回 	值：		WBAN_PRIMITIVE
**说      	明：		如果DataBuffer中有数据 或者 有正在准备传的数据，在本函数中进行发送处理
***********************************************************************/
WBAN_PRIMITIVE DataTypeFrameTxCSMA(){
	TICK t;
	t.Val = TickGet().Val - macBIB.BeaconStartTime.Val;
	if((t.Val >= macBIB.RAP1Start.Val) && (t.Val < macBIB.RAP1End.Val)){
		if(TickGetDiff(t, macBIB.RAP1End) < (mac_pCSMASlotLength + mac_FRAME_TRANSACTION_TIME) * mac_CLK_Freq){
#ifdef DEBUG
			printf("距RAP1阶段结束时刻的剩余时间不足够传输一帧 @DataTypeFrameTxCSMA()\n");
#endif
			return NO_PRIMITIVE;
		}
	}else if((t.Val >= macBIB.RAP2Start.Val) && (t.Val < macBIB.RAP2End.Val) ){
		if(TickGetDiff(t, macBIB.RAP2End) < (mac_pCSMASlotLength + mac_FRAME_TRANSACTION_TIME) * mac_CLK_Freq){
#ifdef DEBUG
			printf("距RAP2阶段结束时刻的剩余时间不足够传输一帧 @DataTypeFrameTxCSMA()\n");
#endif
			return NO_PRIMITIVE;
		}
	}else{
#ifdef DEBUG
		printf("不在合适的阶段传输 @DataTypeFrameTxCSMA()\n");
#endif
		return NO_PRIMITIVE;
	}

#ifdef DEBUG
	printf("\n开始传输... @DataTypeFrameTxCSMA() @%ld\n", TickGet().Val);
	printf( "TxDSN为：%d, 分片序号为：%d \n", *(GetDataQueueHead() + 2), (*(GetDataQueueHead() + 3)) & 0x07 );
#endif

	if(mac_TasksPending.bit.dataSending == 0){
		mac_BC = getBackoffCounter();
		mac_TasksPending.bit.dataSending = 1;
#ifdef DEBUG
		printf("获取BC值：%d\n", mac_BC);
#endif
	}

	if(TransmitIt() == TRUE){
		//从数据队列中取出数据，和其他选项，构建PL_DATA_request，
		mac_currentPacket.length = GetDataQueuePacketLength();
		mac_currentPacket.buffer = GetDataQueueHead();
		mac_currentPacket.priority = GetDataQueuePacketPriority();
		mac_currentPacket.frameType_Subtype = (mac_DATA_FRAME + mac_currentPacket.priority);
		mac_currentPacket.startTime = TickGet();
		mac_currentPacket.informationDataRate = GetDataQueuePacketInformationDataRate();
		macBIB.informationDataRate = GetDataQueuePacketInformationDataRate(); 	//为了下一次ack的发送采用上一个数据的发送速率
#ifdef DEBUG
		printf( "\n更新currentPacket... @DataTypeFrameTxCSMA()\n" );
		printf( "length: %d\n", mac_currentPacket.length );
		printf( "buffer: 0x%x\n", mac_currentPacket.buffer );
		printf( "priority: 0x%x\n", mac_currentPacket.priority );
		printf( "startTime: %ld\n", mac_currentPacket.startTime.Val );
		printf( "informationDataRate: 0x%x\n", mac_currentPacket.informationDataRate );
		printf( "retry: %d\n", mac_currentPacket.retry );
#endif

		params.PL_DATA_request.psduLength = mac_currentPacket.length;
		params.PL_DATA_request.psdu = mac_currentPacket.buffer;
		params.PL_DATA_request.informationDataRate = mac_currentPacket.informationDataRate;
		params.PL_DATA_request.band = macBIB.band;
		params.PL_DATA_request.burstMode = BURST_MODE_DEFAULT;
		params.PL_DATA_request.scramblerSeed = macBIB.scramblerSeed;
		params.PL_DATA_request.channelNumber = macBIB.channelNumber;

		mac_TasksPending.bit.dataSending = 0;
#ifdef DEBUG
		printf("\n配置PL_DATA_request原语... @DataTypeFrameTxCSMA()\n");
#endif

		if(GetDataQueuePacketAckPolicy() == mac_N_ACK){
			DeDataQueue();
			mac_currentPacket.lastPacketStatus = mac_N_ACK;
			mac_currentPacket.retry = 0;

			mac_currentPacket.ackPolicy = mac_N_ACK;
			mac_currentPacket.RecipientID.v = *(mac_currentPacket.buffer + 4);
			mac_currentPacket.non_finalFragment = ((*(mac_currentPacket.buffer + 3)) & 0x08) >> 3;
#ifdef DEBUG
			printf("由于数据不需要ack，因此将该数据从数据队列前端删除\n");
#endif
		}else{
			mac_currentPacket.ackPolicy = mac_I_ACK;
			mac_TasksPending.bit.packetWaitingAck = 1;
#ifdef DEBUG
			printf("等待ACK事件触发...\n");
#endif
		}

		return PL_DATA_request;
	}else{
		//返回大循环
	}

	return NO_PRIMITIVE;
}

/**********************************************************************
**函数原型:		WBAN_PRIMITIVE MngCtrlTypeFrameTxPROB( BYTE frameTypeSubType, BYTE* TxBuffer, WORD length )
**入口参数:   	BYTE frameType
  				BYTE frameSubType
 				BYTE* TxBuffer
  				WORD length
**返   回 	值：		WBAN_PRIMITIVE
**说      	明：		以概率发送
***********************************************************************/
WBAN_PRIMITIVE MngCtrlTypeFrameTxPROB( BYTE frameTypeSubType, BYTE* TxBuffer, WORD length )
{
	double z;
	//获取CP
	double cp_temp = 0.5 / ((BYTE)((mac_currentPacket.retry + 1) / 2 + 1));
	if( cp_temp > 0.125 )
		mac_CP = cp_temp;
	else
		mac_CP = 0.125;

#ifdef DEBUG
	printf( "获取CP值：%f\n", mac_CP );
#endif

	//产生随机数z，判断是否发送
	z = (ReadCpuTimer0() & 0xFF) / 255.0;
#ifdef DEBUG
	printf("获取z值：%f\n", z);
#endif

	if( z <= mac_CP ){
		mac_currentPacket.buffer = TxBuffer;
		mac_currentPacket.startTime = TickGet();
		mac_currentPacket.informationDataRate = macBIB.informationDataRate;
		mac_currentPacket.length = length;
		mac_currentPacket.frameType_Subtype = frameTypeSubType;
	#ifdef DEBUG
		printf("更新currentPacket... @MngCtrlTypeFrameTxPROB @%ld\n",TickGet().Val);
		printf("length: %d\n", mac_currentPacket.length);
		printf("buffer: %x\n", mac_currentPacket.buffer);
		printf("startTime: %ld\n", mac_currentPacket.startTime.Val);
		printf("frameSubtype: %ld\n", mac_currentPacket.frameType_Subtype);
		printf("informationDataRate: %d\n", mac_currentPacket.informationDataRate);
	#endif

		params.PL_DATA_request.psduLength = mac_currentPacket.length;
		params.PL_DATA_request.psdu = mac_currentPacket.buffer;
		params.PL_DATA_request.informationDataRate = mac_currentPacket.informationDataRate;
		params.PL_DATA_request.band = macBIB.band;
		params.PL_DATA_request.burstMode = BURST_MODE_DEFAULT;
		params.PL_DATA_request.scramblerSeed = macBIB.scramblerSeed;
		params.PL_DATA_request.channelNumber = macBIB.channelNumber;

#ifdef DEBUG
		printf("配置PL_DATA_request原语...\n");
#endif

		mac_TasksPending.bit.packetWaitingAck = 1;
#ifdef DEBUG
		printf("等待ACK事件触发...\n");
#endif
		return PL_DATA_request;
	}
	return NO_PRIMITIVE;
}

/**********************************************************************
**函数原型:		WBAN_PRIMITIVE dataTransmitAlohaProcess()
**入口参数:   	void
**返   回 	值：		WBAN_PRIMITIVE
**说      	明：		Aloha方式的发送，在本函数中进行发送处理
***********************************************************************/
/*WBAN_PRIMITIVE dataTransmitAlohaProcess(){
	double z;
	TICK t;

#ifdef DEBUG
#ifdef I_AM_NODE
	printf("\n开始传输...TxDSN为：%d, 分片序号为：%d\n", *(GetDataQueueHead() + 2), (*(GetDataQueueHead() + 3)) & 0x07);
#endif
#ifdef I_AM_HUB
	printf("\n开始传输...TxDSN为：%d, 分片序号为：%d\n", *(GetDataQueueHead() + 2), (*(GetDataQueueHead() + 3) & 0x07));
#endif
#endif

	//获取CP
	mac_CP = getCP( mac_DATA_FRAME );
#ifdef DEBUG
	printf( "获取CP值：%f\n", mac_CP );
#endif

	//判断是否处在合适的阶段
	t.Val = TickGet().Val - macBIB.BeaconStartTime.Val;
	if( (t.Val >= macBIB.RAP1Start.Val) && (t.Val < macBIB.RAP1End.Val) ){
		if( TickGetDiff( t, macBIB.RAP1End ) < (mac_pSIFS + mac_pExtraIFS + mac_FRAME_TRANSACTION_TIME) * mac_CLK_Freq ){
#ifdef DEBUG
			printf("距RAP1阶段结束时刻的剩余时间不足够传输一帧\n");
#endif
			return NO_PRIMITIVE;
		}
	}else if( (t.Val >= macBIB.RAP2Start.Val) && (t.Val < macBIB.RAP2End.Val) ){
		if( TickGetDiff( t, macBIB.RAP2End ) < (mac_pSIFS + mac_pExtraIFS + mac_FRAME_TRANSACTION_TIME) * mac_CLK_Freq ){
#ifdef DEBUG
			printf("距RAP2阶段结束时刻的剩余时间不足够传输一帧\n");
#endif
			return NO_PRIMITIVE;
		}
	}else{
#ifdef DEBUG
		printf("不在合适的阶段传输\n");
#endif
		return NO_PRIMITIVE;
	}

	//产生随机数z，判断是否发送
	z = (ReadCpuTimer0() & 0xFF) / 255.0;
#ifdef DEBUG
	printf("获取z值：%f\n", z);
#endif

	if( z <= mac_CP ){
		mac_currentPacket.length = GetDataQueuePacketLength();
		mac_currentPacket.buffer = GetDataQueueHead();
		mac_currentPacket.priority = GetDataQueuePacketPriority();
		mac_currentPacket.startTime = TickGet();
		mac_currentPacket.informationDataRate = GetDataQueuePacketInformationDataRate();
		macBIB.informationDataRate = GetDataQueuePacketInformationDataRate(); 	//为了下一次ack的发送采用上一个数据的发送速率
#ifdef DEBUG
		printf("更新currentPacket...\n");
		printf("length: %d\n", mac_currentPacket.length);
		printf("buffer: %x\n", mac_currentPacket.buffer);
		printf("priority: %d\n", mac_currentPacket.priority);
		printf("startTime: %ld\n", mac_currentPacket.startTime.Val);
		printf("informationDataRate: %d\n", mac_currentPacket.informationDataRate);
#endif

		params.PL_DATA_request.psduLength = mac_currentPacket.length;
		params.PL_DATA_request.psdu = mac_currentPacket.buffer;
		params.PL_DATA_request.informationDataRate = mac_currentPacket.informationDataRate;
#ifdef DEBUG
		printf("配置PL_DATA_request原语...\n");
#endif

	if(GetDataQueuePacketAckPolicy() == mac_N_ACK){
		DeDataQueue();
		mac_currentPacket.lastPacketStatus = mac_N_ACK;
		mac_currentPacket.retry = 0;
#ifdef DEBUG
		printf("由于数据不需要ack，因此将该数据从数据队列前端删除\n");
#endif
	}else{
		mac_TasksPending.bit.packetWaitingAck = 1;
#ifdef DEBUG
		printf("等待ACK事件触发...\n");
#endif
	}

		return PL_DATA_request;
	}
	return NO_PRIMITIVE;
}
*/

//#ifdef I_AM_NODE
/**********************************************************************
**函数原型:		WBAN_PRIMITIVE mngTransmitProcess()
**入口参数:   	void
**返   回 	值：		WBAN_PRIMITIVE
**说      	明：		使用CSMA/CA模式发送ConnectionRequest帧
***********************************************************************/
/*WBAN_PRIMITIVE mngTransmitProcess(){
	params.MACS_DATA_request.frameType = mac_MANAGEMENT_FRAME;
	params.MACS_DATA_request.frameSubtype = mac_USER_PRIORITY_5;
	mac_BC = getBackoffCounter();
#ifdef DEBUG
		printf("获取BC值：%d\n", mac_BC);
#endif

	if(mngTransmitIt() == TRUE){
		//从数据队列中取出数据，和其他选项，构建PL_DATA_request，
		mac_currentPacket.frameSubtype = mac_CONNECTION_REQUEST_FRAME;
		mac_currentPacket.buffer = mac_TxBuffer;
		mac_currentPacket.priority = mac_USER_PRIORITY_5;
		mac_currentPacket.startTime = TickGet();
		mac_currentPacket.informationDataRate = macBIB.informationDataRate;
		//mac_currentPacket.informationDataRate = params.NME_MLME_CONNECT_request.informationDataRate;
		//macBIB.informationDataRate = params.NME_MLME_CONNECT_request.informationDataRate; 	//为了下一次ack的发送采用上一个数据的发送速率
#ifdef DEBUG
		printf("更新currentPacket...\n");
		printf("length: %d\n", mac_currentPacket.length);
		printf("buffer: %x\n", mac_currentPacket.buffer);
		printf("priority: %d\n", mac_currentPacket.priority);
		printf("startTime: %ld\n", mac_currentPacket.startTime.Val);
		printf("informationDataRate: %d\n", mac_currentPacket.informationDataRate);
#endif

		params.PL_DATA_request.psduLength = mac_currentPacket.length;
		params.PL_DATA_request.psdu = mac_currentPacket.buffer;
		params.PL_DATA_request.informationDataRate = mac_currentPacket.informationDataRate;
#ifdef DEBUG
		printf("配置PL_DATA_request原语...\n");
#endif

		mac_TasksPending.bit.packetWaitingAck = 1;
#ifdef DEBUG
			printf("等待ACK事件触发...\n");
#endif
		return PL_DATA_request;
	}else{
#ifdef DEBUG
		printf("error    mngTransmitProcess()\n");
#endif
	}

	return NO_PRIMITIVE;
}
#endif
*/

/**********************************************************************
**函数原型:		WBAN_PRIMITIVE MngCtrlTypeFrameTxDirect( BYTE frameTypeSubtype, BYTE* TxBuffer, WORD length )
**入口参数:		BYTE frameType
				BYTE frameSubtype
  				BYTE* TxBuffer
  				WORD length
**返   回 	值：		WBAN_PRIMITIVE
**说      	明：		控制帧和管理帧的发送
***********************************************************************/
WBAN_PRIMITIVE MngCtrlTypeFrameTxDirect( BYTE frameTypeSubtype, BYTE* TxBuffer, WORD length ){
	mac_currentPacket.buffer = TxBuffer;
	mac_currentPacket.startTime = TickGet();
	mac_currentPacket.informationDataRate = macBIB.informationDataRate;
	mac_currentPacket.length = length;
	mac_currentPacket.frameType_Subtype = frameTypeSubtype;
#ifdef DEBUG
	printf("\n更新currentPacket... @MngCtrlTypeFrameTxDirect(...) @%ld\n",TickGet().Val);
	printf("length: %d\n", mac_currentPacket.length);
	printf("buffer: 0x%x\n", mac_currentPacket.buffer);
	printf("startTime: %ld\n", mac_currentPacket.startTime.Val);
	printf("frameTypeSubtype: 0x%x\n", mac_currentPacket.frameType_Subtype );
	printf("informationDataRate: 0x%x\n", mac_currentPacket.informationDataRate);
#endif

	params.PL_DATA_request.psduLength = mac_currentPacket.length;
	params.PL_DATA_request.psdu = mac_currentPacket.buffer;
	params.PL_DATA_request.informationDataRate = mac_currentPacket.informationDataRate;
	params.PL_DATA_request.band = macBIB.band;
	params.PL_DATA_request.burstMode = BURST_MODE_DEFAULT;
	params.PL_DATA_request.scramblerSeed = macBIB.scramblerSeed;
	params.PL_DATA_request.channelNumber = macBIB.channelNumber;

	switch(mac_currentPacket.frameType_Subtype & 0xF0){
	case mac_MANAGEMENT_FRAME:
		switch(mac_currentPacket.frameType_Subtype){
		case mac_BEACON_FRAME:
			macBIB.BeaconStartTime = TickGet();
			break;
		case mac_CONNECTION_REQUEST_FRAME:
		case mac_CONNECTION_ASSIGNMENT_FRAME:
		case mac_DISCONNECTION_FRAME:
		case mac_SECURITY_ASSOCIATION_FRAME:
		case mac_PTK_FRAME:
		case mac_SECURITY_DISASSOCIATION_FRAME:
		//如果安全部分的帧都需要ack，需要在这里配置
			mac_TasksPending.bit.packetWaitingAck = 1;
			break;
		}
		break;

	case mac_CONTROL_FRAME:
		switch(mac_currentPacket.frameType_Subtype){
		case mac_T_POLL_FRAME:
		case mac_I_ACK_FRAME:
			break;
		}
		break;
	default:
		;
#ifdef DEBUG
		printf("MngCtrlTypeFrameTxDirect(...)->没有这个帧类型\n");
#endif
	}
	return PL_DATA_request;
}

/**********************************************************************
**函数原型:		WBAN_PRIMITIVE DataTypeFrameTxSchedule()
**入口参数:   	void
**返   回 	值：		WBAN_PRIMITIVE
**说      	明：		调度发送
***********************************************************************/
WBAN_PRIMITIVE DataTypeFrameTxSchedule(){
	TICK t;
	t.Val = TickGet().Val - macBIB.BeaconStartTime.Val;
	if( t.Val > macBIB.IntervalEnd.Val ){
#ifdef DEBUG
		printf( "当前时刻不在MAP期 @DataTypeFrameTxSchedule()" );
#endif
		return NO_PRIMITIVE;
	}
	if(TickGetDiff(t, macBIB.IntervalEnd) <= (mac_FRAME_TRANSACTION_TIME * mac_CLK_Freq)){
#ifdef DEBUG
		printf( "距时隙结束时刻的剩余时间不足够传输一帧 @DataTypeFrameTxSchedule()" );
#endif
		return NO_PRIMITIVE;
	}

#ifdef DEBUG
	printf("\n开始传输...TxDSN为：%d, 分片序号为：%d\n", *(GetDataQueueHead() + 2), (*(GetDataQueueHead() + 3)) & 0x07);
	printf(" @DataTypeFrameTxSchedule @%ld\n",TickGet().Val);
#endif
	//从数据队列中取出数据，和其他选项，构建PL_DATA_request，
	mac_currentPacket.length = GetDataQueuePacketLength();
	mac_currentPacket.buffer = GetDataQueueHead();
	mac_currentPacket.priority = GetDataQueuePacketPriority();
	mac_currentPacket.frameType_Subtype = (mac_DATA_FRAME + mac_currentPacket.priority);
	mac_currentPacket.startTime = TickGet();
	mac_currentPacket.informationDataRate = GetDataQueuePacketInformationDataRate();
	macBIB.informationDataRate = GetDataQueuePacketInformationDataRate(); 	//为了下一次ack的发送采用上一个数据的发送速率

#ifdef DEBUG
	printf("更新mac_currentPacket...\n");
	printf("length: %d\n", mac_currentPacket.length);
	printf("buffer: %x\n", mac_currentPacket.buffer);
	printf("priority: %d\n", mac_currentPacket.priority);
	printf("startTime: %ld\n", mac_currentPacket.startTime.Val);
	printf("informationDataRate: %d\n", mac_currentPacket.informationDataRate);
	printf( "retry: %d\n", mac_currentPacket.retry );
#endif

	params.PL_DATA_request.psduLength = mac_currentPacket.length;
	params.PL_DATA_request.psdu = mac_currentPacket.buffer;
	params.PL_DATA_request.informationDataRate = mac_currentPacket.informationDataRate;
	params.PL_DATA_request.band = macBIB.band;
	params.PL_DATA_request.burstMode = BURST_MODE_DEFAULT;
	params.PL_DATA_request.scramblerSeed = macBIB.scramblerSeed;
	params.PL_DATA_request.channelNumber = macBIB.channelNumber;

#ifdef DEBUG
	printf("配置PL_DATA_request原语...\n");
#endif

	if(GetDataQueuePacketAckPolicy() == mac_N_ACK){
		DeDataQueue();
		mac_currentPacket.lastPacketStatus = mac_N_ACK;
		mac_currentPacket.retry = 0;

		mac_currentPacket.ackPolicy = mac_N_ACK;
		mac_currentPacket.RecipientID.v = *(mac_currentPacket.buffer + 4);
		mac_currentPacket.non_finalFragment = ((*(mac_currentPacket.buffer + 3)) & 0x08) >> 3;
#ifdef DEBUG
		printf("由于数据不需要ack，因此将该数据从数据队列前端删除\n");
#endif
	}else{
		mac_currentPacket.ackPolicy = mac_I_ACK;
		mac_TasksPending.bit.packetWaitingAck = 1;
#ifdef DEBUG
		printf("等待ACK事件触发...\n");
#endif
	}

	return PL_DATA_request;
}

/**********************************************************************
**函数原型:		WBAN_PRIMITIVE MACMngCtrlFrameReception( BYTE frameSubtype )
**入口参数:   	BYTE frameSubtype
**返 回 值：	WBAN_PRIMITIVE
**说    明：	处理MAC部分接收到的管理类型的帧和控制类型的帧
***********************************************************************/
WBAN_PRIMITIVE MACMngCtrlFrameReception( BYTE frameSubtype )
{
#ifdef I_AM_NODE
	if( frameSubtype == mac_BEACON_FRAME ){
#ifdef DEBUG
		printf("收到的是Beacon帧\n");
		printf("开始配置params...\n");
#endif
		params.MLME_BEACON_NOTIFY_indication.SenderID.v = params.MACS_DATA_indication.SenderID.v;
		params.MLME_BEACON_NOTIFY_indication.BANID.v = params.MACS_DATA_indication.BANID.v;
		params.MLME_BEACON_NOTIFY_indication.BSN = mac_frameControl.bits.SequenceNumber_PollPostWindow;
		params.MLME_BEACON_NOTIFY_indication.hubAddress.v[0] = MACGet();
		params.MLME_BEACON_NOTIFY_indication.hubAddress.v[1] = MACGet();
		params.MLME_BEACON_NOTIFY_indication.hubAddress.v[2] = MACGet();
		params.MLME_BEACON_NOTIFY_indication.hubAddress.v[3] = MACGet();
		params.MLME_BEACON_NOTIFY_indication.hubAddress.v[4] = MACGet();
		params.MLME_BEACON_NOTIFY_indication.hubAddress.v[5] = MACGet();
		params.MLME_BEACON_NOTIFY_indication.beaconPeriodLength = MACGet();
		params.MLME_BEACON_NOTIFY_indication.allocationSlotLength = MACGet();
		params.MLME_BEACON_NOTIFY_indication.RAP1End = MACGet();
		params.MLME_BEACON_NOTIFY_indication.RAP2Start = MACGet();
		params.MLME_BEACON_NOTIFY_indication.RAP2End = MACGet();
		params.MLME_BEACON_NOTIFY_indication.MACCapability[0] = MACGet();
		params.MLME_BEACON_NOTIFY_indication.MACCapability[1] = MACGet();
		params.MLME_BEACON_NOTIFY_indication.MACCapability[2] = MACGet();
		params.MLME_BEACON_NOTIFY_indication.PHYCapability = MACGet();

#ifdef DEBUG
		printf("开始更新macBIB...\n");
#endif
		macBIB.BANID.v = params.MLME_BEACON_NOTIFY_indication.BANID.v;
		macBIB.BeaconStartTime = TickGet();
		uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
		macBIB.RAP1End.Val = params.MLME_BEACON_NOTIFY_indication.RAP1End * temp;
		macBIB.RAP1Start.Val = params.MLME_BEACON_NOTIFY_indication.RAP1Start * temp;
		macBIB.RAP2End.Val = params.MLME_BEACON_NOTIFY_indication.RAP2End * temp;
		macBIB.RAP2Start.Val = params.MLME_BEACON_NOTIFY_indication.RAP2Start * temp;
		macBIB.allocationSlotLength = params.MLME_BEACON_NOTIFY_indication.allocationSlotLength;
		macBIB.beaconPeriodLength = params.MLME_BEACON_NOTIFY_indication.beaconPeriodLength;
		macBIB.hubCapability.bytes.b0 = params.MLME_BEACON_NOTIFY_indication.MACCapability[0];
		macBIB.hubCapability.bytes.b1 = params.MLME_BEACON_NOTIFY_indication.MACCapability[1];
		macBIB.hubCapability.bytes.b2 = params.MLME_BEACON_NOTIFY_indication.MACCapability[2];
		macBIB.hubCapability.bytes.b3 = params.MLME_BEACON_NOTIFY_indication.PHYCapability;
		macBIB.macBSN = params.MLME_BEACON_NOTIFY_indication.BSN;

		//如果在正常通信过程中，有新节点的加入，它以受到Beacon帧代替收到T-poll帧
		if(macBIB.abbrAddr.v == mac_UNCONNECTED_BROADCAST_NID){
			macBIB.SecurityLevel = mac_frameControl.bits.SecurityLevel;
			macBIB.AccessMode = mac_frameControl.bits.LastFrame_AccessMode_B2;
			macBIB.MoreData = mac_frameControl.bits.MoreData;
			macBIB.PollPostWindow = mac_frameControl.bits.SequenceNumber_PollPostWindow;
			macBIB.Next = mac_frameControl.bits.FragmentNumber_Next_Coexistence;
			macBIB.hubAddr.v = params.MLME_BEACON_NOTIFY_indication.SenderID.v;
			macBIB.abbrAddr.v = mac_UNCONNECTED_NID;

			return NME_MLME_CONNECT_indication;
		}

		return MLME_BEACON_NOTIFY_indication;
	}
	else if( frameSubtype == mac_CONNECTION_ASSIGNMENT_FRAME ){
#ifdef DEBUG
		printf("收到的是CONNECTION ASSIGNMENT帧\n");
#endif
		macBIB.abbrAddr.v = params.MACS_DATA_indication.RecipientID.v;
		macBIB.hubAddr.v = params.MACS_DATA_indication.SenderID.v;
		macBIB.BANID.v = params.MACS_DATA_indication.BANID.v;
		params.NME_MLME_CONNECT_confirm.AssocAbbrID.v = macBIB.abbrAddr.v;

		params.NME_MLME_CONNECT_confirm.fullAddr.v[0] = MACGet();
		params.NME_MLME_CONNECT_confirm.fullAddr.v[1] = MACGet();
		params.NME_MLME_CONNECT_confirm.fullAddr.v[2] = MACGet();
		params.NME_MLME_CONNECT_confirm.fullAddr.v[3] = MACGet();
		params.NME_MLME_CONNECT_confirm.fullAddr.v[4] = MACGet();
		params.NME_MLME_CONNECT_confirm.fullAddr.v[5] = MACGet();
		for(mac_i = 0; mac_i < 6; mac_i++){
			if(params.NME_MLME_CONNECT_confirm.fullAddr.v[mac_i] != macBIB.fullAddr.v[mac_i]){
				params.NME_MLME_CONNECT_confirm.status = mac_FAIL;
				params.NME_MLME_CONNECT_confirm.AssocAbbrID.v = mac_UNCONNECTED_BROADCAST_NID;
				return NME_MLME_CONNECT_confirm;
			}
		}

		macBIB.hubFullAddr.v[0] = MACGet();
		macBIB.hubFullAddr.v[1] = MACGet();
		macBIB.hubFullAddr.v[2] = MACGet();
		macBIB.hubFullAddr.v[3] = MACGet();
		macBIB.hubFullAddr.v[4] = MACGet();
		macBIB.hubFullAddr.v[5] = MACGet();

		params.NME_MLME_CONNECT_confirm.status = MACGet();
		params.NME_MLME_CONNECT_confirm.ConnectionStatus = ((params.NME_MLME_CONNECT_confirm.status >> 2) & 0x1F);
		params.NME_MLME_CONNECT_confirm.AccessIndicator = (params.NME_MLME_CONNECT_confirm.status & 0x03);
		params.NME_MLME_CONNECT_confirm.CurrentSuperframeNumber = MACGet();

		macBIB.EarliestRAP1End = MACGet();

		uint64_t temp;
		temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);

		macBIB.EAP2Start.Val = MACGet() * temp;
		params.NME_MLME_CONNECT_confirm.MinmimCAPLength = MACGet();
		macBIB.hubCapability.bytes.b0 = MACGet();
		macBIB.hubCapability.bytes.b1 = MACGet();
		macBIB.hubCapability.bytes.b2 = MACGet();
		macBIB.hubCapability.bytes.b3 = MACGet();

		params.NME_MLME_CONNECT_confirm.ConnectionChangeIndicator = MACGet();
		params.NME_MLME_CONNECT_confirm.AssignedAckDataRates = MACGet();
		params.NME_MLME_CONNECT_confirm.AssignedWakeupPhase[0] = MACGet();
		params.NME_MLME_CONNECT_confirm.AssignedWakeupPhase[1] = MACGet();
		params.NME_MLME_CONNECT_confirm.AssignedWakeupPeriod[0] = MACGet();
		params.NME_MLME_CONNECT_confirm.AssignedWakeupPeriod[1] = MACGet();

		if(params.NME_MLME_CONNECT_confirm.ConnectionStatus == mac_SUCCESS){
			params.NME_MLME_CONNECT_confirm.ElementID = MACGet();
			params.NME_MLME_CONNECT_confirm.Length = MACGet();
			params.NME_MLME_CONNECT_confirm.AllocationID = MACGet();
			params.NME_MLME_CONNECT_confirm.IntervalStart = MACGet();
			params.NME_MLME_CONNECT_confirm.IntervalEnd = MACGet();

			macBIB.IntervalStart.Val = temp * params.NME_MLME_CONNECT_confirm.IntervalStart;
			macBIB.IntervalEnd.Val = temp * params.NME_MLME_CONNECT_confirm.IntervalEnd;
		}
		else{
#ifdef DEBUG
			printf("CONNECTION ASSIGNMENT帧指示连接不成功\n");
#endif
		}

		if(mac_frameControl.bits.AckPolicy == mac_I_ACK){
			mac_packetHeader = mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = 0x10;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = 0x00;

			mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

			//帧尾包装，校验和，暂缺
			mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;
#ifdef DEBUG
			printf("收到的是CONNECTION_ASSIGNMENT_FRAME，且需要回复ACK，ACK已成帧\n");
#endif
			mac_TasksPending.bit.ackSending = 1;
		}
		return NME_MLME_CONNECT_confirm;
	}
	else if( frameSubtype == mac_T_POLL_FRAME ){
#ifdef DEBUG
		printf("收到的是T_POLL帧\n");
#endif
		if(macBIB.abbrAddr.v != mac_UNCONNECTED_BROADCAST_NID)
			return NO_PRIMITIVE;

/*		params.NME_MLME_CONNECT_indication.AccessMode = mac_frameControl.bits.LastFrame_AccessMode_B2;
		params.NME_MLME_CONNECT_indication.MoreData = mac_frameControl.bits.MoreData;
		params.NME_MLME_CONNECT_indication.PollPostWindow = mac_frameControl.bits.SequenceNumber_PollPostWindow;
		params.NME_MLME_CONNECT_indication.Next = mac_frameControl.bits.FragmentNumber_Next_Coexistence;
		params.NME_MLME_CONNECT_indication.SenderID.v = params.MACS_DATA_indication.SenderID.v;
		params.NME_MLME_CONNECT_indication.BANID.v = params.MACS_DATA_indication.BANID.v;
		macBIB.PollPostWindow = params.NME_MLME_CONNECT_indication.PollPostWindow;
		macBIB.Next = params.NME_MLME_CONNECT_indication.Next;
		macBIB.hubAddr.v = params.NME_MLME_CONNECT_indication.SenderID.v;
		macBIB.BANID.v = params.NME_MLME_CONNECT_indication.BANID.v;
		return NME_MLME_CONNECT_indication;
*/
		params.NME_MLME_CONNECT_indication.SlotNumber = MACGet();
		params.NME_MLME_CONNECT_indication.SlotOffset[0] = MACGet();
		params.NME_MLME_CONNECT_indication.SlotOffset[1] = MACGet();
		params.NME_MLME_CONNECT_indication.SlotLength = MACGet();
		params.NME_MLME_CONNECT_indication.SecurityLevel = mac_frameControl.bits.SecurityLevel;
		macBIB.allocationSlotLength = params.NME_MLME_CONNECT_indication.SlotLength;
		macBIB.BeaconStartTime = TickGet();
		BYTE m = getCurrentSlotNumber();

		if(m > mac_frameControl.bits.SequenceNumber_PollPostWindow){
#ifdef DEBUG
			printf("CurrentSlotNumber :%d\n",m);
		    printf("PollPostWindow :%d\n",mac_frameControl.bits.SequenceNumber_PollPostWindow);
		    printf("超出接入时间\n");
#endif
			return NO_PRIMITIVE;
		}

		macBIB.SecurityLevel = mac_frameControl.bits.SecurityLevel;
		macBIB.AccessMode = mac_frameControl.bits.LastFrame_AccessMode_B2;
		macBIB.MoreData = mac_frameControl.bits.MoreData;
		macBIB.PollPostWindow = mac_frameControl.bits.SequenceNumber_PollPostWindow;
		macBIB.Next = mac_frameControl.bits.FragmentNumber_Next_Coexistence;
		params.NME_MLME_CONNECT_indication.SenderID.v = params.MACS_DATA_indication.SenderID.v;
		params.NME_MLME_CONNECT_indication.BANID.v = params.MACS_DATA_indication.BANID.v;
		macBIB.hubAddr.v = params.NME_MLME_CONNECT_indication.SenderID.v;
		macBIB.BANID.v = params.NME_MLME_CONNECT_indication.BANID.v;
		macBIB.abbrAddr.v = mac_UNCONNECTED_NID;

		return NME_MLME_CONNECT_indication;
	}
#endif

#ifdef I_AM_HUB
	if( frameSubtype == mac_CONNECTION_REQUEST_FRAME ){
#ifdef DEBUG
		printf("收到的是CONNECTION REQUEST帧\n");
#endif

		params.HME_MLME_CONNECT_indication.RecipientID.v = params.MACS_DATA_indication.RecipientID.v;
		params.HME_MLME_CONNECT_indication.SenderID.v = params.MACS_DATA_indication.SenderID.v;
		params.HME_MLME_CONNECT_indication.BANID.v = params.MACS_DATA_indication.BANID.v;
		params.HME_MLME_CONNECT_indication.HubAddress.v[0] = MACGet();
		params.HME_MLME_CONNECT_indication.HubAddress.v[1] = MACGet();
		params.HME_MLME_CONNECT_indication.HubAddress.v[2] = MACGet();
		params.HME_MLME_CONNECT_indication.HubAddress.v[3] = MACGet();
		params.HME_MLME_CONNECT_indication.HubAddress.v[4] = MACGet();
		params.HME_MLME_CONNECT_indication.HubAddress.v[5] = MACGet();
		params.HME_MLME_CONNECT_indication.NodeAddress.v[0] = MACGet();
		params.HME_MLME_CONNECT_indication.NodeAddress.v[1] = MACGet();
		params.HME_MLME_CONNECT_indication.NodeAddress.v[2] = MACGet();
		params.HME_MLME_CONNECT_indication.NodeAddress.v[3] = MACGet();
		params.HME_MLME_CONNECT_indication.NodeAddress.v[4] = MACGet();
		params.HME_MLME_CONNECT_indication.NodeAddress.v[5] = MACGet();
		CAPABILITY tem;
		tem.bytes.b0 = MACGet();
		tem.bytes.b1 = MACGet();
		tem.bytes.b2 = MACGet();
		tem.bytes.b3 = MACGet();
		params.HME_MLME_CONNECT_indication.ConnectionChangeIndicator = MACGet();
		params.HME_MLME_CONNECT_indication.RequestedAckDataRates = MACGet();
		params.HME_MLME_CONNECT_indication.RequestedWakeupPeriod[0] = MACGet();
		params.HME_MLME_CONNECT_indication.RequestedWakeupPeriod[1] = MACGet();
		params.HME_MLME_CONNECT_indication.RequestedWakeupPhase[0] = MACGet();
		params.HME_MLME_CONNECT_indication.RequestedWakeupPhase[1] = MACGet();

		params.HME_MLME_CONNECT_indication.ElementID = MACGet();
		params.HME_MLME_CONNECT_indication.Length = MACGet();
		params.HME_MLME_CONNECT_indication.AllocationID = MACGet();
		params.HME_MLME_CONNECT_indication.MaximumGap = MACGet();
		params.HME_MLME_CONNECT_indication.MinimumGap = MACGet();
		params.HME_MLME_CONNECT_indication.MinimumLength = MACGet();
		params.HME_MLME_CONNECT_indication.AllocationLength = MACGet();
		params.HME_MLME_CONNECT_indication.ScalingFactor = MACGet();

#ifdef DEBUG
		printf("更新macBIB...\n");
#endif
      	macBIB.uplinkRequestIE.AllocationLength = params.HME_MLME_CONNECT_indication.AllocationLength;
		macBIB.BANID.v = params.HME_MLME_CONNECT_indication.BANID.v;
		params.HME_MLME_CONNECT_indication.AssocAbbrID = GetAbbrAddr();
		EnNodeToNodeList(params.HME_MLME_CONNECT_indication.AssocAbbrID,
				         params.HME_MLME_CONNECT_indication.NodeAddress,
				         tem);
		/*BYTE mac_a;
		mac_a = getIndexFromNodeList( params.HME_MLME_CONNECT_indication.AssocAbbrID );
		macBIB.nodeList[mac_a].fullAddr.v[0] = params.HME_MLME_CONNECT_indication.NodeAddress.v[0];
		macBIB.nodeList[mac_a].fullAddr.v[1] = params.HME_MLME_CONNECT_indication.NodeAddress.v[1];
		macBIB.nodeList[mac_a].fullAddr.v[2] = params.HME_MLME_CONNECT_indication.NodeAddress.v[2];
		macBIB.nodeList[mac_a].fullAddr.v[3] = params.HME_MLME_CONNECT_indication.NodeAddress.v[3];
		macBIB.nodeList[mac_a].fullAddr.v[4] = params.HME_MLME_CONNECT_indication.NodeAddress.v[4];
		macBIB.nodeList[mac_a].fullAddr.v[5] = params.HME_MLME_CONNECT_indication.NodeAddress.v[5];
		macBIB.nodeList[mac_a].capability.bytes.b0 = tem.bytes.b0;
		macBIB.nodeList[mac_a].capability.bytes.b1 = tem.bytes.b1;
		macBIB.nodeList[mac_a].capability.bytes.b2 = tem.bytes.b2;
		macBIB.nodeList[mac_a].capability.bytes.b3 = tem.bytes.b3;*/

		if(mac_frameControl.bits.AckPolicy == mac_I_ACK){
			mac_packetHeader = mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = 0x10;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = 0x00;

			mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

			//帧尾包装，校验和，暂缺
			mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;
#ifdef DEBUG
			printf("收到的是数据帧，且需要回复ACK，ACK已成帧\n");
#endif
			mac_TasksPending.bit.ackSending = 1;
		}
		return HME_MLME_CONNECT_indication;
	}
#endif

	else if( frameSubtype == mac_DISCONNECTION_FRAME ){
#ifdef DEBUG
		printf("收到的是DISCONNECTION_FRAME帧\n");
#endif
		params.MLME_DISCONNECT_indication.AssocAbbrID.v = params.MACS_DATA_indication.RecipientID.v;
		params.MLME_DISCONNECT_indication.SenderID.v = params.MACS_DATA_indication.SenderID.v;
		params.MLME_DISCONNECT_indication.BANID.v = params.MACS_DATA_indication.BANID.v;

		params.MLME_DISCONNECT_indication.fullAddr.v[0] = MACGet();
		params.MLME_DISCONNECT_indication.fullAddr.v[1] = MACGet();
		params.MLME_DISCONNECT_indication.fullAddr.v[2] = MACGet();
		params.MLME_DISCONNECT_indication.fullAddr.v[3] = MACGet();
		params.MLME_DISCONNECT_indication.fullAddr.v[4] = MACGet();
		params.MLME_DISCONNECT_indication.fullAddr.v[5] = MACGet();

		params.MLME_DISCONNECT_indication.senderFullAddr.v[0] = MACGet();
		params.MLME_DISCONNECT_indication.senderFullAddr.v[1] = MACGet();
		params.MLME_DISCONNECT_indication.senderFullAddr.v[2] = MACGet();
		params.MLME_DISCONNECT_indication.senderFullAddr.v[3] = MACGet();
		params.MLME_DISCONNECT_indication.senderFullAddr.v[4] = MACGet();
		params.MLME_DISCONNECT_indication.senderFullAddr.v[5] = MACGet();

		for(mac__i = 0; mac__i < 6; mac__i++)
			if(macBIB.fullAddr.v[mac__i] != params.MLME_DISCONNECT_indication.fullAddr.v[mac__i]){
				return NO_PRIMITIVE;
			}
#ifdef I_AM_HUB
		for(mac__i = 0; mac__i < 6; mac__i++){
			if(macBIB.nodeList[getIndexFromNodeList(params.MACS_DATA_indication.SenderID)].fullAddr.v[mac__i] != params.MLME_DISCONNECT_indication.senderFullAddr.v[mac__i])
				return NO_PRIMITIVE;
		}
#endif
#ifdef I_AM_NODE
		for(mac__i = 0; mac__i < 6; mac__i++){
			if(macBIB.hubFullAddr.v[mac__i] != params.MLME_DISCONNECT_indication.senderFullAddr.v[mac__i])
				return NO_PRIMITIVE;
		}
#endif
		if(mac_frameControl.bits.AckPolicy == mac_I_ACK){
			mac_packetHeader = mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = 0x10;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
#ifdef I_AM_HUB
			mac_TxBuffer[mac_packetHeader++] = macBIB.nodeList[getIndexFromNodeList(params.MLME_DISCONNECT_indication.SenderID)].abbrAddr.v;
#endif
#ifdef I_AM_NODE
			mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;
#endif
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

			//帧尾包装，校验和，暂缺
			mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;
#ifdef DEBUG
			printf("收到的是DISCONNECTION_FRAME，且需要回复ACK，ACK已成帧\n");
#endif
			mac_currentPacket.frameType_Subtype = mac_I_ACK_FRAME;
			mac_currentPacket.length = 0;
			mac_TasksPending.bit.ackSending = 1;
		}
#ifdef I_AM_NODE
		macBIB.abbrAddr.v = mac_UNCONNECTED_NID;
#endif
#ifdef I_AM_HUB
		DeNodeFromNodeList( params.MACS_DATA_indication.SenderID );
#endif
		return MLME_DISCONNECT_indication;
	}
	else{
#ifdef DEBUG
		printf( "**ERROR: Can't find this case!  @MACMngCtrlFrameReception(...)" );
#endif
		return NO_PRIMITIVE;
	}
}

/**********************************************************************
**函数原型:		WBAN_PRIMITIVE MACDataFrameReception()
**入口参数:   	void
**返 回 值：	WBAN_PRIMITIVE
**说    明：	处理MAC部分接收到的数据类型的帧
***********************************************************************/
WBAN_PRIMITIVE MACDataFrameReception()
{
	params.MACS_DATA_indication.frameTypeSubtype = mac_frameControl.bits.FrameType_Subtype;
	params.MACS_DATA_indication.RxDSN = mac_frameControl.bits.SequenceNumber_PollPostWindow;
#ifdef I_AM_HUB
	macBIB.macRxDSN[getIndexFromNodeList(params.MACS_DATA_indication.SenderID)]++;
	macBIB.macRxDSN[getIndexFromNodeList(params.MACS_DATA_indication.SenderID)] = params.MACS_DATA_indication.RxDSN; //是不是应该考虑如果收到的和本地计数的不一样的情况？
#endif
#ifdef I_AM_NODE
	macBIB.macRxDSN++;
	macBIB.macRxDSN = params.MACS_DATA_indication.RxDSN;
#endif
	params.MACS_DATA_indication.securityLevel = mac_frameControl.bits.SecurityLevel;
	params.MACS_DATA_indication.frameTypeSubtype = mac_frameControl.bits.FrameType_Subtype;
	params.MACS_DATA_indication.fragmentNumber = mac_frameControl.bits.FragmentNumber_Next_Coexistence;
	params.MACS_DATA_indication.NonfinalFragment = mac_frameControl.bits.NonfinalFragment_Cancel_Scal_Inactive;
	params.MACS_DATA_indication.msduLength = params.PL_DATA_indication.psduLength - 2;//减去MAC的校验和两个字节

	if(mac_frameControl.bits.AckPolicy == mac_I_ACK){
		mac_packetHeader = mac_PACKET_HEADER_START;
		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = 0x10;
		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = 0x00;

		mac_TxBuffer[mac_packetHeader++] = params.MACS_DATA_indication.SenderID.v;
		mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
		mac_TxBuffer[mac_packetHeader++] = params.MACS_DATA_indication.BANID.v;

				//帧尾包装，校验和，暂缺
		mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
		mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;
#ifdef DEBUG
		printf("收到的是数据帧，且需要回复ACK，ACK已成帧\n");
#endif
		mac_TasksPending.bit.ackSending = 1;
		return MACS_DATA_indication;
	}
	else if(mac_frameControl.bits.AckPolicy == mac_N_ACK){
#ifdef DEBUG
		printf("收到的是数据帧，且不需要回复ACK\n");
#endif
	}
	return MACS_DATA_indication;
}

/**********************************************************************
**函数原型:		WBAN_PRIMITIVE MACIAckFrameReception( BYTE lastFrameSubtype )
**入口参数:   	BYTE lastFrameSubtype
				传入mac_currentPacket.frameSubtype
**返 回 值：	WBAN_PRIMITIVE
**说    明：	处理MAC层接收到的I-ACK帧，是对MAC部分的管理类型的帧和数据类型的帧的回复
***********************************************************************/
WBAN_PRIMITIVE MACIAckFrameReception( BYTE lastFrameSubtype )
{
#ifdef DEBUG
	printf("收到的是ACK帧\n");
#endif

#ifdef I_AM_NODE
	if(lastFrameSubtype == mac_CONNECTION_REQUEST_FRAME && mac_TasksPending.bit.packetWaitingAck == 1){
		mac_TasksPending.bit.packetWaitingAck = 0;
		mac_currentPacket.lastPacketStatus = mac_SUCCESS;
		mac_currentPacket.retry = 0;
#ifdef DEBUG
		printf("Connection_request_frame已收到ack\n");
#endif
		return NO_PRIMITIVE;
	}
#endif

#ifdef I_AM_HUB
	if(lastFrameSubtype == mac_CONNECTION_ASSIGNMENT_FRAME && mac_TasksPending.bit.packetWaitingAck == 1){
		mac_TasksPending.bit.packetWaitingAck = 0;
		mac_currentPacket.lastPacketStatus = mac_SUCCESS;
		mac_currentPacket.retry = 0;
#ifdef DEBUG
		printf("Connection_assignment_frame已收到ack\n");
#endif
		params.HME_MLME_CONNECT_confirm.ConnectedNID.v = params.MACS_DATA_indication.RecipientID.v;
		params.HME_MLME_CONNECT_confirm.status = mac_SUCCESS;
		return HME_MLME_CONNECT_confirm;
	}
#endif

	else if( lastFrameSubtype == mac_DISCONNECTION_FRAME && mac_TasksPending.bit.packetWaitingAck == 1 ){
		mac_TasksPending.bit.packetWaitingAck = 0;
		mac_currentPacket.lastPacketStatus = mac_SUCCESS;
		mac_currentPacket.retry = 0;
#ifdef DEBUG
		printf("Disonnection_frame已收到ack\n");
#endif
#ifdef I_AM_NODE
		macBIB.abbrAddr.v = mac_UNCONNECTED_NID;
#endif
#ifdef I_AM_HUB
		macBIB.nodeList[getIndexFromNodeList(params.MACS_DATA_indication.SenderID)].abbrAddr.v = mac_UNCONNECTED_BROADCAST_NID;
#endif
		params.MLME_DISCONNECT_confirm.status = mac_SUCCESS;
		return MLME_DISCONNECT_confirm;
	}
	else if( (lastFrameSubtype & 0xF0) == mac_DATA_FRAME && mac_TasksPending.bit.packetWaitingAck == 1 ){
		mac_TasksPending.bit.packetWaitingAck = 0;
		mac_currentPacket.lastPacketStatus = mac_SUCCESS;
		mac_currentPacket.retry = 0;
		DeDataQueue();
#ifdef DEBUG
		printf("由于数据已收到ack，因此将该数据从数据队列前端删除 @MACIAckFrameReception(...)\n");
#endif
		params.MACS_DATA_confirm.status = mac_SUCCESS;
#ifdef I_AM_NODE
		params.MACS_DATA_confirm.TxDSN = macBIB.macTxDSN;
#endif
#ifdef I_AM_HUB
		params.MACS_DATA_confirm.TxDSN = macBIB.macTxDSN[getIndexFromNodeList(params.MACS_DATA_indication.SenderID)];
#endif
		params.MACS_DATA_confirm.RecipientID.v = params.MACS_DATA_indication.SenderID.v;
		return MACS_DATA_confirm;
	}
	else{
#ifdef DEBUG
		printf( "**ERROR: Can't find this case!  @MACIAckFrameReception(...)" );
#endif
		return NO_PRIMITIVE;
	}
}

#ifdef I_AM_HUB
/**********************************************************************
**函数原型:		WBAN_PRIMITIVE packetWaitingConnectionRequestframeProcess()
**入口参数:   	void
**返   回 	值：		WBAN_PRIMITIVE
**说      	明：		等待ConnectionRequestframe的处理过程
***********************************************************************/
WBAN_PRIMITIVE packetWaitingConnectionRequestProcess(){
	TICK t;
    uint64_t temp;
    temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
#ifdef DEBUG
	printf("进入ConnectionRequestframe等待...\n");
#endif
	t.Val = TickGet().Val - mac_currentPacket.startTime.Val;
	if(t.Val > 0 && t.Val <= temp){
		return NO_PRIMITIVE;
	}
	else{
#ifdef DEBUG
		printf("ConnectionRequestframe没有在规定的时间内接收到，超时！\n");
#endif
		mac_currentPacket.lastPacketStatus = mac_FAIL;
		mac_currentPacket.retry++;
		if(mac_currentPacket.retry <= mac_mUnscheduledAllocationAborted){
			if(mac_currentPacket.frameType_Subtype == mac_T_POLL_FRAME){
				macBIB.BeaconStartTime = TickGet();
				return MACFrameTransmission( mac_T_POLL_FRAME, mac_DIRECT, mac_TxBuffer, 13 );
			}
		}
		else{
			 mac_TasksPending.bit.packetWaitingConnectionRequest = 0;
			 params.HME_MLME_CONNECT_confirm.ConnectedNID.v = mac_UNCONNECTED_BROADCAST_NID;
			 params.HME_MLME_CONNECT_confirm.status = mac_NO_DATA;
             return HME_MLME_CONNECT_confirm;
		}
	}
	return NO_PRIMITIVE;
}
#endif

/**********************************************************************
**函数原型:		WBAN_PRIMITIVE packetWaitingAckProcess()
**入口参数:   	void
**返   回 	值：		WBAN_PRIMITIVE
**说      	明：		等待ack的处理过程
***********************************************************************/
WBAN_PRIMITIVE packetWaitingAckProcess(){
	TICK t;
#ifdef DEBUG
	printf("进入ACK等待...\n");
#endif
	t.Val = TickGet().Val - mac_currentPacket.startTime.Val;
	if(t.Val > 0 && t.Val <= (mac_ACKTimeOut * mac_CLK_Freq)){
		return NO_PRIMITIVE;
	}
	else{
#ifdef DEBUG
		printf("ACK没有在规定的时间内接收到，超时！ @%ld\n", TickGet().Val);
#endif
		mac_currentPacket.lastPacketStatus = mac_FAIL;
		mac_currentPacket.retry++;
		mac_TasksPending.bit.packetWaitingAck = 0;
#ifdef I_AM_NODE
		if(mac_currentPacket.frameType_Subtype == mac_CONNECTION_REQUEST_FRAME){
			//params.MLME_RESET_request.setDefaultBIB = TRUE;
			//MACTasks(MLME_RESET_request);
		}
#endif
#ifdef I_AM_HUB
		if(mac_currentPacket.frameType_Subtype == mac_CONNECTION_ASSIGNMENT_FRAME){
			return MACFrameTransmission( mac_CONNECTION_ASSIGNMENT_FRAME, mac_DIRECT, mac_TxBuffer, 35 );
		}
#endif
		else if(mac_currentPacket.frameType_Subtype & 0xF0 == mac_DATA_FRAME){
			if(mac_currentPacket.retry > mac_RETRY_LIMITATION){
				DeDataQueue();
#ifdef DEBUG
				printf("由于数据超过重传次数限制，因此将该数据从数据队列前端删除 @packetWaitingAckProcess()\n");
#endif
				params.MACS_DATA_confirm.status = mac_NO_ACK;
#ifdef I_AM_NODE
				params.MACS_DATA_confirm.TxDSN = macBIB.macTxDSN;
#endif
#ifdef I_AM_HUB
				ABBR_ADDR addr_temp;
				addr_temp.v = *(mac_currentPacket.buffer + 5);
				params.MACS_DATA_confirm.TxDSN = macBIB.macTxDSN[getIndexFromNodeList(addr_temp)];

#endif
				params.MACS_DATA_confirm.RecipientID.v = *(mac_currentPacket.buffer + 4);

				return MACS_DATA_confirm;
			}
		}
	}
	return NO_PRIMITIVE;
}

/**********************************************************************
**函数原型:		BYTE Encrypt(WORD len, BYTE* buf)
**入口参数:		BYTE len, BYTE* buf
**返   回 	值：		BYTE 加密后的字节长度
**说      	明：		加密数据，并返回加密后的数据长度
***********************************************************************/
BYTE Encrypt(WORD len, BYTE* buf){
	int _len = (int)len;
	BYTE* _buf = buf;
#ifdef DEBUG
	WORD i;
	printf("原数据为：\n");
	for(i = 0; i < len; i++){
		printf("0x%x ", buf[i]);
	}
	printf("\n");
#endif

	mac_encrypt(_buf, macBIB.MK, _len);

	if(len > ((len / 16) * 16)){
		_len = ((len / 16) + 1) * 16;
	}
#ifdef DEBUG
	printf("加密后数据为：\n");
	for(i = 0; i < _len; i++){
		printf("0x%x ", buf[i]);
	}
	printf("\n");
	printf("加密后的数据长度为：%d\n", _len);
#endif

	return _len;
}

/**********************************************************************
**函数原型:		BYTE Decrypt(WORD len, BYTE* buf)
**入口参数:		BYTE len, BYTE* buf
**返   回 	值：		BYTE 解密后的字节长度
**说      	明：		解密数据，并返回解密后的数据长度
***********************************************************************/
BYTE Decrypt(WORD len, BYTE* buf){
	int _len = (int)len;
	BYTE* _buf = buf;
#ifdef DEBUG
	WORD i;
	printf("原数据为：\n");
	for(i = 0; i < len; i++){
		printf("0x%x ", buf[i]);
	}
	printf("\n");
#endif

	mac_decrypt(_buf, macBIB.MK, _len);

	if(len > ((len / 16) * 16)){
		_len = ((len / 16) + 1) * 16;
	}
#ifdef DEBUG
	printf("解密后数据为：\n");
	for(i = 0; i < _len; i++){
		printf("0x%x ", buf[i]);
	}
	printf("\n");
	printf("解密后的数据长度为：%d\n", _len);
#endif

	return _len;
}

#ifdef I_AM_HUB
/**********************************************************************
**函数原型:		WBAN_PRIMITIVE beaconTransmitProcess()
**入口参数:   	void
**返   回 	值：		WBAN_PRIMITIVE
**说      	明：		当超帧周期到来时，如果高层不给START原语，MAC默认将上次的Beacon发送
***********************************************************************/
WBAN_PRIMITIVE beaconTransmitProcess()
{
	//帧头包装
	mac_packetHeader = mac_PACKET_HEADER_START;
	mac_TxBuffer[mac_packetHeader++] = 0x00;
	mac_TxBuffer[mac_packetHeader++] = 0x00;
	mac_TxBuffer[mac_packetHeader++] = ++macBIB.macBSN;
	mac_TxBuffer[mac_packetHeader++] = 0x00;
	mac_TxBuffer[mac_packetHeader++] = mac_BROADCAST_NID;
	mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
	mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

	//payload包装
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[0];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[1];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[2];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[3];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[4];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[5];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[6];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[7];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[8];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[9];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[10];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[11];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[12];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[13];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[14];
	mac_TxBuffer[mac_packetHeader++] = macBIB.macBeaconPayload[15];

	//帧尾包装，校验和，暂缺
	mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
	mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

	return MACFrameTransmission( mac_BEACON_FRAME, mac_DIRECT, mac_TxBuffer, 25 );
}
#endif

/**********************************************************************
**函数原型:		BYTE getBackoffCounter()
**入口参数:   	void
**返   回 	值：		BYTE
**说      	明：		根据不同的条件，获取退避计数器
***********************************************************************/
BYTE getBackoffCounter(){
	BYTE CWmin, CWmax, _BC;

	if( (params.MACS_DATA_request.frameTypeSubtype & 0xF0) == mac_DATA_FRAME ){
		switch(params.MACS_DATA_request.frameTypeSubtype){
		case mac_DATA_FRAME_USER_PRIORITY_0:
			CWmin = 4;
			CWmax = 6;
			break;
		case mac_DATA_FRAME_USER_PRIORITY_1:
			CWmin = 4;
			CWmax = 5;
			break;
		case mac_DATA_FRAME_USER_PRIORITY_2:
			CWmin = 3;
			CWmax = 5;
			break;
		case mac_DATA_FRAME_USER_PRIORITY_3:
			CWmin = 3;
			CWmax = 4;
			break;
		case mac_DATA_FRAME_USER_PRIORITY_4:
			CWmin = 2;
			CWmax = 4;
			break;
		case mac_DATA_FRAME_USER_PRIORITY_5:
			CWmin = 2;
			CWmax = 3;
			break;
		case mac_DATA_FRAME_USER_PRIORITY_6:
			CWmin = 1;
			CWmax = 3;
			break;
		case mac_DATA_FRAME_EMERGENCY:
			CWmin = 0;
			CWmax = 2;
		default:
#ifdef DEBUG
			printf("mac_CW() can't find this case\n");
#endif
			;
		}
	}
	else if( (mac_currentPacket.frameType_Subtype & 0xF0) == mac_DATA_FRAME ){
		switch(mac_currentPacket.frameType_Subtype){
		case mac_DATA_FRAME_USER_PRIORITY_0:
			CWmin = 4;
			CWmax = 6;
			break;
		case mac_DATA_FRAME_USER_PRIORITY_1:
			CWmin = 4;
			CWmax = 5;
			break;
		case mac_DATA_FRAME_USER_PRIORITY_2:
			CWmin = 3;
			CWmax = 5;
			break;
		case mac_DATA_FRAME_USER_PRIORITY_3:
			CWmin = 3;
			CWmax = 4;
			break;
		case mac_DATA_FRAME_USER_PRIORITY_4:
			CWmin = 2;
			CWmax = 4;
			break;
		case mac_DATA_FRAME_USER_PRIORITY_5:
			CWmin = 2;
			CWmax = 3;
			break;
		case mac_DATA_FRAME_USER_PRIORITY_6:
			CWmin = 1;
			CWmax = 3;
			break;
		case mac_DATA_FRAME_EMERGENCY:
			CWmin = 0;
			CWmax = 2;
		default:
#ifdef DEBUG
			printf("mac_CW() can't find this case\n");
#endif
			;
		}
	}

	if(mac_currentPacket.lastPacketStatus == mac_NONE){
		mac_CW = CWmin;
	}
	else if(mac_currentPacket.lastPacketStatus == mac_SUCCESS){
		mac_CW = CWmin;
	}
	else if(mac_currentPacket.lastPacketStatus == mac_N_ACK){
		mac_CW = mac_CW;
	}
	else if(mac_currentPacket.lastPacketStatus == mac_FAIL){
		if(mac_currentPacket.retry % 2 == 1){
			mac_CW = mac_CW;
		}
		else if(mac_currentPacket.retry % 2 == 0){
			mac_CW = mac_CW + 1;
		}
	}

	if(mac_CW > CWmax){
		mac_CW = CWmax;
	}

	_BC = (BYTE)((ReadCpuTimer0() & ((0x00000001 << mac_CW) - 1)) + 1);

	return _BC;
}

/**********************************************************************
**函数原型:		double getCP(BYTE type)
**入口参数:   	void
**返   回 	值：		double
**说      	明：		根据不同的条件，获取CP,管理帧的aloha部分待补充
***********************************************************************/
double getCP(BYTE type){
	double cp = 0.0, cpmax = 0.0, cpmin = 0.0;

	if( type == mac_DATA_FRAME ){
		if( mac_TasksPending.bit.dataInBuffer == 1 ){
			switch( GetDataQueuePacketPriority() ){
			case mac_DATA_FRAME_USER_PRIORITY_0:
				cpmax = 0.125;
				cpmin = 0.0625;
				break;
			case mac_DATA_FRAME_USER_PRIORITY_1:
				cpmax = 0.125;
				cpmin = 0.09375;
				break;
			case mac_DATA_FRAME_USER_PRIORITY_2:
				cpmax = 0.25;
				cpmin = 0.09375;
				break;
			case mac_DATA_FRAME_USER_PRIORITY_3:
				cpmax = 0.25;
				cpmin = 0.125;
				break;
			case mac_DATA_FRAME_USER_PRIORITY_4:
				cpmax = 0.375;
				cpmin = 0.125;
				break;
			case mac_DATA_FRAME_USER_PRIORITY_5:
				cpmax = 0.375;
				cpmin = 0.1875;
				break;
			case mac_DATA_FRAME_USER_PRIORITY_6:
				cpmax = 0.5;
				cpmin = 0.1875;
				break;
			case mac_DATA_FRAME_EMERGENCY:
				cpmax = 1.0;
				cpmin = 0.25;
			default:
#ifdef DEBUG
				printf("getCP() can't find this case\n");
#endif
				;
			}

			if( mac_currentPacket.lastPacketStatus == mac_NONE ){
				cp = cpmax;
			}
			else if( mac_currentPacket.lastPacketStatus == mac_SUCCESS ){
				cp = cpmax;
			}
			else if( mac_currentPacket.lastPacketStatus == mac_N_ACK ){
				cp = mac_CP;
			}
			else if( mac_currentPacket.lastPacketStatus == mac_FAIL ){
				if( mac_currentPacket.retry % 2 == 1 ){
					cp = mac_CP;
				}
				else if( mac_currentPacket.retry % 2 == 0 ){
					cp = mac_CP / 2.0;
				}
			}

			if(cp < cpmin){
				cp = cpmin;
			}
		}
	}
	else if( type == mac_MANAGEMENT_FRAME ){
		//待补充
	}
	else{
#ifdef DEBUG
		printf("Error!Wrong type in getCP()!");
#endif
	}

	return cp;
}

/**********************************************************************
**函数原型:		void Delay(WORD usecond)
**入口参数:   	WORD usecond
**返   回 	值：		void
**说      	明：		延时程序
***********************************************************************/
void Delay(WORD usecond){
	TICK t1,t2;
	t1 = TickGet();
	while(1){
		t2 = TickGet();
		if((WORD)(TickGetDiff(t1, t2)) > usecond * mac_CLK_Freq){
			break;
		}
	}
}

/**********************************************************************
**函数原型:		BOOL TransmitIt()
**入口参数:   	void
**返   回 	值：		BOOL
**说      	明：		CSMA/CA的传输程序(目前版本只有RAP期的传输，FRAME_TRANSACTION_TIME的估计目前是个常量)
***********************************************************************/
BOOL TransmitIt(){
	TICK t;
	BYTE CCAState;

	while(mac_BC != 0){
		t.Val = TickGet().Val - macBIB.BeaconStartTime.Val;
		if((t.Val >= macBIB.RAP1Start.Val) && (t.Val < macBIB.RAP1End.Val)){
			if(TickGetDiff(t, macBIB.RAP1End) < (mac_pCSMASlotLength + mac_FRAME_TRANSACTION_TIME) * mac_CLK_Freq){
#ifdef DEBUG
				printf("BC中止，原因距RAP1阶段结束时刻的剩余时间不足够传输一帧\n");
#endif
				return FALSE;
			}
		}else if((t.Val >= macBIB.RAP2Start.Val) && (t.Val < macBIB.RAP2End.Val) ){
			if(TickGetDiff(t, macBIB.RAP2End) < (mac_pCSMASlotLength + mac_FRAME_TRANSACTION_TIME) * mac_CLK_Freq){
#ifdef DEBUG
				printf("BC中止，原因距RAP2阶段结束时刻的剩余时间不足够传输一帧\n");
#endif
				return FALSE;
			}
		}else{
#ifdef DEBUG
			printf("BC中止，原因不在合适的阶段传输\n");
#endif
			return FALSE;
		}

		CCAState = CCA();
#ifdef DEBUG
		printf("CCA检测状态：%d\n", CCAState);
#endif
		if(CCAState == mac_IDLE){
			mac_BC--;
		}
		if(mac_BC != 0){
			Delay(mac_pCSMAMACPHYTime);
		}
#ifdef DEBUG
		printf("BC：%d\n", mac_BC);
#endif
	}
	return TRUE;
}

/**********************************************************************
**函数原型:		BOOL mngTransmitIt()
**入口参数:   	void
**返   回 	值：		BOOL
**说      	明：		CSMA/CA的传输程序，管理帧
***********************************************************************/
/*BOOL mngTransmitIt(){
	TICK t;
	BYTE CCAState;
	uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);

	while(mac_BC != 0){
		t.Val = TickGet().Val - macBIB.BeaconStartTime.Val;
		if((t.Val > 0) && (t.Val <= (macBIB.PollPostWindow + 1) * temp)){

		}else{
#ifdef DEBUG
			printf("BC中止，原因不在合适的阶段传输\n");
#endif
			return FALSE;
		}

		CCAState = CCA();
#ifdef DEBUG
		printf("CCA检测状态：%d\n", CCAState);
#endif
		if(CCAState == mac_IDLE){
			mac_BC--;
		}
		if(mac_BC != 0){
			Delay(mac_pCSMAMACPHYTime);
		}
#ifdef DEBUG
		printf("BC：%d\n", mac_BC);
#endif
	}
	return TRUE;
}
*/
/**********************************************************************
**函数原型:		BYTE getCurrentSlotNumber()
**入口参数:		void
**返   回 	值：		BYTE 当前的时隙序号
**说      	明：		返回当前的时隙序号
***********************************************************************/
BYTE getCurrentSlotNumber(){
	TICK t;
	DWORD temp;
	WORD _beaconPeriodLength;
	BYTE slot, _l, _r;

	if(macBIB.beaconPeriodLength == 0){
		_beaconPeriodLength = 256;
	}
	else{
		_beaconPeriodLength = macBIB.beaconPeriodLength;
	}

	t = TickGet();
	t.Val = t.Val - macBIB.BeaconStartTime.Val;
	temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);

	slot = _beaconPeriodLength >> 1;
	_l = _beaconPeriodLength >> 1;
	_r = _beaconPeriodLength - _l;

	for(; (_l +_r) != 1;){
		if(slot * temp <= t.Val){
			slot = slot + (_r >> 1);
			_l = _r >> 1;
			_r = _r - _l;
		}
		else{
			slot = slot - (_l >> 1);
			_r = _l >> 1;
			_l = _l - _r;
		}
	}

	_l = slot;
	if(_l * temp <= t.Val)
		slot = slot;
	else
		slot = slot - 1;

	return slot;
}

#ifdef I_AM_NODE
/**********************************************************************
**函数原型:		ABBR_ADDR GetAllocationLength()
**入口参数:
**返   回 	值：		a
**说      	明：		得到一个简明地址
***********************************************************************/
uint8_t GetAllocationLength( uint8_t DataArrivalRateIndicator )
{
	uint8_t len;
	uint32_t temp;
	if(DataArrivalRateIndicator <= (29 + 30 * macBIB.allocationSlotLength)){

		temp = mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution;
		len = ((DataArrivalRateIndicator + 1) * 4200) /  temp;
		return len;
	}
	else
		return 0xff;
}
#endif

#ifdef I_AM_HUB
/**********************************************************************
**函数原型:		ABBR_ADDR GetAbbrAddr()
**入口参数:
**返   回 	值：		a
**说      	明：		得到一个简明地址
***********************************************************************/
ABBR_ADDR GetAbbrAddr()
{
	ABBR_ADDR a;
    ABBR_ADDR ConnectedNID;
    for(a.v = 3; a.v < 0xf5; a.v++ )
    {
        if(macBIB.abbraddrList[a.v].isUsed == FALSE)
        break;
    }
    ConnectedNID = a;
    return ConnectedNID;
}
#endif

#ifdef I_AM_HUB
/**********************************************************************
**函数原型:		BYTE getIndexFromNodeList(ABBR_ADDR abbrAddr)
**入口参数:		BYTE v
**返   回 	值：		BYTE
**说      	明：		返回当前简明地址的节点在hub的NodeList中的索引号
***********************************************************************/
BYTE getIndexFromNodeList( ABBR_ADDR abbrAddr ){
	BYTE a = 0xFF;
	for(a = 0; a < mac_mMaxBANSize; a++){
		if(macBIB.nodeList[a].abbrAddr.v == abbrAddr.v){
			break;
		}
	}
	return a;
}

/**********************************************************************
**函数原型:		void EnNodeToNodeList( ABBR_ADDR abbrAddr, FULL_ADDR fullAddr, CAPABILITY capability )
**入口参数:		ABBR_ADDR abbrAddr
**入口参数:		FULL_ADDR fullAddr
**入口参数:		CAPABILITY capability
**返   回 	值：		void
**说      	明：		将入口参数的值加入到macBIB里的nodeList中
***********************************************************************/
void EnNodeToNodeList( ABBR_ADDR abbrAddr, FULL_ADDR fullAddr, CAPABILITY capability )
{
	BYTE a;
	//判断哪个下标现在是可用的
	for( a = 0; a < mac_mMaxBANSize; a++ ){
		if( macBIB.nodeList[a].isUsed == FALSE )
			break;
	}
	//再把各个域赋值到这个下标上
	macBIB.nodeList[a].isUsed = TRUE;
	macBIB.nodeList[a].abbrAddr.v = abbrAddr.v;
	macBIB.nodeList[a].fullAddr.v[0] = fullAddr.v[0];
	macBIB.nodeList[a].fullAddr.v[1] = fullAddr.v[1];
	macBIB.nodeList[a].fullAddr.v[2] = fullAddr.v[2];
	macBIB.nodeList[a].fullAddr.v[3] = fullAddr.v[3];
	macBIB.nodeList[a].fullAddr.v[4] = fullAddr.v[4];
	macBIB.nodeList[a].fullAddr.v[5] = fullAddr.v[5];
	macBIB.nodeList[a].capability.Val = capability.Val;
	macBIB.listNodeSum++;
}

/**********************************************************************
**函数原型:		void EnNodeToNodeList( ABBR_ADDR abbrAddr, BYTE AllocationSlot )
**入口参数:		ABBR_ADDR abbrAddr
**入口参数:		BYTE AllocationSlot
**返   回 	值：	sum_slot
**说      	明：	时隙的分配
***********************************************************************/
uint8_t AllocationSlot( ABBR_ADDR abbrAddr, BYTE AllocationSlot)
{
	BYTE a;
	mac_slotNumber = mac_RAP1_END_DEFAULT;
	//判断节点时隙是否已经分配过
	for( a = 0; a < mac_mMaxBANSize; a++ ){
		if( macBIB.nodeList[a].abbrAddr.v == abbrAddr.v )
		break;
	}

	macBIB.nodeList[getIndexFromNodeList( params.HME_MLME_CONNECT_response.AssocAbbrID )].IntervalStart = mac_slotNumber;
	mac_slotNumber = mac_slotNumber + AllocationSlot;

    macBIB.nodeList[getIndexFromNodeList( params.HME_MLME_CONNECT_response.AssocAbbrID )].IntervalEnd = mac_slotNumber;
    return mac_slotNumber;
}


/**********************************************************************
**函数原型:		void DeNodeFromNodeList( ABBR_ADDR abbrAddr )
**入口参数:		ABBR_ADDR abbrAddr
**返   回 	值：		void
**说      	明：		将简明地址为abbrAddr的节点从macBIB里的nodeList中删除
***********************************************************************/
void DeNodeFromNodeList( ABBR_ADDR abbrAddr )
{
	BYTE a;
	a = getIndexFromNodeList( abbrAddr );
	macBIB.nodeList[a].isUsed = FALSE;
	macBIB.listNodeSum--;
}

#endif

WORD do_crc(BYTE *message, WORD len)
{
	int i, j;
	WORD crc_reg = 0;
	WORD current;

	for (i = 0; i < len; i++)
	{
		current = message[i] << 8;
		for (j = 0; j < 8; j++)
		{
			if ((short)(crc_reg ^ current) < 0)
				crc_reg = (crc_reg << 1) ^ 0x1021;
			else
				crc_reg <<= 1;
			current <<= 1;
		}
	}
	return crc_reg;
}

void CRC_16_CCITT(BYTE *data, WORD length)
{
	WORD crc = 0;

	data[length - 2] = 0xFF;
	data[length - 1] = 0xFF;

	crc = do_crc(data, length - 2);

	data[length - 2] = (crc >> 8) & 0xFF;
	data[length - 1] = crc & 0xFF;
}

BOOL CRC_16_CCITT_check(BYTE *data, WORD length)
{
	WORD crc = 0;

	crc = do_crc(data, length);

	if (crc == 0)
		return TRUE;
	else
		return FALSE;
}

/**********************************************************************
**函数原型:		void InitDataQueue()
**入口参数:		void
**返   回 	值：		void
**说      	明：		初始化数据队列
***********************************************************************/
void InitDataQueue()
{
#ifdef DEBUG
	printf("\n初始化数据队列... @InitDataQueue()\n");
#endif
	//将dataQueue中的队首指针和队尾指针的值重置为0
	mac_dataQueue.front = 0;
	mac_dataQueue.rear = 0;

	//将dataQueue中的各个buffer的长度重置为0
	for(mac_i = 0; mac_i < mac_DATA_QUEUE_MAX_LENGTH; mac_i++)
		mac_dataQueue.packetLength[mac_i] = 0;

	//将dataQueue中的各个buffer的优先级重置为8
	for(mac_i = 0; mac_i < mac_DATA_QUEUE_MAX_LENGTH; mac_i++)
		mac_dataQueue.packetPriority[mac_i] = 8;

	//将dataQueue中的各个buffer的ack机制重置为0
	for(mac_i = 0; mac_i < mac_DATA_QUEUE_MAX_LENGTH; mac_i++)
		mac_dataQueue.packetAckPolicy[mac_i] = 0;

	//将dataQueue中的各个buffer的发送速率重置为0
	for(mac_i = 0; mac_i < mac_DATA_QUEUE_MAX_LENGTH; mac_i++)
		mac_dataQueue.packetInformationDataRate[mac_i] = 0;

	mac_TasksPending.bit.dataInBuffer = 0;
#ifdef DEBUG
	printf( "数据队列长度为%d\n", mac_DATA_QUEUE_MAX_LENGTH );
	printf( "队列中每个Buffer长度为 %d Bytes\n", mac_PACKET_BUFFER_SIZE );
#endif
}

/**********************************************************************
**函数原型:		BOOL EnDataQueue(BYTE _bufferTemp[], WORD _bufferTempLength, BYTE _bufferTempPriority, BYTE _bufferTempInformationDataRate, BYTE _bufferTempAckPolicy)
**入口参数:		BYTE _bufferTemp[]	：帧包装时临时使用的缓存buffer
				WORD _bufferTempLength	：帧包装后的有效长度，包括帧头，payload
				BYTE _bufferTempPriority	：帧的优先级
				BYTE _bufferTempInformationDataRate	：帧的发送速率
				BYTE _bufferTempAckPolicy	:	帧的ACK请求类型
**返   回 	值：		BOOL	：	FALSE  到来的数据太快，将还没有发出去的队首数据淹没了
**   						TRUE   成功将数据加入到dataQueue
**说      	明：		将原语SPI中的数据原语加入数据队列
***********************************************************************/
BOOL EnDataQueue(BYTE _bufferTemp[], WORD _bufferTempLength, BYTE _bufferTempPriority, BYTE _bufferTempInformationDataRate, BYTE _bufferTempAckPolicy){
	WORD temp;

	//如果rear已经追上了front，则说明dataQueue的大小有点小，或者发送有问题，一直发不出去
	temp = mac_dataQueue.rear + 1;
	if(temp == mac_DATA_QUEUE_MAX_LENGTH)
		temp = 0;
	if(temp == mac_dataQueue.front)
		return FALSE;

	//将_bufferTemp中的数据复制到dataQueue中
	for(mac_i = 0; mac_i < _bufferTempLength; mac_i++)
		mac_dataQueue.packetBuffer[mac_dataQueue.rear][mac_i] = _bufferTemp[mac_i];

	mac_dataQueue.packetBuffer[mac_dataQueue.rear][mac_i++] = _bufferTemp[mac_PACKET_FCS_START];
	mac_dataQueue.packetBuffer[mac_dataQueue.rear][mac_i] = _bufferTemp[mac_PACKET_FCS_START + 1];

	//将_bufferTemp的有效长度和优先级赋值给dataQueue中
	mac_dataQueue.packetLength[mac_dataQueue.rear] = _bufferTempLength + mac_DATA_FCS_LENGTH;
	mac_dataQueue.packetPriority[mac_dataQueue.rear] = _bufferTempPriority;
	mac_dataQueue.packetInformationDataRate[mac_dataQueue.rear] = _bufferTempInformationDataRate;
	mac_dataQueue.packetAckPolicy[mac_dataQueue.rear] = _bufferTempAckPolicy;
	mac_TasksPending.bit.dataInBuffer = 1;

	//更新dataQueue的尾指针
	mac_dataQueue.rear++;
	if(mac_dataQueue.rear == mac_DATA_QUEUE_MAX_LENGTH)
		mac_dataQueue.rear = 0;

	return TRUE;
}

/**********************************************************************
**函数原型:		BOOL DeDataQueue()
**入口参数:		void
**返   回 	值：		BOOL	：	FALSE  队列为空
**   						TRUE   成功将dataQueue的队首数据删除
**说      	明：		将dataQueue中的队首的数据删除
***********************************************************************/
BOOL DeDataQueue(){
	//若队列为空，则返回FALSE
	if(mac_dataQueue.front == mac_dataQueue.rear)
		return FALSE;

	//将dataQueue的对应buffer的有效长度重置为0
	mac_dataQueue.packetLength[mac_dataQueue.front] = 0;

	//更新dataQueue的头指针
	mac_dataQueue.front++;
	if(mac_dataQueue.front == mac_DATA_QUEUE_MAX_LENGTH)
		mac_dataQueue.front = 0;

	if(GetDataQueueLength() == 0)
		mac_TasksPending.bit.dataInBuffer = 0;

	return TRUE;
}

/**********************************************************************
**函数原型:		BYTE* GetDataQueueHead()
**入口参数:		void
**返   回 	值：		BYTE*	：NULL 队列为空
**						  其他为dataQueue中队首的packetBuffer一维数组的地址
**说      	明：		获取dataQueue中队首的packetBuffer数组
***********************************************************************/
BYTE* GetDataQueueHead(){
	BYTE* temp;

	//若队列为空，则返回NULL
	if(mac_dataQueue.front == mac_dataQueue.rear)
		return NULL;

	//将队首的packetBuffer的地址传递给temp
	temp = mac_dataQueue.packetBuffer[mac_dataQueue.front];

	return temp;
}

/**********************************************************************
**函数原型:		WORD GetDataQueuePacketLength()
**入口参数:		void
**返   回 	值：		WORD		:dataQueue中队首的packetBuffer的数据长度
**说      	明：		获取dataQueue中队首的packetBuffer的数据长度
***********************************************************************/
WORD GetDataQueuePacketLength(){
	WORD temp;

	//若队列为空，则返回0xFF
	if(mac_dataQueue.front == mac_dataQueue.rear)
		return mac_ERROR;

	//将队首的packetLength传递给temp
	temp = mac_dataQueue.packetLength[mac_dataQueue.front];

	return temp;
}

/**********************************************************************
**函数原型:		BYTE GetDataQueuePacketPriority()
**入口参数:		void
**返   回 	值：		BYTE		:dataQueue中队首的packetBuffer的数据优先级
**说      	明：		获取dataQueue中队首的packetBuffer的数据优先级
***********************************************************************/
BYTE GetDataQueuePacketPriority(){
	BYTE temp;

	//若队列为空，则返回0
	if(mac_dataQueue.front == mac_dataQueue.rear)
		return mac_ERROR;

	//将队首的packetPriority传递给temp
	temp = mac_dataQueue.packetPriority[mac_dataQueue.front];

	return temp;
}

/**********************************************************************
**函数原型:		BYTE GetDataQueuePacketInformationDataRate()
**入口参数:		void
**返   回 	值：		BYTE		:dataQueue中队首的packetInformationDataRate
**说      	明：		获取dataQueue中队首的packetInformationDataRate
***********************************************************************/
BYTE GetDataQueuePacketInformationDataRate(){
	BYTE temp;

	//若队列为空，则返回0xFF
	if(mac_dataQueue.front == mac_dataQueue.rear)
		return mac_ERROR;

	//将队首的packetInformationDataRate传递给temp
	temp = mac_dataQueue.packetInformationDataRate[mac_dataQueue.front];

	return temp;
}

/**********************************************************************
**函数原型:		BYTE GetDataQueuePacketAckPolicy()
**入口参数:		void
**返   回 	值：		BYTE		:dataQueue中队首的packetAckPolicy
**说      	明：		获取dataQueue中队首的packetAckPolicy
***********************************************************************/
BYTE GetDataQueuePacketAckPolicy(){
	BYTE temp;

	//若队列为空，则返回0XFF
	if(mac_dataQueue.front == mac_dataQueue.rear)
		return mac_ERROR;

	//将队首的packetAckPolicy传递给temp
	temp = mac_dataQueue.packetAckPolicy[mac_dataQueue.front];

	return temp;
}

/**********************************************************************
**函数原型:		WORD GetDataQueueLength()
**入口参数:		void
**返   回 	值：		WORD	：队列中已经存入的packet数 （0 - DATA_QUEUE_MAX_LENGTH）
**说      	明：		获取dataQueue的已经存入的packet数
***********************************************************************/
WORD GetDataQueueLength(){
	WORD temp;

	//三种情况需要考虑，一种是rear在front之前，一种是rear在front之后，一种是rear和front重合
	temp = mac_dataQueue.rear + mac_DATA_QUEUE_MAX_LENGTH - mac_dataQueue.front;
	if(temp >= mac_DATA_QUEUE_MAX_LENGTH)
		temp = mac_dataQueue.rear - mac_dataQueue.front;

	return temp;
}
