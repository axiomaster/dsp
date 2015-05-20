/*********************************************************************
 *
 *                  WbanMAC Header File
 *
 *********************************************************************
 * FileName:        WbanMAC.h
 * Company:         Xidian University
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Yueyang Song       2013.10.02
 *Yueyang Song 		 2013.11.18		v1.2
 *Yueyang Song		 2014.11.22		v2.0
 *Yanle Lei			 2014.11.22		v2.0
 ********************************************************************/
#ifndef _WbanMAC
#define _WbanMAC
#include "Wban.h"
#include "WbanTasks.h"
#include "CpuTimers.h"
/*********************************************************************
*MAC帧头中FrameControl的定义
*以及各域的值
**********************************************************************/
typedef union _MAC_FRMAE_CONTROL{
	DWORD Val;
	struct _byte{
		BYTE b1;
		BYTE b2;
		BYTE b3;
		BYTE b4;
	}bytes;
	struct _MAC_FRAME_CONTROL_bits{
		DWORD ProtocolVersion							:1;
		DWORD AckPolicy 								:2;
		DWORD SecurityLevel								:2;
		DWORD TKIndex									:1;
		DWORD BANSecurity_Relay							:1;
		DWORD AckTiming_EAPIndicator_FirstFrameOnTime	:1;
//		/DWORD FrameSubtype								:4;
		DWORD FrameType_Subtype							:6;
		DWORD MoreData									:1;
		DWORD LastFrame_AccessMode_B2					:1;
		DWORD SequenceNumber_PollPostWindow				:8;
		DWORD FragmentNumber_Next_Coexistence			:3;
		DWORD NonfinalFragment_Cancel_Scal_Inactive		:1;
		DWORD Reserved_leftBYTEs						:4;
	}bits;
}MAC_FRAME_CONTROL;

/*********************************************************************
*MAC的Payload中PHY/MAC Capability的定义
*以及各域的值
**********************************************************************/
typedef union _CAPABILITY{
	DWORD Val;
	struct byte_0{
		BYTE b0;
		BYTE b1;
		BYTE b2;
		BYTE b3;
	}bytes;
	struct _CAPABILITY_bits{
		DWORD CSMACA						:1;
		DWORD SlottedAlohaAccess			:1;
		DWORD TypeIPollingAccess			:1;
		DWORD TypeIIPollingAccess			:1;
		DWORD ScheduledAccess				:1;
		DWORD UnscheduledAccess				:1;
		DWORD Fragmentation_Reassembly		:1;
		DWORD CommandFrames					:1;
		DWORD NodeAlwaysActive_HubClockPPM	:1;
		DWORD GuardTimeProvisioning			:1;
		DWORD LAck_BAck						:1;
		DWORD GAck							:1;
		DWORD RelayingNode					:1;
		DWORD RelayedHub_Node				:1;
		DWORD BeaconShifting				:1;
		DWORD ChannelHopping				:1;
		DWORD DataSubtypes					:4;
		DWORD Reserved						:4;
		DWORD DataRate0						:1;
		DWORD DataRate1						:1;
		DWORD DataRate2						:1;
		DWORD DataRate3						:1;
		DWORD DataRate4						:1;
		DWORD DataRate5						:1;
		DWORD DataRate6						:1;
		DWORD DataRate7						:1;
	}bits;
}CAPABILITY;

#ifdef I_AM_HUB
/*********************************************************************
*HUB中的Node记录表
**********************************************************************/
typedef struct _NODE_LIST{
	ABBR_ADDR abbrAddr;
	FULL_ADDR fullAddr;
	CAPABILITY capability;
	BOOL isUsed;
	BYTE IntervalStart;
	BYTE IntervalEnd;
}NODE_LIST;
#endif

#ifdef I_AM_HUB
/*********************************************************************
*HUB中的Node简明地址记录表
**********************************************************************/
typedef struct _ABBRADDR_LIST{
	BOOL isUsed;
}ABBRADDR_LIST;
#endif

