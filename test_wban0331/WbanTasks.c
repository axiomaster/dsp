/*********************************************************************
 *
 *                  WbanTasks C File
 *
 *********************************************************************
 * FileName:        WbanTasks.c
 * Company:         Xidian University
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Yueyang Song       2013.10.16
 *Yueyang Song		 2013.11.15		v1.2
 *Yueyang Song 		 2014.11.22		v2.0
 *Yanle Lei			 2014.11.22		v2.0
 ********************************************************************/
#include "WbanTasks.h"
#include "WbanMAC.h"
#include "WbanSecurity.h"

#include "registers.h"
#include "hardware.h"
#include "FPGAtx.h"
#include<stdlib.h>

#include "WbanPHY.h"
#include <stdio.h>

BYTE mac_SPIBuffer_Rx[mac_SPI_BUFFER_SIZE] = {0};
BYTE mac_SPIBuffer_Tx[mac_SPI_BUFFER_SIZE] = {0};
SPIBuffer_ControlInfo mac_spiRxInfo;
SPIBuffer_ControlInfo mac_spiTxInfo;
SPIBuffer_ControlInfo mac_primitiveInfo;
PARAMS params;
WORD mac_j, mac__msdulen = 0;
TICK mac_t;
extern BYTE mac_TxBuffer[];
extern CURRENT_PACKET mac_currentPacket;
BYTE isSend = 0;	//������

/**********************************************************************
**����ԭ��:		void PrimitiveInit()
**��ڲ���:		void
**��   ��   ֵ��		void
**˵           ����		��ʼ����ǰ��ԭ�������Ϣ�����ݶ�����Ϣ��ԭ�������Ϣ�ͺ�̨�����¼�
***********************************************************************/
void PrimitiveInit()
{
#ifdef DEBUG
	printf("\n��ʼ��ԭ����Ʒ���Ϣ... @PrimitiveInit() @%ld\n", TickGet().Val);
#endif
	//��ʼ��SPIBuffer_Rx_ControlInfo
	mac_spiRxInfo.p = mac_SPIBuffer_Rx;
	mac_spiRxInfo.primitive = NO_PRIMITIVE;
	mac_spiRxInfo.length.v = 0xFFFF;

	//��ʼ��SPIBuffer_Tx_ControlInfo
	mac_spiTxInfo.p = mac_SPIBuffer_Tx;
	mac_spiTxInfo.primitive = NO_PRIMITIVE;
	mac_spiTxInfo.length.v = 0x0000;

	mac_primitiveInfo.p = NULL;
	mac_primitiveInfo.primitive = NO_PRIMITIVE;
	mac_primitiveInfo.length.v = 0x0000;
	//��ʼ��MAC_BACKGROUND_TASKS_PENDING��̨�����¼�
	mac_TasksPending.v = 0;
	//��ʼ�����ݶ���
	InitDataQueue();
}

