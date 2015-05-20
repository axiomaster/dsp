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
*MAC֡ͷ��FrameControl�Ķ���
*�Լ������ֵ
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
*MAC��Payload��PHY/MAC Capability�Ķ���
*�Լ������ֵ
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
*HUB�е�Node��¼��
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
*HUB�е�Node������ַ��¼��
**********************************************************************/
typedef struct _ABBRADDR_LIST{
	BOOL isUsed;
}ABBRADDR_LIST;
#endif

#ifdef I_AM_HUB
/*********************************************************************
*HUB�е�UplinkRequestIE��¼��
**********************************************************************/
typedef struct _UplinkRequestIE{
	BYTE AllocationLength;
}UplinkRequestIE;
#endif
/*********************************************************************
*һЩ�����Ķ���
**********************************************************************/
//һЩĬ�ϵļ�����ַ
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
								//  ����maxFrameLength = 256, minDataRate = 91900, ackFrameLength = 9

//����status����Ϊ�Լ��Ķ���
#define mac_NONE	0x02							//����ָʾ֮ǰ��û�н��д���
#define mac_FAIL	0x03							//����ָʾ��һ֡�Ĵ���ʧ��
#define mac_IDLE	0x04							//����CCA���صĽ��
#define mac_BUSY	0x05							//����CCA���صĽ��
#define mac_ERROR	0xFF							//����ָʾ���ݶ����еĴ���

#define mac_SUCCESS							0x00	//��ָʾ�Ĳ�����ȫ�ɹ��������������У�ָʾConnection request frame�ɹ�����
#define mac_POLICY_RESTRICTION				0x01	//���ӱ��ܾ�-������hub�Ĺ�����/ӵ�����ƶ��ķ�����������
#define mac_FORMAT_ERROR					0x02	//���ӱ��ܾ�-��Ч�Ļ��߲�֧�ֵ�֡��ʽ
#define mac_UNSECURED_COMMUNICATION			0x03	//���ӱ��ܾ�-��hubû�н��а�ȫ����
#define mac_NO_CHANNEL						0x04	//���ӱ��ܾ�-û�ж���Ĵ���
#define mac_NO_CONNECTED_NID				0x05	//���ӱ��ܾ�-û�ж����Connected_NID
#define mac_NO_INTERNAL_RESOURCE			0x06	//���ӱ��ܾ�-û�ж�����ڲ���Դ
#define mac_UNSUPPORTED_SYNCHRONIZATION		0x07	//���ӱ��ܾ�-node�����ͬ�������������֧��
#define mac_UNSUPPORTED_CLOCK_PPM			0x08	//���ӱ��ܾ�-node��ʱ��PPM̫�󣬲�֧��
#define mac_UNSUPPORTED_BEACON_SHIFTING		0x09	//���ӱ��ܾ�-hub֧��BEACONƯ�ƣ���node��֧��
#define mac_UNSUPPORTED_CHANNEL_HOPPING		0x0A	//���ӱ��ܾ�-hub֧����Ƶ����node��֧��
//RESERVED					0x0A-0x0E	Ԥ��
#define mac_MODIFIED_ASSIGNMENT				0x10	//���ӷ���֡ Connection assignment frame ���޸�
//RESERVED					0x11-0x1F	Ԥ������������״ָ̬ʾ
#define mac_TRX_OFF							0x20	//�շ����ر�
#define mac_NO_ACK							0x21	//�ﵽ�����������ƺ�û���յ�ACK
#define mac_NO_DATA							0x22	//û���յ��κ����ݻ�Ӧ
#define mac_CHANNEL_ACCESS_FAILURE			0x23	//δ�ܽ����ŵ����磬csma����ʧ�ܣ�
#define mac_TRANSACTION_OVERFLOW			0x24	//û�ж���Ĵ洢�ռ�
#define mac_TRANSACTION_EXPIRED				0x25	//�����ѹ��ڣ����ݱ�ɾ��
#define mac_INVALID_ADDRESS					0x26	//���͵�ַ����յ�ַ��Ч
#define mac_INVALID_PARAMETER				0x27	//�����ԭ�������֧�ֻ��߳�����ȡֵ��Χ
//RESERVED					0x28-0x3F	Ԥ�����������ݴ���״ָ̬ʾ
#define mac_NO_MK							0x40	//���ӱ��ܾ�-ȱ����������Կ
#define mac_INVALID_SecuritySuitSelector	0x41	//�����֡��������
//RESERVED					0x42-0x5F	Ԥ�������ڰ�ȫ״ָ̬ʾ
//RESERVED					0x60-0xFF	Ԥ������������״ָ̬ʾ