#ifdef I_AM_HUB
/*********************************************************************
*HUB中的UplinkRequestIE记录表
**********************************************************************/
typedef struct _UplinkRequestIE{
	BYTE AllocationLength;
}UplinkRequestIE;
#endif
/*********************************************************************
*一些常量的定义
**********************************************************************/
//一些默认的简明地址
#define mac_UNCONNECTED_BROADCAST_NID	0x00
#define mac_UNCONNECTED_NID				0x01
//Connected_NID 					0x02-0xF5
//Reserved							0xF6
//Multicast_NID						0xF7-0xFD
#define mac_LOCAL_BROADCAST_NID			0xFE
#define mac_BROADCAST_NID				0xFF

//AckPolicy value
#define mac_N_ACK 0x00
#define mac_I_ACK 0x01
#define mac_L_ACK 0x03
#define mac_B_ACK 0x02

//SecurityLevel value
#define mac_LEVEL0 0x00
#define mac_LEVEL1 0x01
#define mac_LEVEL2 0x02

//FrameType and FrameSubtype value
#define mac_MANAGEMENT_FRAME	0x00
#define mac_CONTROL_FRAME		0x10
#define mac_DATA_FRAME			0x20

#define mac_BEACON_FRAME					0x00
#define mac_SECURITY_ASSOCIATION_FRAME		0x02
#define mac_SECURITY_DISASSOCIATION_FRAME	0x03
#define mac_PTK_FRAME						0x04
#define mac_GTK_FRAME						0x05
#define mac_CONNECTION_REQUEST_FRAME		0x08
#define mac_CONNECTION_ASSIGNMENT_FRAME		0x09
#define mac_DISCONNECTION_FRAME				0x0A
#define mac_COMMAND_FRAME					0x0F

#define mac_I_ACK_FRAME			0x10
#define mac_B_ACK_FRAME			0x11
#define mac_I_ACK_POLL_FRAME	0x14
#define mac_B_ACK_POLL_FRAME	0x15
#define mac_POLL_FRAME			0x16
#define mac_T_POLL_FRAME		0x17
#define mac_WAKEUP_FRAME		0x1E
#define mac_B2_FRAME			0x1F

//Data Frame
#define mac_DATA_FRAME_USER_PRIORITY_0		0x20
#define mac_DATA_FRAME_USER_PRIORITY_1		0x21
#define mac_DATA_FRAME_USER_PRIORITY_2		0x22
#define mac_DATA_FRAME_USER_PRIORITY_3		0x23
#define mac_DATA_FRAME_USER_PRIORITY_4		0x24
#define mac_DATA_FRAME_USER_PRIORITY_5		0x25
#define mac_DATA_FRAME_USER_PRIORITY_6		0x26
#define mac_DATA_FRAME_EMERGENCY			0x27

//txType
#define mac_DIRECT 		0x01 	//0b00000001
#define mac_CSMACA		0x02 	//0b00000010
#define mac_PROB		0x04 	//0b00000100
#define mac_SCHEDULE	0x08 	//0b00001000

//MAC sublayer parameters
#define mac_mUnscheduledAllocationAborted 32
#define mac_mMaxABBRADDRSize 243
#define mac_mMaxBANSize 64
#define mac_pAllocationSlotMin 500
#define mac_pAllocationSlotResolution 500
#define mac_pCSMAMACPHYTime 40
#define mac_pCCATime (63.0 / mac_SymbolRate)
#define mac_pCSMASlotLength (mac_pCCATime + mac_pCSMAMACPHYTime)
#define mac_pSIFS 75
#define mac_pExtraIFS 10
#define mac_ACKTimeOut   23269	//1000   us,mac_pCSMAMACPHYTime + (maxFrameLength / minDataRate) + mac_pCSMAMACPHYTime + mTimeOut +
								//	mac_pCSMAMACPHYTime + (ackFrameLength / minDataRate) + mac_pCSMAMACPHYTime,
								//  其中maxFrameLength = 256, minDataRate = 91900, ackFrameLength = 9

//以下status常量为自己的定义
#define mac_NONE	0x02							//用于指示之前从没有进行传输
#define mac_FAIL	0x03							//用于指示上一帧的传输失败
#define mac_IDLE	0x04							//用于CCA返回的结果
#define mac_BUSY	0x05							//用于CCA返回的结果
#define mac_ERROR	0xFF							//用于指示数据队列中的错误