/**********************************************************************
**����ԭ��:		void PrimitiveTasks()
**��ڲ���:		void
**��   �� 	ֵ��		void
**˵      	����		ϵͳ��ѭ�������ݲ�ͬԭ����¼������ȼ���������������
***********************************************************************/
void PrimitiveTasks(){
	uint64_t slottemp;

	while(1){
		printf("no primitive @%ld\n", TickGet().Val);
		slottemp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);

		//�жϵ�ǰʱ�̾��뵱ǰ��֡�ṹ��ʼʱ�̴���ʱ�䣬�Ƿ���MAP�����ʱ�����ڣ����ڣ���ʹ��־λ��1
		mac_t.Val = TickGet().Val - macBIB.BeaconStartTime.Val;
		//���ȷ��Ͷ�ʱ��
		if( (mac_t.Val >= macBIB.IntervalStart.Val) && (mac_t.Val < macBIB.IntervalEnd.Val) )
			mac_TasksPending.bit.intervalTimerInMAP = 1;
		else
			mac_TasksPending.bit.intervalTimerInMAP = 0;

#ifdef I_AM_HUB
		//BEACON���ڷ��Ͷ�ʱ��
		if( (mac_t.Val > 0) && (mac_t.Val < macBIB.beaconPeriodLength * slottemp) )
			mac_TasksPending.bit.beaconSendTimer = 0;
		else{
			mac_TasksPending.bit.beaconSendTimer = 1;
		}
#endif
		mac_t.Val = TickGet().Val - macBIB.BeaconStartTime.Val;

 		if(mac_primitiveInfo.primitive == NO_PRIMITIVE){
 			if(mac_TasksPending.bit.ackSending == 1){
 			 	mac_TasksPending.bit.ackSending = 0;
 			 	PutToNWK();
 			}
 			else if(mac_TasksPending.bit.frameWaitingAck == 1){
 				mac_TasksPending.bit.frameWaitingAck = 0;
 				mac_primitiveInfo.primitive = TheSecondSendingProcess();
 			}
 			else if(mac_TasksPending.bit.dataIndication == 1){	//��PHY�����ݵ���ʱ����Ҫ��־���¼�Ϊ1�������ڴ�ѭ������ѯ,�¼�����
 				mac_primitiveInfo.primitive = dataIndicationProcess();
 			}
#ifdef I_AM_HUB
 			else if(mac_TasksPending.bit.beaconSendTimer == 1){
 				mac_TasksPending.bit.beaconSendTimer = 0;
 				mac_primitiveInfo.primitive = beaconTransmitProcess();
 			}
#endif
 			else if(mac_TasksPending.bit.dataSending == 1){
 				mac_primitiveInfo.primitive = MACFrameTransmission( mac_DATA_FRAME, mac_CSMACA, 0, 0 );
 			}
 			else if(mac_TasksPending.bit.packetWaitingAck == 1){
 			 	mac_primitiveInfo.primitive = packetWaitingAckProcess();
 			}
#ifdef I_AM_HUB
 			else if(mac_TasksPending.bit.packetWaitingConnectionRequest == 1){
 			 	mac_primitiveInfo.primitive = packetWaitingConnectionRequestProcess();
 			}
#endif
			else if(mac_TasksPending.bit.dataInBuffer == 1){
				if( mac_TasksPending.bit.intervalTimerInMAP == 1 )
					mac_primitiveInfo.primitive = MACFrameTransmission( mac_DATA_FRAME, mac_SCHEDULE, 0, 0 );
				else{
					if((mac_t.Val > 0 && mac_t.Val < macBIB.RAP1End.Val ) || (mac_t.Val > macBIB.EAP2Start.Val && mac_t.Val < macBIB.beaconPeriodLength * slottemp) )
						mac_primitiveInfo.primitive = MACFrameTransmission( mac_DATA_FRAME, mac_CSMACA, 0, 0 );
				}
			}
			else{
				//��߲㷢��SPI��ȡ����

				//�ȴ�SPI���ݵ���

				//��SPI����д��mac_SPIBuffer_Rx

				//����ԭ��
				//if( spiflash_cycletest() == TRUE ){	//
				if( isSend == 1 ){	//
					//spiflash_cyclerx(mac_SPIBuffer_Rx);	//����spi���ݺ���

					/*int k;
					for(k=0; k<503; k++)
					{
						printf("%d ", mac_SPIBuffer_Rx[k]);
					}
					printf("\n");*/

					mac_primitiveInfo.primitive = primitiveInBufferProcess(&mac_spiRxInfo);
				}
			}
		}

		switch(mac_primitiveInfo.primitive){
		case NO_PRIMITIVE:
			break;
		case PL_DATA_request:
			//CRCУ��
			CRC_16_CCITT(params.PL_DATA_request.psdu, params.PL_DATA_request.psduLength);

			if( macBIB.isEncrypt == TRUE && mac_currentPacket.retry == 0 ){
				switch( mac_currentPacket.frameType_Subtype ){
				case mac_T_POLL_FRAME:
				case mac_I_ACK_FRAME:
				case mac_BEACON_FRAME:
				case mac_POLL_FRAME:
					//����Ҫ����
					break;
				default:
					params.PL_DATA_request.psduLength = Encrypt(params.PL_DATA_request.psduLength, params.PL_DATA_request.psdu);
				}
			}
			mac_primitiveInfo.primitive = PHYTasks(mac_primitiveInfo.primitive);
			break;
		case PL_DATA_indication:
			if( macBIB.isEncrypt == TRUE && params.PL_DATA_indication.psduLength % 16 == 0){
				params.PL_DATA_indication.psduLength = Decrypt(params.PL_DATA_indication.psduLength, params.PL_DATA_indication.psdu);
			}
		case MLME_RESET_request:
		case MLME_SCAN_request:
		case MLME_START_request:
		case HME_MLME_CONNECT_request:
		case HME_MLME_CONNECT_response:
		case NME_MLME_CONNECT_request:
			mac_primitiveInfo.primitive = MACTasks(mac_primitiveInfo.primitive);
			break;
		case MACS_DATA_indication:
		case MACS_DATA_confirm:
		case MLME_START_confirm:
		case MLME_RESET_confirm:
		case MLME_SCAN_confirm:
		case MLME_BEACON_NOTIFY_indication:
		case NME_MLME_CONNECT_indication:
		case HME_MLME_CONNECT_indication:
		case NME_MLME_CONNECT_confirm:
		case HME_MLME_CONNECT_confirm:
		case MLME_DISCONNECT_indication:
		case MLME_DISCONNECT_confirm:
			mac_primitiveInfo.primitive = MACPrimitiveTransmission( mac_primitiveInfo.primitive );
			break;

		case NME_MLME_SECURITY_confirm:
		case HME_MLME_SECURITY_indication:
		case HME_MLME_SECURITY_confirm:
			mac_primitiveInfo.primitive = SecurityPrimitiveTransmission( mac_primitiveInfo.primitive );
			break;

		case HME_MLME_SECURITY_response:
		case NME_MLME_SECURITY_request:
			mac_primitiveInfo.primitive = SecurityTasks(mac_primitiveInfo.primitive);
			break;

		default:
#ifdef DEBUG
	printf("PrimitiveTasks() can't find this case\n");
#endif
			;
		}
	}
}

/**********************************************************************
**����ԭ��:		BOOL PutToNWK()
**��ڲ���:		void
**��   �� 	ֵ��		BOOL
**˵      	����		��ԭ��SPIBuffer_Tx[]�е���������������
***********************************************************************/
BOOL PutToNWK(){
	mac_SPIBuffer_Tx[0] = mac_SPIBuffer_Tx[0] + 0xf0;//��Ϊ����SPI���յĵ�һ���ֽ���ò���0x00��������Ϊ���ղ�������
#ifdef DEBUG1
	printf("\n׼����߲㴫��ԭ�@PutToNWK() @%ld\n", TickGet().Val);
	for( mac_j = 0; mac_j < (mac_spiTxInfo.length.v + 2); mac_j++ )
		printf( "0x%x ", mac_SPIBuffer_Tx[mac_j] );
	printf( "\n֡��Ϊ��%d\n", mac_spiTxInfo.length.v );
#endif

	spiflash_cycletx(mac_SPIBuffer_Tx, (int)mac_spiTxInfo.length.v + 2);

	mac_spiTxInfo.p = mac_SPIBuffer_Tx;
	mac_spiTxInfo.primitive = NO_PRIMITIVE;
	mac_spiTxInfo.length.v = 0x0000;
	return TRUE;
}