#define mac_MAC_BEACON_PAYLOAD_LENGTH 16	//15 - 21
#define mac_DATA_HEAD_LENGTH 7
#define mac_DATA_FCS_LENGTH 2

#define HUB_ABBR_ADDR_DEFAULT 				0x02	//hub
#define mac_BANID_DEFAULT					0x02
#define mac_INIT_BEACON_PERIOD_LENGTH_DEFAULT	255		//1
#define mac_INIT_ALLOCATION_SLOT_LENGTH_DEFAULT	0x08//255
#define mac_BEACON_PERIOD_LENGTH_DEFAULT	255		//���ݼ���ɱ�
#define mac_ALLOCATION_SLOT_LENGTH_DEFAULT	255		//���ݼ���ɱ䣬Ӧ���ܹ���֤һ���ڵ����ȫ����
#define mac_RAP1_START_DEFAULT				1
#define mac_EarliestRAP1End                 0
#define mac_RAP1_END_DEFAULT				50
#define mac_EAP2_START_DEFAULT              130
#define mac_RAP2_START_DEFAULT				130
#define mac_RAP2_END_DEFAULT				180
#define mac_POLLPOSTWINDOW_DEFAULT			0x01   //ʹwindowʱ���ԼΪ37.5ms
#define mac_NEXT_DEFAULT					0x00
#define mac_Minimum_CAP_Length              0x00

#define mac_RETRY_LIMITATION				0x05	//�ش��������ƣ�Ŀǰ�ݶ�5

#define mac_BEACON_WITH_SUPERFRAME_CSMA			0x00
#define mac_BEACON_WITH_SUPERFRAME_ALOHA        0x01
#define mac_NONBEACON_WITH_SUPERFRAME           0x10
#define mac_NONBEACON_WITHOUT_SUPERFRAME	    0x11

//��ȫ�õ��Ĳ���
#define Nb 4
#define Nk 4
#define Nr 10
#define BUFFSIZE 128

//����� �������ӣ�������;���꣬Ĭ��Ϊ0
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
	BYTE macTxDSN;	//data or management SN �˸����ȼ�
	BYTE macRxDSN;
	ABBR_ADDR hubAddr;
	FULL_ADDR hubFullAddr;
	CAPABILITY hubCapability;
#endif

#ifdef I_AM_HUB
	BYTE macTxDSN[mac_mMaxBANSize];	//data or management SN ���64���ڵ�
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
	TICK IntervalStart;				//�������Ŀǰ�汾��Ϊ��֪����ʵ����Ӧ����assignment֡�����л�ȡ
	TICK IntervalEnd;				//�������Ŀǰ�汾��Ϊ��֪����ʵ����Ӧ����assignment֡�����л�ȡ
	TICK BeaconStartTime;
	BYTE EarliestRAP1End;
	TICK EAP2Start;
	BYTE allocationSlotLength;
	BYTE beaconPeriodLength;
	BYTE DataArrivalRateIndicator;

	CAPABILITY capability;
	BYTE AccessMode;	//ֻ������ģʽ
	BYTE PollPostWindow;
	BYTE MoreData;
	BYTE Next;
	BYTE RequestedWakeupPeriod;		//�������Ŀǰ�汾��Ϊ��֪����ʵ����Ӧ����assignment֡�����л�ȡ
	BYTE AccessIndicator;

	//PHY��
	BYTE band;
	BYTE channelNumber;
	BYTE scramblerSeed;
	BYTE informationDataRate;

	//��ȫ���ֵ�macBIB
	BYTE SecurityLevel;
	BYTE PTK[16];
	BYTE MK[16];
	BOOL isEncrypt;		//�Ƿ���Ҫ���ܵı�־λ
	BOOL isAuthenticated;
}MAC_BIB;
extern MAC_BIB macBIB;

typedef struct _Current_Packet{
	TICK startTime;
	WORD length;
	BYTE retry;
	BYTE lastPacketStatus;
	BYTE priority;
	BYTE ackPolicy;	//Ŀǰ���ڲ���Ҫ�ظ�ACK���������͵�֡���ϴ�ԭ�����
	ABBR_ADDR RecipientID; //Ŀǰ���ڲ���Ҫ�ظ�ACK���������͵�֡���ϴ�ԭ�����
	BYTE* buffer;
	BYTE informationDataRate;
	BYTE frameType_Subtype;
	BOOL non_finalFragment;
}CURRENT_PACKET;

/*********************************************************************
*������ѭ�����еĶ���
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
*������WbanMAC.c�ĺ�������
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
*������ѭ�����е�һЩ����
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