#define mac_SUCCESS							0x00	//所指示的操作完全成功；在入网过程中，指示Connection request frame成功接收
#define mac_POLICY_RESTRICTION				0x01	//连接被拒绝-不符合hub的管理者/拥有者制定的访问政策限制
#define mac_FORMAT_ERROR					0x02	//连接被拒绝-无效的或者不支持的帧格式
#define mac_UNSECURED_COMMUNICATION			0x03	//连接被拒绝-与hub没有进行安全交互
#define mac_NO_CHANNEL						0x04	//连接被拒绝-没有多余的带宽
#define mac_NO_CONNECTED_NID				0x05	//连接被拒绝-没有多余的Connected_NID
#define mac_NO_INTERNAL_RESOURCE			0x06	//连接被拒绝-没有多余的内部资源
#define mac_UNSUPPORTED_SYNCHRONIZATION		0x07	//连接被拒绝-node的最大同步间隔过长，不支持
#define mac_UNSUPPORTED_CLOCK_PPM			0x08	//连接被拒绝-node的时钟PPM太大，不支持
#define mac_UNSUPPORTED_BEACON_SHIFTING		0x09	//连接被拒绝-hub支持BEACON漂移，但node不支持
#define mac_UNSUPPORTED_CHANNEL_HOPPING		0x0A	//连接被拒绝-hub支持跳频，但node不支持
//RESERVED					0x0A-0x0E	预留
#define mac_MODIFIED_ASSIGNMENT				0x10	//连接分配帧 Connection assignment frame 被修改
//RESERVED					0x11-0x1F	预留，用于组网状态指示
#define mac_TRX_OFF							0x20	//收发机关闭
#define mac_NO_ACK							0x21	//达到最大传输次数限制后没有收到ACK
#define mac_NO_DATA							0x22	//没有收到任何数据回应
#define mac_CHANNEL_ACCESS_FAILURE			0x23	//未能接入信道（如，csma发送失败）
#define mac_TRANSACTION_OVERFLOW			0x24	//没有多余的存储空间
#define mac_TRANSACTION_EXPIRED				0x25	//传输已过期，数据被删除
#define mac_INVALID_ADDRESS					0x26	//发送地址或接收地址无效
#define mac_INVALID_PARAMETER				0x27	//传输的原语参数不支持或者超出了取值范围
//RESERVED					0x28-0x3F	预留，用于数据传输状态指示
#define mac_NO_MK							0x40	//连接被拒绝-缺乏共享主密钥
#define mac_INVALID_SecuritySuitSelector	0x41	//传输的帧参数错误
//RESERVED					0x42-0x5F	预留，用于安全状态指示
//RESERVED					0x60-0xFF	预留，用于其他状态指示

#define mac_MAC_BEACON_PAYLOAD_LENGTH 16	//15 - 21
#define mac_DATA_HEAD_LENGTH 7
#define mac_DATA_FCS_LENGTH 2

#define HUB_ABBR_ADDR_DEFAULT 				0x02	//hub
#define mac_BANID_DEFAULT					0x02
#define mac_INIT_BEACON_PERIOD_LENGTH_DEFAULT	255		//1
#define mac_INIT_ALLOCATION_SLOT_LENGTH_DEFAULT	0x08//255
#define mac_BEACON_PERIOD_LENGTH_DEFAULT	255		//根据计算可变
#define mac_ALLOCATION_SLOT_LENGTH_DEFAULT	255		//根据计算可变，应该能够保证一个节点的完全接入
#define mac_RAP1_START_DEFAULT				1
#define mac_EarliestRAP1End                 0
#define mac_RAP1_END_DEFAULT				50
#define mac_EAP2_START_DEFAULT              130
#define mac_RAP2_START_DEFAULT				130
#define mac_RAP2_END_DEFAULT				180
#define mac_POLLPOSTWINDOW_DEFAULT			0x01   //使window时间大约为37.5ms
#define mac_NEXT_DEFAULT					0x00
#define mac_Minimum_CAP_Length              0x00