/**********************************************************************
**����ԭ��:		WBAN_PRIMITIVE MACPrimitiveTransmission( WBAN_PRIMITIVE inputPrimitive )
**��ڲ���:		WBAN_PRIMITIVE inputPrimitive
**�� �� ֵ��	WBAN_PRIMITIVE
**˵    ����	��params�еĲ���д��mac_SPIBuffer_Tx�У�����ԭ���ʽ����߲㴫��
***********************************************************************/
WBAN_PRIMITIVE MACPrimitiveTransmission( WBAN_PRIMITIVE inputPrimitive )
{
	BYTE _length = 0;
	mac_spiTxInfo.primitive = inputPrimitive;

	switch(inputPrimitive){
	case MACS_DATA_indication:
		if(params.MACS_DATA_indication.fragmentNumber == 0){
			mac_spiTxInfo.p++;
			mac_spiTxInfo.p++;
			*mac_spiTxInfo.p++ = MACS_DATA_indication;

			*mac_spiTxInfo.p++ = params.MACS_DATA_indication.SenderID.v;
			*mac_spiTxInfo.p++ = params.MACS_DATA_indication.RecipientID.v;
			*mac_spiTxInfo.p++ = params.MACS_DATA_indication.BANID.v;
			*mac_spiTxInfo.p++ = params.MACS_DATA_indication.RxDSN;
			*mac_spiTxInfo.p++ = params.MACS_DATA_indication.frameTypeSubtype;
			*mac_spiTxInfo.p++ = params.MACS_DATA_indication.securityLevel;
			*mac_spiTxInfo.p++;
			*mac_spiTxInfo.p++;
			mac__msdulen = mac__msdulen + params.MACS_DATA_indication.msduLength;
			mac_spiTxInfo.length.v = mac_spiTxInfo.length.v + 9;

			_length = params.MACS_DATA_indication.msduLength;
			for(mac_j = 0; mac_j < _length; mac_j++){
				*mac_spiTxInfo.p++ = MACGet();
				mac_spiTxInfo.length.v++;
			}
		}
		else{
			mac__msdulen = mac__msdulen + params.MACS_DATA_indication.msduLength;
			_length = params.MACS_DATA_indication.msduLength;
			for(mac_j = 0; mac_j < _length; mac_j++){
				*mac_spiTxInfo.p++ = MACGet();
				mac_spiTxInfo.length.v++;
			}
		}

		if(params.MACS_DATA_indication.NonfinalFragment == 0){
			mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START] = mac_spiTxInfo.length.bytes.MSB;
			mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START + 1] = mac_spiTxInfo.length.bytes.LSB;
			mac_SPIBuffer_Tx[mac_PRIMITIVE_MSDULENGTH_START] = (BYTE)((mac__msdulen >> 8) & 0x00FF);
			mac_SPIBuffer_Tx[mac_PRIMITIVE_MSDULENGTH_START + 1] = (BYTE)(mac__msdulen & 0x00FF);
#ifdef DEBUG
			printf("\nMACS_DATA_indication������� ����ԭ�� @MACPrimitiveTransmission(...) @%ld\n", TickGet().Val);
			printf("SenderID = 0x%x \n", params.MACS_DATA_indication.SenderID.v);
			printf("RecipientID = 0x%x \n", params.MACS_DATA_indication.RecipientID.v);
			printf("BANID = 0x%x \n", params.MACS_DATA_indication.BANID.v);
			printf("UserPriority = 0x%x \n", params.MACS_DATA_indication.frameTypeSubtype & 0x0F);
			printf("frameType = 0x%x \n", params.MACS_DATA_indication.frameTypeSubtype & 0xF0);
			printf("msduLength = %d \n", mac__msdulen);
#endif
			mac__msdulen = 0;

			if(mac_TasksPending.bit.ackSending == 0)
				PutToNWK();
			else
				return MACFrameTransmission( mac_I_ACK_FRAME, mac_DIRECT, mac_TxBuffer, 9 );
		}
		else{
			return NO_PRIMITIVE;
		}

		break;

	case MLME_DISCONNECT_indication:
		mac_spiTxInfo.p++;	//�ճ�length��λ��
		mac_spiTxInfo.p++;
		*mac_spiTxInfo.p++ = MLME_DISCONNECT_indication;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_DISCONNECT_indication.SenderID.v;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_DISCONNECT_indication.BANID.v;
		mac_spiTxInfo.length.v++;

		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START] = mac_spiTxInfo.length.bytes.MSB;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START + 1] = mac_spiTxInfo.length.bytes.LSB;
#ifdef DEBUG
		printf("\nMLME_DISCONNECT_indication�������, SenderID = %d\n", params.MLME_DISCONNECT_indication.SenderID.v);
		printf("\n BANID = %d\n", params.MLME_DISCONNECT_indication.BANID.v);
#endif
		return MACFrameTransmission( mac_I_ACK_FRAME, mac_DIRECT, mac_TxBuffer, 9 );

	case MLME_DISCONNECT_confirm:
		mac_spiTxInfo.p++;	//�ճ�length��λ��
		mac_spiTxInfo.p++;
		*mac_spiTxInfo.p++ = MLME_DISCONNECT_confirm;
		mac_spiTxInfo.length.v++;

		*mac_spiTxInfo.p++ = params.MLME_DISCONNECT_confirm.AssocAbbrID.v;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_DISCONNECT_confirm.BANID.v;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_DISCONNECT_confirm.status;
		mac_spiTxInfo.length.v++;

		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START] = mac_spiTxInfo.length.bytes.MSB;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START + 1] = mac_spiTxInfo.length.bytes.LSB;
