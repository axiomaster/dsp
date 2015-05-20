/*********************************************************************
 *
 *                  WbanTasks Header File
 *
 *********************************************************************
 * FileName:        WbanTasks.h
 * Company:         Xidian University
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Yueyang Song       2013.10.02
 *Yueyang Song		 2013.11.15		v1.2
 *Yueyang Song		 2014.11.22		v2.0
 *Yanle Lei			 2014.11.22		v2.0
 ********************************************************************/
#ifndef _WbanTasks
#define _WbanTasks

#include "Wban.h"
/*********************************************************************
*MAC����߲㣬MAC����PHY���ԭ��������壬����Щԭ����ڹ������ڣ����Լ���
*�洢�ռ��ռ�ã���Ҫע����ǣ���Щԭ�����Щ�����ǹ��õģ����Բ�����λ��
*Ҫ����һ��
**********************************************************************/
typedef union _PARAMS{
	struct _MACS_DATA_request{
		WORD		msduLength;
		BYTE*		msdu;
		ABBR_ADDR	RecipientID;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID;
		BYTE		TxDSN;
		BYTE		ackPolicy;
		BYTE		frameTypeSubtype;
		BYTE		informationDataRate;
		BYTE 		securityLevel;
		//����ԭ��δ�ж��壬����������Ҫ����
		BYTE		fragmentNumber;
		BOOL        non_finalFragment;
	}MACS_DATA_request;
	
	struct _MACS_DATA_confirm{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		BYTE		TxDSN;
		BYTE		status;
	}MACS_DATA_confirm;
	
	struct _MACS_DATA_indication{
		WORD		msduLength;
		BYTE*		msdu;
		ABBR_ADDR	RecipientID;	//���λ�ò��ܱ� ��MLME_BEACON_NOTIFY_indicationһ��
		ABBR_ADDR	SenderID;		//���λ�ò��ܱ� ��MLME_BEACON_NOTIFY_indicationһ��
		ABBR_ADDR	BANID;			//���λ�ò��ܱ� ��MLME_BEACON_NOTIFY_indicationһ��
		BYTE		RxDSN;
		BYTE		fill[1];
		BYTE		frameTypeSubtype;
		BYTE 		fragmentNumber;
		BYTE		NonfinalFragment;
		BYTE		securityLevel;
	}MACS_DATA_indication;
	
	struct _MLME_RESET_request{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		BOOL	setDefaultBIB;
	}MLME_RESET_request;
	
	struct _MLME_RESET_confirm{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		BYTE	status;
	}MLME_RESET_confirm;

	struct _MLME_SCAN_request{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		BYTE		ScanType;
		BYTE		ChannelFreBand;
		BYTE		ChannelNum;
	}MLME_SCAN_request;

	struct _MLME_SCAN_confirm{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		BYTE		Status;
	}MLME_SCAN_confirm;

	struct _MLME_DISCONNECT_request{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR   BANID;
		ABBR_ADDR	AssocAbbrID;
	}MLME_DISCONNECT_request;

	struct _MLME_DISCONNECT_indication{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID;
		ABBR_ADDR	BANID;
		FULL_ADDR   fullAddr;
		FULL_ADDR   senderFullAddr;
		ABBR_ADDR	AssocAbbrID;
	}MLME_DISCONNECT_indication;

	struct _MLME_DISCONNECT_confirm{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR   BANID;
		ABBR_ADDR	AssocAbbrID;
		BYTE        status;
	}MLME_DISCONNECT_confirm;

	struct _PL_DATA_request{
		WORD		psduLength;
		BYTE*		psdu;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		BYTE		informationDataRate;
		BYTE 		burstMode;
		BYTE		scramblerSeed;
		BYTE		band;
		BYTE		channelNumber;
	}PL_DATA_request;

	struct _PL_DATA_confirm{         //��ʱû�õ�
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		BYTE		status;
	}PL_DATA_confirm;

	struct _PL_DATA_indication{
		WORD		psduLength;
		BYTE*		psdu;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
	}PL_DATA_indication;

	struct _PLME_CCA_request{        //��ʱû�õ�
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		BYTE		filler;
	}PLME_CCA_request;

	struct _PLME_CCA_confirm{        //��ʱû�õ�
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		BYTE		status;
	}PLME_CCA_confirm;

#ifdef I_AM_HUB
	struct _MLME_START_request{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID;
		BOOL isDefault;
		BYTE beaconPeriodLength;
		BYTE allocationSlotLength;
		BYTE RAP1End;
		BYTE RAP2Start;
		BYTE RAP2End;
		BYTE securityLevel;
		BYTE RAP1Start;
	}MLME_START_request;

	struct _MLME_START_confirm{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		BYTE status;
	}MLME_START_confirm;

	struct _HME_MLME_CONNECT_request{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID;
		BYTE        AccessIndicator;
	    BYTE 		CapabilityInformation;
	    BYTE        SecurityLevel;
	}HME_MLME_CONNECT_request;

	struct _HME_MLME_CONNECT_response{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		FULL_ADDR 	NodeAddress;
		ABBR_ADDR   AssocAbbrID;
		BYTE        ConnectionStatus;
		BYTE        SecurityLevel;
	}HME_MLME_CONNECT_response;

	struct _HME_MLME_CONNECT_indication{
		WORD 		psduLength_fill;
		BYTE* 		psdu_fill;
		ABBR_ADDR	RecipientID;
		ABBR_ADDR	SenderID;
		ABBR_ADDR	BANID;
		FULL_ADDR   HubAddress;
		FULL_ADDR   NodeAddress;
		ABBR_ADDR   AssocAbbrID;
		BYTE        ConnectionChangeIndicator;
		BYTE        RequestedAckDataRates;
		BYTE        RequestedWakeupPeriod[2];
		BYTE        RequestedWakeupPhase[2];
		BYTE        CapabilityInformation;
		BYTE        ElementID;
		BYTE        Length;
		BYTE        AllocationID;
		BYTE        MaximumGap;
		BYTE        MinimumGap;
		BYTE        MinimumLength;
		BYTE        AllocationLength;
		BYTE        ScalingFactor;
	}HME_MLME_CONNECT_indication;

	struct _HME_MLME_CONNECT_confirm{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		ABBR_ADDR   ConnectedNID;
		BYTE        status;
	}HME_MLME_CONNECT_confirm;
	
	struct _HME_MLME_SECURITY_indication{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		FULL_ADDR 	SenderAddress;
		BYTE        SecurityLevel;
	}HME_MLME_SECURITY_indication;
	
	struct _HME_MLME_SECURITY_response{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		FULL_ADDR 	NodeAddress;
		BYTE        AssociationStatus;
		BYTE        PTKCreationStatus;
	}HME_MLME_SECURITY_response;
	
	struct _HME_MLME_SECURITY_confirm{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		FULL_ADDR   SenderAddress;
		BYTE        Status;
	}HME_MLME_SECURITY_confirm;

#endif

#ifdef I_AM_NODE
	struct _MLME_BEACON_NOTIFY_indication{
		WORD psduLength_fill;
		BYTE* psdu_fill;
		ABBR_ADDR RecipientID_fill;	//���λ�ò��ܱ�  ��MACS_DATA_indicationһ��
		ABBR_ADDR SenderID;		//���λ�ò��ܱ�  ��MACS_DATA_indicationһ��
		ABBR_ADDR BANID;		//���λ�ò��ܱ�  ��MACS_DATA_indicationһ��
		FULL_ADDR hubAddress;
		BYTE BSN;
		BYTE beaconPeriodLength;
		BYTE allocationSlotLength;
		BYTE RAP1Start;
		BYTE RAP1End;
		BYTE RAP2Start;
		BYTE RAP2End;
		BYTE MACCapability[3];
		BYTE PHYCapability;
	}MLME_BEACON_NOTIFY_indication;

	struct _NME_MLME_CONNECT_request{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID;
		ABBR_ADDR	HubID;
		BYTE        CapabilityInformation;
		BYTE		SecurityLevel;
		BYTE        DataArrivalRateIndicator;
		BYTE        FrameSubtype;
		BYTE        UserPriority;
	}NME_MLME_CONNECT_request;

	struct _NME_MLME_CONNECT_indication{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID;
		ABBR_ADDR	BANID;
		BYTE		SecurityLevel;
		BYTE        SlotNumber;
		BYTE        SlotOffset[2];
		BYTE        SlotLength;
	}NME_MLME_CONNECT_indication;

	struct _NME_MLME_CONNECT_confirm{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		ABBR_ADDR	AssocAbbrID;
		FULL_ADDR   fullAddr;
		BYTE        status;
		BYTE        ConnectionStatus;
		BYTE        AccessIndicator;
		BYTE        CurrentSuperframeNumber;
		BYTE        ConnectionChangeIndicator;
		BYTE        AssignedAckDataRates;
		BYTE        MinmimCAPLength;
		BYTE        AssignedWakeupPhase[2];
		BYTE        AssignedWakeupPeriod[2];
		BYTE        ElementID;
		BYTE        Length;
		BYTE        AllocationID;
		BYTE        IntervalStart;
		BYTE        IntervalEnd;
	}NME_MLME_CONNECT_confirm;
	
	struct _NME_MLME_SECURITY_request{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		FULL_ADDR 	RecipientAddress;
		BYTE        SecurityLevel;
		BYTE        AssociationStatus;
		BYTE        PTKCreationStatus;
	}NME_MLME_SECURITY_request;
	
	struct _NME_MLME_SECURITY_confirm{
		WORD		msduLength_fill;
		BYTE*		msdu_fill;
		ABBR_ADDR	RecipientID_fill;
		ABBR_ADDR	SenderID_fill;
		ABBR_ADDR	BANID_fill;
		FULL_ADDR 	SenderAddress;
		BYTE        Status;
	}NME_MLME_SECURITY_confirm;
#endif
	
}PARAMS;
extern PARAMS params;