#define mac_RETRY_LIMITATION				0x05	//重传次数限制，目前暂定5

#define mac_BEACON_WITH_SUPERFRAME_CSMA			0x00
#define mac_BEACON_WITH_SUPERFRAME_ALOHA        0x01
#define mac_NONBEACON_WITH_SUPERFRAME           0x10
#define mac_NONBEACON_WITHOUT_SUPERFRAME	    0x11

//安全用到的参数
#define Nb 4
#define Nk 4
#define Nr 10
#define BUFFSIZE 128

//物理层 扰码种子，具体用途不详，默认为0
#define SCRAMBLER_SEED_0 0
#define SCRAMBLER_SEED_1 1

#define BAND_402M_405M		0
#define	BAND_420M_450M		1
#define BAND_863M_870M		2
#define BAND_902M_928M		3
#define BAND_950M_958M		4
#define BAND_2360M_2400M	5
#define BAND_2400M_2483M	6

#ifdef WBAN_BAND_402M_405M
#define mac_SymbolRate 0.1875
#define WBAN_BAND BAND_402M_405M
#endif
#ifdef WBAN_BAND_2400M_2483M
#define mac_SymbolRate 0.6
#define WBAN_BAND BAND_2400M_2483M
#endif

#define BURST_MODE_DEFAULT 0

//BAN Information Base
typedef struct _MAC_BIB{
	ABBR_ADDR abbrAddr;
	FULL_ADDR fullAddr;
	ABBR_ADDR BANID;
	BYTE macBSN;	//Beacon SN

#ifdef I_AM_NODE
	BYTE macTxDSN;	//data or management SN 八个优先级
	BYTE macRxDSN;
	ABBR_ADDR hubAddr;
	FULL_ADDR hubFullAddr;
	CAPABILITY hubCapability;
#endif

#ifdef I_AM_HUB
	BYTE macTxDSN[mac_mMaxBANSize];	//data or management SN 最多64个节点
	BYTE macRxDSN[mac_mMaxBANSize];
	NODE_LIST nodeList[mac_mMaxBANSize];
	BYTE listNodeSum;
	FULL_ADDR nodeFullAddrTemp;
	BYTE macBeaconPayload[mac_MAC_BEACON_PAYLOAD_LENGTH];
	ABBRADDR_LIST abbraddrList[mac_mMaxABBRADDRSize];
	UplinkRequestIE uplinkRequestIE;
#endif

	TICK RAP1Start;
	TICK RAP1End;	//macBIB.RAP1End * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution) * mac_CLK_Freq
	TICK RAP2Start;
	TICK RAP2End;
	TICK IntervalStart;				//这个变量目前版本设为已知，但实际上应该是assignment帧交互中获取
	TICK IntervalEnd;				//这个变量目前版本设为已知，但实际上应该是assignment帧交互中获取
	TICK BeaconStartTime;
	BYTE EarliestRAP1End;
	TICK EAP2Start;
	BYTE allocationSlotLength;
	BYTE beaconPeriodLength;
	BYTE DataArrivalRateIndicator;

	CAPABILITY capability;
	BYTE AccessMode;	//只有三种模式
	BYTE PollPostWindow;
	BYTE MoreData;
	BYTE Next;
	BYTE RequestedWakeupPeriod;		//这个变量目前版本设为已知，但实际上应该是assignment帧交互中获取
	BYTE AccessIndicator;

	//PHY层
	BYTE band;
	BYTE channelNumber;
	BYTE scramblerSeed;
	BYTE informationDataRate;

	//安全部分的macBIB
	BYTE SecurityLevel;
	BYTE PTK[16];
	BYTE MK[16];
	BOOL isEncrypt;		//是否需要加密的标志位
	BOOL isAuthenticated;
}MAC_BIB;
extern MAC_BIB macBIB;

typedef struct _Current_Packet{
	TICK startTime;
	WORD length;
	BYTE retry;
	BYTE lastPacketStatus;
	BYTE priority;
	BYTE ackPolicy;	//目前用于不需要回复ACK的数据类型的帧的上传原语操作
	ABBR_ADDR RecipientID; //目前用于不需要回复ACK的数据类型的帧的上传原语操作
	BYTE* buffer;
	BYTE informationDataRate;
	BYTE frameType_Subtype;
	BOOL non_finalFragment;
}CURRENT_PACKET;