#ifdef DEBUG
		printf("\nMLME_DISCONNECT_confirm�������, status = %d\n", params.MLME_DISCONNECT_confirm.status);
#endif
		PutToNWK();
		break;

#ifdef I_AM_HUB
	case MLME_START_confirm:
		mac_spiTxInfo.p++;	//�ճ�length��λ��
		mac_spiTxInfo.p++;
		*mac_spiTxInfo.p++ = mac_spiTxInfo.primitive;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_START_confirm.status;
		mac_spiTxInfo.length.v++;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START] = mac_spiTxInfo.length.bytes.MSB;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START + 1] = mac_spiTxInfo.length.bytes.LSB;
#ifdef DEBUG
		printf("\nMLME_START_confirm�������, status = %d\n", params.MLME_START_confirm.status);
#endif
		return MACFrameTransmission( mac_BEACON_FRAME, mac_DIRECT, mac_TxBuffer, 25 );

	case HME_MLME_CONNECT_indication:
		mac_spiTxInfo.p++;	//�ճ�length��λ��
		mac_spiTxInfo.p++;
		*mac_spiTxInfo.p++ = HME_MLME_CONNECT_indication;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.HME_MLME_CONNECT_indication.NodeAddress.v[0];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.HME_MLME_CONNECT_indication.NodeAddress.v[1];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.HME_MLME_CONNECT_indication.NodeAddress.v[2];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.HME_MLME_CONNECT_indication.NodeAddress.v[3];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.HME_MLME_CONNECT_indication.NodeAddress.v[4];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.HME_MLME_CONNECT_indication.NodeAddress.v[5];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.HME_MLME_CONNECT_indication.AssocAbbrID.v;;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.HME_MLME_CONNECT_indication.BANID.v;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.HME_MLME_CONNECT_indication.CapabilityInformation;
		mac_spiTxInfo.length.v++;

		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START] = mac_spiTxInfo.length.bytes.MSB;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START + 1] = mac_spiTxInfo.length.bytes.LSB;
#ifdef DEBUG
		printf("\nHME_MLME_CONNECT_indication�������");
		printf("NodeAddress : 0x%x ", params.HME_MLME_CONNECT_indication.NodeAddress.v[0]);
		printf("0x%x ", params.HME_MLME_CONNECT_indication.NodeAddress.v[1]);
		printf("0x%x ", params.HME_MLME_CONNECT_indication.NodeAddress.v[2]);
		printf("0x%x ", params.HME_MLME_CONNECT_indication.NodeAddress.v[3]);
		printf("0x%x ", params.HME_MLME_CONNECT_indication.NodeAddress.v[4]);
		printf("0x%x \n", params.HME_MLME_CONNECT_indication.NodeAddress.v[5]);
		printf("BANID = %d\n", params.HME_MLME_CONNECT_indication.BANID.v);
		printf("CapabilityInformation = %d\n", params.HME_MLME_CONNECT_indication.CapabilityInformation);
#endif
		return MACFrameTransmission( mac_I_ACK_FRAME, mac_DIRECT, mac_TxBuffer, 9 );

	case HME_MLME_CONNECT_confirm:
		mac_spiTxInfo.p++;	//�ճ�length��λ��
		mac_spiTxInfo.p++;
		*mac_spiTxInfo.p++ = HME_MLME_CONNECT_confirm;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.HME_MLME_CONNECT_confirm.ConnectedNID.v;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.HME_MLME_CONNECT_confirm.status;
		mac_spiTxInfo.length.v++;

		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START] = mac_spiTxInfo.length.bytes.MSB;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START + 1] = mac_spiTxInfo.length.bytes.LSB;
#ifdef DEBUG
		printf("\nHME_MLME_CONNECT_confirm�������, status = %d\n", params.HME_MLME_CONNECT_confirm.status);
		printf("\nHME_MLME_CONNECT_confirm�������, ConnectedNID = %d\n", params.HME_MLME_CONNECT_confirm.ConnectedNID.v);
#endif
		PutToNWK();
		break;
#endif

#ifdef I_AM_NODE
	case MLME_BEACON_NOTIFY_indication:
		mac_spiTxInfo.p++;	//�ճ�length��λ��
		mac_spiTxInfo.p++;
		*mac_spiTxInfo.p++ = mac_spiTxInfo.primitive;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.BSN;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.BANID.v;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.SenderID.v;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.hubAddress.v[0];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.hubAddress.v[1];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.hubAddress.v[2];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.hubAddress.v[3];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.hubAddress.v[4];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.hubAddress.v[5];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.beaconPeriodLength;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.allocationSlotLength;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.RAP1Start;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.RAP1End;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.RAP2Start;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.RAP2End;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.MACCapability[0];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.MACCapability[1];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.MACCapability[2];
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_BEACON_NOTIFY_indication.PHYCapability;
		mac_spiTxInfo.length.v++;

		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START] = mac_spiTxInfo.length.bytes.MSB;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START + 1] = mac_spiTxInfo.length.bytes.LSB;