/*********************************************************************
*ԭ�����Ͷ��壬�����ڳ���������ͬ��ԭ�����ͣ�����ö�����Ͷ���
**********************************************************************/
typedef enum _WBAN_PRIMITIVE{
	NO_PRIMITIVE = 0,

	MACS_DATA_request 				= 0x01,
	MACS_DATA_confirm 				= 0x02,
	MACS_DATA_indication 			= 0x03,
	MLME_RESET_request 				= 0x04,
	MLME_RESET_confirm 				= 0x05,
	MLME_START_request				= 0x06,
	MLME_START_confirm				= 0x07,
	MLME_BEACON_NOTIFY_indication	= 0x08,
	NME_MLME_CONNECT_request		= 0x09,
	NME_MLME_CONNECT_confirm		= 0x0A,
	NME_MLME_CONNECT_indication		= 0x0B,
	HME_MLME_CONNECT_request		= 0x0C,
	HME_MLME_CONNECT_indication		= 0x0D,
	HME_MLME_CONNECT_response		= 0x0E,
	HME_MLME_CONNECT_confirm		= 0x0F,
	MLME_DISCONNECT_request			= 0x10,
	MLME_DISCONNECT_confirm			= 0x11,
	MLME_DISCONNECT_indication		= 0x12,
	
	NME_MLME_SECURITY_request       = 0x13,
	NME_MLME_SECURITY_confirm       = 0x14,
	HME_MLME_SECURITY_indication    = 0x15,
	HME_MLME_SECURITY_response      = 0x16,
	HME_MLME_SECURITY_confirm       = 0x17,
	MLME_SCAN_request				= 0x18,
	MLME_SCAN_confirm				= 0x19,

	PL_DATA_request 				= 0x30,
	PL_DATA_indication 				= 0x31,
	PLME_CCA_request				= 0x32,
	PLME_CCA_confirm				= 0x33

}WBAN_PRIMITIVE;