/*********************************************************************
*对数据循环队列的定义
**********************************************************************/
typedef struct _Data_Queue{
	WORD front;
	WORD rear;
	BYTE packetBuffer[mac_DATA_QUEUE_MAX_LENGTH][mac_PACKET_BUFFER_SIZE];
	WORD packetLength[mac_DATA_QUEUE_MAX_LENGTH];
	BYTE packetPriority[mac_DATA_QUEUE_MAX_LENGTH];
	BYTE packetInformationDataRate[mac_DATA_QUEUE_MAX_LENGTH];
	BYTE packetAckPolicy[mac_DATA_QUEUE_MAX_LENGTH];
}DataQueue;

/*********************************************************************
*下面是WbanMAC.c的函数声明
**********************************************************************/
void MACInit();
BYTE MACGet();
BOOL TransmitIt();
WBAN_PRIMITIVE MACTasks(WBAN_PRIMITIVE inputPrimitive);
WBAN_PRIMITIVE MACFrameTransmission( BYTE frameTypeSubtype, BYTE txType, BYTE* TxBuffer, WORD length );
WBAN_PRIMITIVE DataTypeFrameTxCSMA();
WBAN_PRIMITIVE MngCtrlTypeFrameTxDirect( BYTE frameTypeSubtype, BYTE* TxBuffer, WORD length );
WBAN_PRIMITIVE MngCtrlTypeFrameTxPROB( BYTE frameTypeSubType, BYTE* TxBuffer, WORD length );
WBAN_PRIMITIVE DataTypeFrameTxSchedule();
WBAN_PRIMITIVE MACMngCtrlFrameReception( BYTE frameSubtype );
WBAN_PRIMITIVE MACDataFrameReception();
WBAN_PRIMITIVE MACIAckFrameReception( BYTE lastFrameSubtype );
BYTE getBackoffCounter();
double getCP(BYTE type);
void Delay(WORD usecond);
//WBAN_PRIMITIVE dataTransmitAlohaProcess();
WBAN_PRIMITIVE packetWaitingAckProcess();
BYTE getCurrentSlotNumber();
//WBAN_PRIMITIVE mngTransmitProcess();
//BOOL mngTransmitIt();
BYTE Encrypt(WORD len, BYTE* buf);
BYTE Decrypt(WORD len, BYTE* buf);
WORD do_crc(BYTE *message, WORD len);
void CRC_16_CCITT(BYTE *data, WORD length);
BOOL CRC_16_CCITT_check(BYTE *data, WORD length);
#ifdef I_AM_HUB
BYTE getIndexFromNodeList( ABBR_ADDR abbrAddr );
void EnNodeToNodeList( ABBR_ADDR abbrAddr, FULL_ADDR fullAddr, CAPABILITY capability );
void DeNodeFromNodeList( ABBR_ADDR abbrAddr );
ABBR_ADDR GetAbbrAddr();
WBAN_PRIMITIVE packetWaitingConnectionRequestProcess();
WBAN_PRIMITIVE beaconTransmitProcess();
BYTE AllocationSlot( ABBR_ADDR abbrAddr, BYTE AllocationSlot);
#endif

#ifdef I_AM_NODE
BYTE GetAllocationLength( uint8_t DataArrivalRateIndicator );
#endif
/*********************************************************************
*对数据循环队列的一些操作
**********************************************************************/
void InitDataQueue();
BOOL EnDataQueue(BYTE _bufferTemp[], WORD _bufferTempLength, BYTE _bufferTempPriority, BYTE _bufferTempInformationDataRate, BYTE _bufferTempAckPolicy);
BOOL DeDataQueue();
BYTE* GetDataQueueHead();
WORD GetDataQueuePacketLength();
BYTE GetDataQueuePacketPriority();
BYTE GetDataQueuePacketInformationDataRate();
BYTE GetDataQueuePacketAckPolicy();
WORD GetDataQueueLength();

#endif