#ifdef DEBUG
		printf("\nMLME_BEACON_NOTIFY_indication����spiTxBuffer���\n");
		printf("BSN : 0x%x ", params.MLME_BEACON_NOTIFY_indication.BSN);
		printf("SenderID : 0x%x ", params.MLME_BEACON_NOTIFY_indication.SenderID.v);
		printf("BANID : 0x%x\n", params.MLME_BEACON_NOTIFY_indication.BANID.v);
		printf("hubAddress(��������) : 0x%x ", params.MLME_BEACON_NOTIFY_indication.hubAddress.v[0]);
		printf("0x%x ", params.MLME_BEACON_NOTIFY_indication.hubAddress.v[1]);
		printf("0x%x ", params.MLME_BEACON_NOTIFY_indication.hubAddress.v[2]);
		printf("0x%x ", params.MLME_BEACON_NOTIFY_indication.hubAddress.v[3]);
		printf("0x%x ", params.MLME_BEACON_NOTIFY_indication.hubAddress.v[4]);
		printf("0x%x \n", params.MLME_BEACON_NOTIFY_indication.hubAddress.v[5]);
		printf("beaconPeriodLength : %d \n", params.MLME_BEACON_NOTIFY_indication.beaconPeriodLength);
		printf("allocationSlotLength : %d \n", params.MLME_BEACON_NOTIFY_indication.allocationSlotLength);
		printf("RAP1Start : %d \n", params.MLME_BEACON_NOTIFY_indication.RAP1Start);
		printf("RAP1End : %d \n", params.MLME_BEACON_NOTIFY_indication.RAP1End);
		printf("RAP2Start : %d \n", params.MLME_BEACON_NOTIFY_indication.RAP2Start);
		printf("RAP2End : %d \n", params.MLME_BEACON_NOTIFY_indication.RAP2End);
		printf("MACCapability : 0x%x ", params.MLME_BEACON_NOTIFY_indication.MACCapability[0]);
		printf("0x%x ", params.MLME_BEACON_NOTIFY_indication.MACCapability[1]);
		printf("0x%x \n", params.MLME_BEACON_NOTIFY_indication.MACCapability[2]);
		printf("PHYCapability : 0x%x\n", params.MLME_BEACON_NOTIFY_indication.PHYCapability);
#endif
		PutToNWK();
		break;

	case NME_MLME_CONNECT_indication:
		mac_spiTxInfo.p++;	//�ճ�length��λ��
		mac_spiTxInfo.p++;
		*mac_spiTxInfo.p++ = NME_MLME_CONNECT_indication;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.NME_MLME_CONNECT_indication.SenderID.v;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.NME_MLME_CONNECT_indication.BANID.v;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.NME_MLME_CONNECT_indication.SecurityLevel;
		mac_spiTxInfo.length.v++;

		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START] = mac_spiTxInfo.length.bytes.MSB;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START + 1] = mac_spiTxInfo.length.bytes.LSB;
#ifdef DEBUG
		printf("\nNME_MLME_CONNECT_indication�������, SenderID = %d\n", params.NME_MLME_CONNECT_indication.SenderID.v);
		printf("BANID = %d\n", params.NME_MLME_CONNECT_indication.BANID.v);
#endif
		PutToNWK();
		break;

	case NME_MLME_CONNECT_confirm:
		mac_spiTxInfo.p++;	//�ճ�length��λ��
		mac_spiTxInfo.p++;
		*mac_spiTxInfo.p++ = NME_MLME_CONNECT_confirm;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.NME_MLME_CONNECT_confirm.AssocAbbrID.v;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.NME_MLME_CONNECT_confirm.status;
		mac_spiTxInfo.length.v++;

		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START] = mac_spiTxInfo.length.bytes.MSB;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START + 1] = mac_spiTxInfo.length.bytes.LSB;
#ifdef DEBUG
		printf("\nNME_MLME_CONNECT_confirm�������,status = %d\n", params.NME_MLME_CONNECT_confirm.ConnectionStatus);
#endif
		if(mac_TasksPending.bit.ackSending == 0)
			PutToNWK();
		else
			return MACFrameTransmission( mac_I_ACK_FRAME, mac_DIRECT, mac_TxBuffer, 9 );
		break;
#endif

	case MACS_DATA_confirm:
		mac_spiTxInfo.p++;	//�ճ�length��λ��
		mac_spiTxInfo.p++;
		*mac_spiTxInfo.p++ = mac_spiTxInfo.primitive;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MACS_DATA_confirm.RecipientID.v;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MACS_DATA_confirm.TxDSN;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MACS_DATA_confirm.status;
		mac_spiTxInfo.length.v++;

		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START] = mac_spiTxInfo.length.bytes.MSB;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START + 1] = mac_spiTxInfo.length.bytes.LSB;
#ifdef DEBUG
		printf( "\nMACS_DATA_confirm������� @MACPrimitiveTransmission \n");
		printf( "RecientID = 0x%x\n", params.MACS_DATA_confirm.RecipientID.v );
		printf( "status = 0x%x\n", params.MACS_DATA_confirm.status );
		printf( "TxDSN = 0x%x\n", params.MACS_DATA_confirm.TxDSN );
#endif
		PutToNWK();
		break;

	case MLME_SCAN_confirm:
		mac_spiTxInfo.p++;	//�ճ�length��λ��
		mac_spiTxInfo.p++;
		*mac_spiTxInfo.p++ = mac_spiTxInfo.primitive;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_SCAN_confirm.Status;
		mac_spiTxInfo.length.v++;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START] = mac_spiTxInfo.length.bytes.MSB;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START + 1] = mac_spiTxInfo.length.bytes.LSB;
#ifdef DEBUG
		printf("\nMLME_SCAN_confirm�������, Status = 0x%x  @ParamsSetPrimitive(...)\n", params.MLME_SCAN_confirm.Status);
#endif
		PutToNWK();
		break;

	case MLME_RESET_confirm:
		mac_spiTxInfo.p++;	//�ճ�length��λ��
		mac_spiTxInfo.p++;
		*mac_spiTxInfo.p++ = mac_spiTxInfo.primitive;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = params.MLME_RESET_confirm.status;
		mac_spiTxInfo.length.v++;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START] = mac_spiTxInfo.length.bytes.MSB;
		mac_SPIBuffer_Tx[mac_PRIMITIVE_LENGTH_START + 1] = mac_spiTxInfo.length.bytes.LSB;