/*********************************************************************
*������һЩ��WbanTasks���õ��ĳ�������
**********************************************************************/
#define mac_PACKET_BUFFER_SIZE 				256		//���׼�����ϣ���Ϊ�˼��ܺ�Ϊ16�ı���
#define mac_FRAME_PAYLOAD_MAX_LENGTH 		(mac_PACKET_BUFFER_SIZE - 9)
#define mac_SPI_BUFFER_SIZE 				(mac_FRAME_PAYLOAD_MAX_LENGTH * 8 + 12)	//SPIԭ����󳤶�      11ΪMACS_DATA_request����msdu��������ܳ���
#define mac_DATA_QUEUE_MAX_LENGTH 			9	//8 + 1 ����1����Ϊѭ������Ҫ��Ȼ�Ӳ���ȥ�ڰ˸���Ƭ

//����ָʾBUFFER�е�֡ͷ��payload��FCS�Ŀ�ʼλ��
#define mac_PACKET_HEADER_START 0
#define mac_PACKET_DATA_START	7
#define mac_PACKET_FCS_START	(mac_PACKET_BUFFER_SIZE - 2)

#define mac_FCS_LSB 0x12
#define mac_FCS_MSB 0x12

#define mac_PRIMITIVE_LENGTH_START 0x00
#define mac_PRIMITIVE_MSDULENGTH_START 0x09

/*********************************************************************
*������SPIBuffer�Ŀ�����Ϣ�Ķ���
**********************************************************************/
typedef struct _SPIBuffer_ControlInfo{
	WBAN_PRIMITIVE primitive;
	BYTE* p;
	union _Length_R{
		WORD v;
		struct _Length_bytes_R{
			BYTE LSB;
			BYTE MSB;
		}bytes;
	}length;
}SPIBuffer_ControlInfo;
extern SPIBuffer_ControlInfo mac_spiTxInfo;

/*********************************************************************
*�Ժ�̨���е������¼��Ľṹ�嶨��
**********************************************************************/
typedef union _MAC_BACKGROUND_TASKS_PENDING{
	DWORD v;
	struct _background_bits{
		DWORD dataSending			:1;
		DWORD dataInBuffer			:1;
		DWORD packetWaitingAck		:1;
		DWORD dataIndication		:1;
		DWORD ackSending			:1;
		DWORD beaconSendTimer		:1;
		DWORD intervalTimerInMAP	:1;
		DWORD packetWaitingConnectionRequest  :1;
		DWORD frameWaitingAck		:1;
	}bit;
}MAC_TASKS_PENDING;

//�ⲿ����
extern BYTE mac_bufferTemp[mac_PACKET_BUFFER_SIZE];
extern WORD mac_bufferTempLength;
extern MAC_TASKS_PENDING mac_TasksPending;

/*********************************************************************
*������WbanTasks.c�ĺ�������
**********************************************************************/
void PrimitiveInit();
void PrimitiveTasks();

BOOL PutToNWK();
WBAN_PRIMITIVE MACPrimitiveTransmission( WBAN_PRIMITIVE inputPrimitive );
BYTE PrimitiveGet(SPIBuffer_ControlInfo* info);
WBAN_PRIMITIVE primitiveInBufferProcess(SPIBuffer_ControlInfo* info);
#ifdef I_AM_HUB
void beaconSendTimer();									//�����Ҫ��Ϊ�жϷ������
#endif

#endif