#ifdef DEBUG
		printf("\nMLME_RESET_confirm�������, status = 0x%x  @ParamsSetPrimitive(...)\n", params.MLME_RESET_confirm.status);
#endif
		/**mac_spiTxInfo.p++ = 0xf0;
		*mac_spiTxInfo.p++ = 0xf1;
		*mac_spiTxInfo.p++ = 0xf2;
		mac_spiTxInfo.length.v++;
		*mac_spiTxInfo.p++ = 0xf3;
		mac_spiTxInfo.length.v++;*/

		PutToNWK();
		break;

	default:
#ifdef DEBUG
		printf("PrimitiveParamsSet() can't find this case\n");
#endif
		;
	}
	return NO_PRIMITIVE;
}

#ifdef I_AM_HUB
/**********************************************************************
**����ԭ��:		void beaconSendTimer()
**��ڲ���:		void
**��   �� 	ֵ��		void
**˵      	����		����DSP���ڲ���ʱ��CpuTimer1����ʱBeacon֡�ķ������ڣ���ʱ�䵽ʱ�����жϣ������¼�Ϊ1���ж�
***********************************************************************/
void beaconSendTimer(){
	mac_TasksPending.bit.beaconSendTimer = 1;
}
#endif

/**********************************************************************
**����ԭ��:		WBAN_PRIMITIVE primitiveInBufferProcess(SPIBuffer_ControlInfo* info)
**��ڲ���:		SPIBuffer_ControlInfo* info     ������֮ǰһ��Ҫ���ú�info�ĵ�ַָ��p
**��   �� 	ֵ��		WBAN_PRIMITIVE
**˵      	����		����ԭ������е�����ԭ����ݲ�ͬ��ԭ�����ͣ�����params
***********************************************************************/
WBAN_PRIMITIVE primitiveInBufferProcess(SPIBuffer_ControlInfo* info){
#ifdef DEBUG
	printf("\n����ԭ�￪ʼ  @primitiveInBufferProcess() @%ld\n", TickGet().Val);
#endif
	//���س���
	info->length.bytes.MSB = PrimitiveGet(info);
	info->length.bytes.LSB = PrimitiveGet(info);

#ifdef DEBUG1
	printf( "\nԭ�ﵽ�� --> \n" );
	for( mac_j = 0; mac_j < (info->length.v + 2); mac_j++ )
		printf( "0x%x ", mac_SPIBuffer_Rx[mac_j] );
	printf("\nԭ�ﳤ�ȣ�%d\n", info->length.v);
#endif

	//primitive����1byte
	info->primitive = (WBAN_PRIMITIVE)PrimitiveGet(info);

	switch(info->primitive){
	case NO_PRIMITIVE:
		break;

	case MACS_DATA_request:
		//����
		params.MACS_DATA_request.RecipientID.v = PrimitiveGet(info);
		params.MACS_DATA_request.BANID.v = PrimitiveGet(info);
		params.MACS_DATA_request.TxDSN = PrimitiveGet(info);
		params.MACS_DATA_request.ackPolicy = PrimitiveGet(info);
		params.MACS_DATA_request.frameTypeSubtype = PrimitiveGet(info);
		params.MACS_DATA_request.informationDataRate = PrimitiveGet(info);
		params.MACS_DATA_request.securityLevel = PrimitiveGet(info);
		params.MACS_DATA_request.msduLength = ((WORD)(PrimitiveGet(info)) << 8);
		params.MACS_DATA_request.msduLength |= PrimitiveGet(info);
		params.MACS_DATA_request.fragmentNumber = 0;
		params.MACS_DATA_request.non_finalFragment = 0;
#ifdef DEBUG
		printf( "ԭ�����ͣ�MACS_DATA_request\n" );
		printf( "RecipientID: 0x%x\n", params.MACS_DATA_request.RecipientID.v );
		printf( "BANID: 0x%x\n", params.MACS_DATA_request.BANID.v );
		printf( "TxDSN: 0x%x\n", params.MACS_DATA_request.TxDSN );
		printf( "AckPolicy: 0x%x\n", params.MACS_DATA_request.ackPolicy );
		printf( "FrameType: 0x%x\n", (params.MACS_DATA_request.frameTypeSubtype & 0xF0) );
		printf( "UserPriority: 0x%x\n", (params.MACS_DATA_request.frameTypeSubtype & 0x0F) );
		printf( "InformationDataRate: 0x%x\n", params.MACS_DATA_request.informationDataRate );
		printf( "SecurityLevel: 0x%x\n", params.MACS_DATA_request.securityLevel );
		printf( "MsduLength: %d\n", params.MACS_DATA_request.msduLength );
#endif

		while(info->length.v != 0){
			if(info->length.v > mac_FRAME_PAYLOAD_MAX_LENGTH){
				for(mac_j = mac_PACKET_DATA_START; mac_j < (mac_PACKET_DATA_START + mac_FRAME_PAYLOAD_MAX_LENGTH); mac_j++){
					mac_bufferTemp[mac_j] = PrimitiveGet(info);
				}
				mac_bufferTempLength = mac_FRAME_PAYLOAD_MAX_LENGTH;
				if( params.MACS_DATA_request.non_finalFragment == 1 )
					params.MACS_DATA_request.fragmentNumber++;
				params.MACS_DATA_request.non_finalFragment = 1;
#ifdef DEBUG
				printf("\n��SPIԭ���е����ݽ��з�Ƭ����ǰfragmentNumber��%d\n", params.MACS_DATA_request.fragmentNumber);
#endif
				if(params.MACS_DATA_request.fragmentNumber == 8){
					//�������е����˵��msdu�ĳ��ȴ���255 * 8 = 2040�� ����Ӧ��ֹͣ���п�����Ҫ���ϲ�˵�����
#ifdef DEBUG
					printf("**ERROR: Msdulength > %d @PrimitiveGetParams()\n", (mac_SPI_BUFFER_SIZE - 12));
#endif
					break;
				}
			}else{
				WORD _length = info->length.v;
				for(mac_j = mac_PACKET_DATA_START; mac_j < (mac_PACKET_DATA_START + _length); mac_j++){
					mac_bufferTemp[mac_j] = PrimitiveGet(info);
				}
				mac_bufferTempLength = _length;
				if(params.MACS_DATA_request.fragmentNumber != 0){
					params.MACS_DATA_request.fragmentNumber++;
#ifdef DEBUG
					printf("\n��SPIԭ���е����ݽ��з�Ƭ����ǰfragmentNumber��%d\n", params.MACS_DATA_request.fragmentNumber);
#endif
				}
				params.MACS_DATA_request.non_finalFragment = 0;
			}
			MACTasks(MACS_DATA_request);
		}
		break;

	case MLME_RESET_request:
		params.MLME_RESET_request.setDefaultBIB = PrimitiveGet(info);
#ifdef DEBUG
		printf("MLME_RESET_request.setDefaultBIB = 0x%x\n", params.MLME_RESET_request.setDefaultBIB);
#endif
		break;

	case MLME_SCAN_request:
		params.MLME_SCAN_request.ScanType = PrimitiveGet(info);
		params.MLME_SCAN_request.ChannelFreBand = PrimitiveGet(info);
		params.MLME_SCAN_request.ChannelNum = PrimitiveGet(info);
#ifdef DEBUG
		printf("ScanType = 0x%x\n", params.MLME_SCAN_request.ScanType);
		printf("ChannelFreBand = 0x%x\n", params.MLME_SCAN_request.ChannelFreBand);
		printf("ChannelNum = 0x%x\n", params.MLME_SCAN_request.ChannelNum);
#endif
		break;

	case MLME_DISCONNECT_request:
		params.MLME_DISCONNECT_request.BANID.v = PrimitiveGet(info);
		params.MLME_DISCONNECT_request.AssocAbbrID.v = PrimitiveGet(info);
#ifdef DEBUG
		printf("  ԭ�����ͣ�MLME_DISCONNECT_request\n");
		printf("  MLME_DISCONNECT_request.BANID.v: %d\n", params.MLME_DISCONNECT_request.BANID.v);
		printf("  MLME_DISCONNECT_request.AssocAbbrID.v: %d\n", params.MLME_DISCONNECT_request.AssocAbbrID.v);
#endif
		break;

#ifdef I_AM_HUB
	case MLME_START_request:
		params.MLME_START_request.BANID.v = PrimitiveGet(info);
		params.MLME_START_request.isDefault = PrimitiveGet(info);
		params.MLME_START_request.beaconPeriodLength = PrimitiveGet(info);
		params.MLME_START_request.allocationSlotLength = PrimitiveGet(info);
		params.MLME_START_request.RAP1Start = PrimitiveGet(info);
		params.MLME_START_request.RAP1End = PrimitiveGet(info);
		params.MLME_START_request.RAP2Start = PrimitiveGet(info);
		params.MLME_START_request.RAP2End = PrimitiveGet(info);
		params.MLME_START_request.securityLevel = PrimitiveGet(info);

#ifdef DEBUG
		printf("BANID = 0x%x\n", params.MLME_START_request.BANID.v);
		printf("isDefault = 0x%x\n", params.MLME_START_request.isDefault);
		printf("beaconPeriodLength = 0x%x\n", params.MLME_START_request.beaconPeriodLength);
		printf("allocationSlotLength = 0x%x\n", params.MLME_START_request.allocationSlotLength);
		printf("RAP1Start = 0x%x\n", params.MLME_START_request.RAP1Start);
		printf("RAP1End = 0x%x\n", params.MLME_START_request.RAP1End);
		printf("RAP2Start = 0x%x\n", params.MLME_START_request.RAP2Start);
		printf("RAP2End = 0x%x\n", params.MLME_START_request.RAP2End);
		printf("SecurityLevel = 0x%x\n", params.MLME_START_request.securityLevel);

#endif
		break;

	case HME_MLME_CONNECT_request:
		params.HME_MLME_CONNECT_request.BANID.v = PrimitiveGet(info);
		params.HME_MLME_CONNECT_request.RecipientID.v = PrimitiveGet(info);
	//	params.HME_MLME_CONNECT_request.AccessIndicator = PrimitiveGet(info);
		params.HME_MLME_CONNECT_request.CapabilityInformation = PrimitiveGet(info);
		params.HME_MLME_CONNECT_request.SecurityLevel = PrimitiveGet(info);
#ifdef DEBUG
		printf("ԭ�����ͣ�HME_MLME_CONNECT_request\n");
		printf("BANID: 0x%x\n", params.HME_MLME_CONNECT_request.BANID.v);
		printf("RecipientID: 0x%x\n", params.HME_MLME_CONNECT_request.RecipientID.v);
	//	printf("AccessIndicator: 0x%x\n", params.HME_MLME_CONNECT_request.AccessIndicator);
		printf("CapabilityInformation: 0x%x\n", params.HME_MLME_CONNECT_request.CapabilityInformation);
		printf("SecurityLevel: 0x%x\n", params.HME_MLME_CONNECT_request.SecurityLevel);
#endif
		break;

	case HME_MLME_CONNECT_response:
		params.HME_MLME_CONNECT_response.AssocAbbrID.v = PrimitiveGet(info);
		params.HME_MLME_CONNECT_response.ConnectionStatus = PrimitiveGet(info);
		params.HME_MLME_CONNECT_response.SecurityLevel = PrimitiveGet(info);

#ifdef DEBUG
		printf("ԭ�����ͣ�HME_MLME_CONNECT_response\n");
		printf("AssocAbbrID.v: 0x%x\n", params.HME_MLME_CONNECT_response.AssocAbbrID.v);
		printf("status: 0x%x\n", params.HME_MLME_CONNECT_response.ConnectionStatus);
		printf("SecurityLevel: 0x%x\n", params.HME_MLME_CONNECT_response.SecurityLevel);
#endif
		break;

	case HME_MLME_SECURITY_response:
		params.HME_MLME_SECURITY_response.NodeAddress.v[0] = PrimitiveGet(info);
		params.HME_MLME_SECURITY_response.NodeAddress.v[1] = PrimitiveGet(info);
		params.HME_MLME_SECURITY_response.NodeAddress.v[2] = PrimitiveGet(info);
		params.HME_MLME_SECURITY_response.NodeAddress.v[3] = PrimitiveGet(info);
		params.HME_MLME_SECURITY_response.NodeAddress.v[4] = PrimitiveGet(info);
		params.HME_MLME_SECURITY_response.NodeAddress.v[5] = PrimitiveGet(info);
		params.HME_MLME_SECURITY_response.AssociationStatus = PrimitiveGet(info);
		params.HME_MLME_SECURITY_response.PTKCreationStatus = PrimitiveGet(info);
		break;
#endif

#ifdef I_AM_NODE
	case NME_MLME_CONNECT_request:    
		params.NME_MLME_CONNECT_request.BANID.v = PrimitiveGet(info);
		params.NME_MLME_CONNECT_request.HubID.v = PrimitiveGet(info);
		params.NME_MLME_CONNECT_request.CapabilityInformation = PrimitiveGet(info);
		params.NME_MLME_CONNECT_request.SecurityLevel = PrimitiveGet(info);
		params.NME_MLME_CONNECT_request.DataArrivalRateIndicator = PrimitiveGet(info);
		params.NME_MLME_CONNECT_request.FrameSubtype = PrimitiveGet(info);
		params.NME_MLME_CONNECT_request.UserPriority = PrimitiveGet(info);
#ifdef DEBUG
        printf("  ����macBIB");		
#endif 		
        macBIB.hubAddr.v = params.NME_MLME_CONNECT_request.HubID.v;
        macBIB.BANID.v = params.NME_MLME_CONNECT_request.BANID.v;
        macBIB.SecurityLevel = params.NME_MLME_CONNECT_request.SecurityLevel;
        macBIB.DataArrivalRateIndicator = params.NME_MLME_CONNECT_request.DataArrivalRateIndicator;
#ifdef DEBUG
		printf("ԭ�����ͣ�NME_MLME_CONNECT_request\n");
		printf("BANID.v: %d\n", params.NME_MLME_CONNECT_request.BANID.v);
		printf("HubID: %d\n", params.NME_MLME_CONNECT_request.HubID.v);
		printf("CapabilityInformation: %d\n", params.NME_MLME_CONNECT_request.CapabilityInformation);
		printf("SecurityLevel: %d\n", params.NME_MLME_CONNECT_request.SecurityLevel);
		printf("DataArrivalRateIndicator: %d\n", params.NME_MLME_CONNECT_request.DataArrivalRateIndicator);
		printf("FrameSubtype: %d\n", params.NME_MLME_CONNECT_request.FrameSubtype);
		printf("UserPriority: %d\n", params.NME_MLME_CONNECT_request.UserPriority);
#endif
		break;

	case NME_MLME_SECURITY_request:
		params.NME_MLME_SECURITY_request.RecipientAddress.v[0] = PrimitiveGet(info);
		params.NME_MLME_SECURITY_request.RecipientAddress.v[1] = PrimitiveGet(info);
		params.NME_MLME_SECURITY_request.RecipientAddress.v[2] = PrimitiveGet(info);
		params.NME_MLME_SECURITY_request.RecipientAddress.v[3] = PrimitiveGet(info);
		params.NME_MLME_SECURITY_request.RecipientAddress.v[4] = PrimitiveGet(info);
		params.NME_MLME_SECURITY_request.RecipientAddress.v[5] = PrimitiveGet(info);
		params.NME_MLME_SECURITY_request.SecurityLevel = PrimitiveGet(info);
		params.NME_MLME_SECURITY_request.AssociationStatus = PrimitiveGet(info);
		params.NME_MLME_SECURITY_request.PTKCreationStatus = PrimitiveGet(info);
		break;
#endif

	default:
#ifdef DEBUG
	printf("primitiveInBufferProcess() can't find this case\n");
#endif
	;
	}

	WBAN_PRIMITIVE temp_primitive = NO_PRIMITIVE;
	if( info->primitive != MACS_DATA_request )
		temp_primitive = info->primitive;

	info->length.v = 0xFFFF;
	info->p = mac_SPIBuffer_Rx;
	info->primitive = NO_PRIMITIVE;

	return temp_primitive;
}

/**********************************************************************
**����ԭ��:		BYTE PrimitiveGet(SPIBuffer_ControlInfo* info)
**��ڲ���:		SPIBuffer_ControlInfo* info
**��   �� 	ֵ��		BYTE
**˵      	����		��ȡԭ���ֽڣ�ֻ������SPIBuffer_Rx_ControlInfo��Primitive_ControlInfo
***********************************************************************/
BYTE PrimitiveGet(SPIBuffer_ControlInfo* info){
	if(info->length.v > 0 && info->length.v < mac_SPI_BUFFER_SIZE){
		info->length.v--;
		return *info->p++;
	}
	else{
		if(info->length.v == 0)
			info->length.v = 0xFFFF;
		return *info->p++;
	}
}
