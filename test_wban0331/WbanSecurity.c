#include "WbanSecurity.h"
#include "AES_CBC.h"
#include "WbanTasks.h"
#include "WbanMAC.h"
#include "Wban.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>

extern BYTE mac_TxBuffer[];
extern BYTE mac_SPIBuffer_Tx[];
extern WORD mac_packetHeader, mac_packetFCS;
extern MAC_FRAME_CONTROL mac_frameControl;
WORD sec_s;

BYTE Nonce_I[16],Nonce_R[16];
BYTE plaintext1[46];
BYTE plaintext2[46];
BYTE *p1=plaintext1;
BYTE *p2=plaintext2;
BYTE KCK[16];
BYTE P[16];
BYTE PTK_KMAC_2B[8];
BYTE PTK_KMAC_3B[8];
BYTE PTK_KMAC_2A[8];
BYTE PTK_KMAC_3A[8];
BYTE PTK_Control[2];

BYTE messageNumber=0;  //����PTK�����ж�,�趨һ��ȫ�ֱ���
BYTE MK[4*4] =	{0x5B,0xE,0x5B,0xE,
	             0x5B,0xE,0x5B,0xE,
	             0x5B,0xE,0x5B,0xE,
	             0x5B,0xE,0x5B,0xE};
extern unsigned char **expandedkey;   //cbc������Ҫ
/**********************************************************************
**����ԭ��:		WBAN_PRIMITIVE SecurityIAckFrameReception( BYTE frameSubtype )
**��ڲ���:   	BYTE lastFrameSubtype
				����mac_currentPacket.frameSubtype
**�� �� ֵ��	WBAN_PRIMITIVE
**˵    ����	����MAC����յ���I-ACK֡���Ƕ԰�ȫ���ֵĹ������͵�֡�Ļظ�
***********************************************************************/
WBAN_PRIMITIVE SecurityIAckFrameReception( BYTE lastFrameSubtype )
{
#ifdef DEBUG
	printf( "@SecurityIAckFrameReception(...)" );
#endif

#ifdef I_AM_NODE
	if(lastFrameSubtype==mac_PTK_FRAME)
	{
		params.NME_MLME_SECURITY_confirm.SenderAddress.v[0]=macBIB.hubFullAddr.v[0];
		params.NME_MLME_SECURITY_confirm.SenderAddress.v[1]=macBIB.hubFullAddr.v[1];
		params.NME_MLME_SECURITY_confirm.SenderAddress.v[2]=macBIB.hubFullAddr.v[2];
		params.NME_MLME_SECURITY_confirm.SenderAddress.v[3]=macBIB.hubFullAddr.v[3];
		params.NME_MLME_SECURITY_confirm.SenderAddress.v[4]=macBIB.hubFullAddr.v[4];
		params.NME_MLME_SECURITY_confirm.SenderAddress.v[5]=macBIB.hubFullAddr.v[5];

		params.NME_MLME_SECURITY_confirm.Status=mac_SUCCESS;

		return NME_MLME_SECURITY_confirm;
	}
#endif

	return NO_PRIMITIVE;
}

/**********************************************************************
**����ԭ��:		WBAN_PRIMITIVE SecurityMngFrameReception( BYTE frameSubtype )
**��ڲ���:   	BYTE frameSubtype
**�� �� ֵ��	WBAN_PRIMITIVE
**˵    ����	����ȫ���ֽ��յ��Ĺ������͵�֡
***********************************************************************/
WBAN_PRIMITIVE SecurityMngFrameReception( BYTE frameSubtype )
{
	//BYTE PTK_KMAC[8];
#ifdef DEBUG
	printf( "@SecurityMngFrameReception(...)" );
#endif

#ifdef I_AM_HUB
	if (frameSubtype==mac_SECURITY_ASSOCIATION_FRAME)
	{
#ifdef DEBUG
		printf("�յ�����SecurityAssociation֡\n");
#endif
			  //����֡ͷ����macBIB
			  //macBIB.abbrAddr.v=params.MACS_DATA_indication.RecipientID.v;
			  //macBIB.hubAddr.v=params.MACS_DATA_indication.SenderID.v;
			  //macBIB.BANID.v=params.MACS_DATA_indication.BANID.v;    //����Ҫ����

	          //������ACK֡
		if(mac_frameControl.bits.AckPolicy==mac_I_ACK)
		{
			mac_packetHeader=mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++]=((macBIB.SecurityLevel<<3)|0x02);
			mac_TxBuffer[mac_packetHeader++]=0x10;
			mac_TxBuffer[mac_packetHeader++]=0x00;
			mac_TxBuffer[mac_packetHeader++]=0x00;

			mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;   //recipient address
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;   //������ַ
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

			mac_TxBuffer[mac_packetHeader++]=mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader++]=mac_FCS_MSB;

#ifdef DEBUG
			printf("�յ�����SecurityAssociationFrame������Ҫ�ظ�ACK��ACK�ѳ�֡\n");
#endif
		}
		mac_TasksPending.bit.ackSending=1;   //�ȴ�����ԭ�Ȼ����ACK

			/***********����ԭ��*********************/

		MACGet();     //��Ҫ�жϽ��յ�ַ�ǲ��Ǳ�����ַ
		MACGet();
		MACGet();
		MACGet();
		MACGet();
		MACGet();

		params.HME_MLME_SECURITY_indication.SenderAddress.v[0]=MACGet();
		params.HME_MLME_SECURITY_indication.SenderAddress.v[1]=MACGet();
		params.HME_MLME_SECURITY_indication.SenderAddress.v[2]=MACGet();
		params.HME_MLME_SECURITY_indication.SenderAddress.v[3]=MACGet();
		params.HME_MLME_SECURITY_indication.SenderAddress.v[4]=MACGet();  //HME_MLME_SECURITY_indicationֻ��Ҫ��߲㴫��һ��
		params.HME_MLME_SECURITY_indication.SenderAddress.v[5]=MACGet();  //senderAddress�Լ�SecurityLevel

		macBIB.nodeFullAddrTemp.v[0] = params.HME_MLME_SECURITY_indication.SenderAddress.v[0];
		macBIB.nodeFullAddrTemp.v[1] = params.HME_MLME_SECURITY_indication.SenderAddress.v[1];
		macBIB.nodeFullAddrTemp.v[2] = params.HME_MLME_SECURITY_indication.SenderAddress.v[2];
		macBIB.nodeFullAddrTemp.v[3] = params.HME_MLME_SECURITY_indication.SenderAddress.v[3];
		macBIB.nodeFullAddrTemp.v[4] = params.HME_MLME_SECURITY_indication.SenderAddress.v[4];
		macBIB.nodeFullAddrTemp.v[5] = params.HME_MLME_SECURITY_indication.SenderAddress.v[5];

		params.HME_MLME_SECURITY_indication.SecurityLevel |=MACGet()>>3;   //λ����


	/**********************����ԭ��**********************/
		return HME_MLME_SECURITY_indication;   //���ڴ�ѭ��������MACPrimitiveTransmission()����
	}  //hub���յ����ǰ�ȫ����֡�����


	if (frameSubtype==mac_PTK_FRAME)
	{
#ifdef DEBUG
		printf("�յ�����PTK_FRAME\n");
#endif
			  //����������ж��ǵڼ���PTK֡��
		messageNumber++;
	if(messageNumber==0x01)   //�����Ȳ����ж��ش�����
	{
	     if(mac_frameControl.bits.AckPolicy==mac_I_ACK)
		 {
			mac_packetHeader=mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++]=((macBIB.SecurityLevel<<3)|0x02);
			mac_TxBuffer[mac_packetHeader++]=0x10;   //����֡��Iack
			mac_TxBuffer[mac_packetHeader++]=0x00;
			mac_TxBuffer[mac_packetHeader++]=0x00;

			mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;   //recipient address
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;   //������ַ
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

			mac_TxBuffer[mac_packetHeader++]=mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader++]=mac_FCS_MSB;

#ifdef DEBUG
			printf("�յ����ǵ�һPTK_FRAME������Ҫ�ظ�ACK��ACK�ѳ�֡\n");
#endif
		}
		mac_TasksPending.bit.frameWaitingAck=1;    //�Ѵ��¼���Ϊ1Ȼ����ack֡

		//��֡������
				srand((unsigned)time(NULL));  //����time.h
				for(sec_s=0;sec_s<16;sec_s++)
				{
				Nonce_R[sec_s]=rand()%255;
				}
				MACGet();
				MACGet();
				MACGet();
				MACGet();
				MACGet();
				MACGet();
				MACGet();

				MACGet();
				MACGet();
				MACGet();
				MACGet();
				MACGet();
				MACGet();

				for(sec_s=0;sec_s<15;sec_s++)
				{
				 Nonce_I[sec_s]=MACGet();
				}

				//messageNumber=MACGet()&0x03;
				MACGet();
				MACGet();

				for(sec_s=0;sec_s<8;sec_s++)
				{
					MACGet();
				}

			//��plaintext1��ֵ
				for(sec_s=0;sec_s<6;sec_s++)
				{
				plaintext1[sec_s]=macBIB.fullAddr.v[sec_s];
				}
				for(sec_s=0;sec_s<6;sec_s++)
				{
				//plaintext1[sec_s+6]=macBIB.hubFullAddr.v[sec_s];
				plaintext1[sec_s+6]=macBIB.nodeFullAddrTemp.v[sec_s];
				}
				for(sec_s=0;sec_s<16;sec_s++)
				{
				plaintext1[sec_s+12]=Nonce_I[sec_s];
				}
				for(sec_s=0;sec_s<16;sec_s++)
				{
				plaintext1[sec_s+28]=Nonce_R[sec_s];
				}
				plaintext1[44]=0x02;   //�ڶ�PTK֡�е�PTK_Control��
				plaintext1[45]=0x00;

			//��plaintext2��ֵ
				for(sec_s=0;sec_s<6;sec_s++)
				{
				//plaintext1[sec_s]=macBIB.hubFullAddr.v[sec_s];
				plaintext1[sec_s]=macBIB.nodeFullAddrTemp.v[sec_s];
				}
				for(sec_s=0;sec_s<6;sec_s++)
				{
				plaintext1[sec_s+6]=macBIB.fullAddr.v[sec_s];
				}
				for(sec_s=0;sec_s<16;sec_s++)
				{
				plaintext1[sec_s+12]=Nonce_R[sec_s];
				}
				for(sec_s=0;sec_s<16;sec_s++)
				{
				plaintext1[sec_s+28]=Nonce_I[sec_s];
				}
				plaintext1[44]=PTK_Control[0];
				plaintext1[45]=PTK_Control[1];

			//����Ӧ�ļ��㴦��
				CBC_encrypt(p1,macBIB.PTK,MK,46);    //����PTK
				CBC_encrypt(p2,KCK,MK,46);
				CBC_encrypt(p1,P,KCK,46);
				memcpy(PTK_KMAC_2B,P,2*4*sizeof(char));
				memcpy(PTK_KMAC_3B,(char*)P+2*4,2*4*sizeof(char));

			/**************************�������*************************/



		return MACFrameTransmission(mac_I_ACK_FRAME,mac_DIRECT,mac_TxBuffer,9);


			//currentPacket.length = 47;
			//currentPacket.frameType = MANAGEMENT_FRAME;
			//currentPacket.frameSubtype = mac_PTK_FRAME;

			//if( macTasksPending.bit.frameWaitingAck==0)    �½�һ���¼��ĺ���
				//return MACFrameTransmission(mac_PTK_FRAME, mac_DIRECT,mac_TxBuffer,38);
			//else
				//return MACFrameTransmission(mac_I_ACK_FRAME,mac_DIRECT,mac_TxBuffer,9);

		}
		 else if(messageNumber==0x02)
		 {
		     // macBIB.abbrAddr.v=params.MACS_DATA_indication.RecipientID.v;
			  //macBIB.hubAddr.v=params.MACS_DATA_indication.SenderID.v;
			  //macBIB.BANID.v=params.MACS_DATA_indication.BANID.v;    //���±������Կ�
	          if(mac_frameControl.bits.AckPolicy==mac_I_ACK)
			  {
				  mac_packetHeader=mac_PACKET_HEADER_START;
				  mac_TxBuffer[mac_packetHeader++]=((macBIB.SecurityLevel<<3)|0x02);
				  mac_TxBuffer[mac_packetHeader++]=0x10;   //����֡��Iack
				  mac_TxBuffer[mac_packetHeader++]=0x00;
				  mac_TxBuffer[mac_packetHeader++]=0x00;

				  mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;   //recipient address
			      mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;   //������ַ
			      mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

				  mac_TxBuffer[mac_packetHeader++]=mac_FCS_LSB;
				  mac_TxBuffer[mac_packetHeader++]=mac_FCS_MSB;

	#ifdef DEBUG
				  printf("�յ����ǵ���PTK_FRAME������Ҫ�ظ�ACK��ACK�ѳ�֡\n");
	#endif

			  }
			   mac_TasksPending.bit.ackSending=1;   //����ԭ��

			  MACGet();
			  MACGet();
			  MACGet();
			  MACGet();
			  MACGet();
			  MACGet();

			 params.HME_MLME_SECURITY_confirm.SenderAddress.v[0]=MACGet();
			 params.HME_MLME_SECURITY_confirm.SenderAddress.v[1]=MACGet();
			 params.HME_MLME_SECURITY_confirm.SenderAddress.v[2]=MACGet();
			 params.HME_MLME_SECURITY_confirm.SenderAddress.v[3]=MACGet();
			 params.HME_MLME_SECURITY_confirm.SenderAddress.v[4]=MACGet();
			 params.HME_MLME_SECURITY_confirm.SenderAddress.v[5]=MACGet();

			 for(sec_s=0;sec_s<16;sec_s++)
			 {
			 MACGet();
			 }
			 if(MACGet()>>4==0x00)
				 params.HME_MLME_SECURITY_confirm.Status=0x01;
			 else
				 params.HME_MLME_SECURITY_confirm.Status=0x00;   //��ʱ���ж�������״̬

			 MACGet();

			 for(sec_s=0;sec_s<8;sec_s++)
			 {
			 PTK_KMAC_3A[sec_s]=MACGet();
			 }
	         if(memcpy(PTK_KMAC_3B,PTK_KMAC_3A,8)==0)

			   //���߲��ϴ�ԭ��
	             return HME_MLME_SECURITY_confirm;   //���ڴ�ѭ��������MACPrimitiveTransmission()����
			 else
				 return NO_PRIMITIVE;
		 }

		}


	#endif     //I_AM_HUB




	#ifdef I_AM_NODE
			  //�յ��ڶ�����֡��������ACK֡������Ҫ����ԭ�����ACK֡���齨�µĵ�һPTK֡
	if(frameSubtype==mac_SECURITY_ASSOCIATION_FRAME)
	{
	           #ifdef DEBUG
				  printf("�յ�����SECURITY_ASSOCIATION_FRAME\n");
	           #endif

				    //����֡ͷ����macBIB
			  //macBIB.abbrAddr.v=params.MACS_DATA_indication.RecipientID.v;
			  //macBIB.hubAddr.v=params.MACS_DATA_indication.SenderID.v;
			  //macBIB.BANID.v=params.MACS_DATA_indication.BANID.v;    //���±������Կ�

	          //������ACK֡
			  if(mac_frameControl.bits.AckPolicy==mac_I_ACK)
			  {
				  mac_packetHeader=mac_PACKET_HEADER_START;
				  mac_TxBuffer[mac_packetHeader++]=((macBIB.SecurityLevel<<3)|0x02);
				  mac_TxBuffer[mac_packetHeader++]=0x10;   //��ȫ����֡
				  mac_TxBuffer[mac_packetHeader++]=0x00;
				  mac_TxBuffer[mac_packetHeader++]=0x00;

				  mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;   //recipient address
			      mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;   //������ַ
			      mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

				  mac_TxBuffer[mac_packetHeader++]=mac_FCS_LSB;
				  mac_TxBuffer[mac_packetHeader++]=mac_FCS_MSB;

	#ifdef DEBUG
				  printf("�յ�����SecurityAssociationFrame������Ҫ�ظ�ACK��ACK�ѳ�֡\n");
	#endif

			  }
			   mac_TasksPending.bit.frameWaitingAck=1;   //�ȷ���ack�ٷ������齨��֡

			  MACGet();
			  MACGet();
			  MACGet();
			  MACGet();
			  MACGet();
			  MACGet();

			  macBIB.hubFullAddr.v[0]=MACGet();
			  macBIB.hubFullAddr.v[1]=MACGet();
			  macBIB.hubFullAddr.v[2]=MACGet();
			  macBIB.hubFullAddr.v[3]=MACGet();
			  macBIB.hubFullAddr.v[4]=MACGet();
			  macBIB.hubFullAddr.v[5]=MACGet();

			  macBIB.SecurityLevel |=MACGet()>>3;


		return MACFrameTransmission(mac_I_ACK_FRAME,mac_DIRECT,mac_TxBuffer,9);

	}
	if(frameSubtype==mac_PTK_FRAME)
	{
#ifdef DEBUG
		printf("�յ�����PTK_FRAME\n");
 #endif

			//  macBIB.abbrAddr.v=params.MACS_DATA_indication.RecipientID.v;
			 // macBIB.hubAddr.v=params.MACS_DATA_indication.SenderID.v;
			 // macBIB.BANID.v=params.MACS_DATA_indication.BANID.v;          //���±������Կ�
	          if(mac_frameControl.bits.AckPolicy==mac_I_ACK)
			  {
				  mac_packetHeader=mac_PACKET_HEADER_START;
				  mac_TxBuffer[mac_packetHeader++]=((macBIB.SecurityLevel<<3)|0x02);
				  mac_TxBuffer[mac_packetHeader++]=0x10;   //����֡��Iack
				  mac_TxBuffer[mac_packetHeader++]=0x00;
				  mac_TxBuffer[mac_packetHeader++]=0x00;

				  mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;   //recipient address
			      mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;   //������ַ
			      mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

				  //֡β
				  mac_TxBuffer[mac_packetHeader++]=0x12;
				  mac_TxBuffer[mac_packetHeader++]=0x12;

	#ifdef DEBUG
				  printf("�յ����ǵڶ�PTK_FRAME������Ҫ�ظ�ACK��ACK�ѳ�֡\n");
	#endif

			  }
			   mac_TasksPending.bit.frameWaitingAck=1;


			 MACGet();
			 MACGet();
			 MACGet();
			 MACGet();
			 MACGet();
			 MACGet();

			 MACGet();
			 MACGet();
			 MACGet();
			 MACGet();    //���͵�ַ���ڱ��ظ���
			 MACGet();
			 MACGet();

			 for(sec_s=0;sec_s<16;sec_s++)
			 {
			  	Nonce_R[sec_s]=MACGet();
			  }
			  	PTK_Control[0]=MACGet();
			  	PTK_Control[1]=MACGet();

			  for(sec_s=0;sec_s<8;sec_s++)
			  {
			  	PTK_KMAC_2B[sec_s]=MACGet();
			  }

			  		  //����Ӧ�ļ��㴦������PTK,PTK_KMAC_2...
			  		  //�ȶ�plaintext1��ֵ
			  	for(sec_s=0;sec_s<6;sec_s++)
			  		  {
			  			  plaintext1[sec_s]=macBIB.fullAddr.v[sec_s];
			  		  }
			  		  for(sec_s=0;sec_s<6;sec_s++)
			  		  {
			  			plaintext1[sec_s+6]=macBIB.hubFullAddr.v[sec_s];
			  		  }
			  		  for(sec_s=0;sec_s<16;sec_s++)
			  		  {
			  			plaintext1[sec_s+12]=Nonce_I[sec_s];  //���齨��һPTK֡���Ѿ�����������Nonce_I��
			  		  }
			  		  for(sec_s=0;sec_s<16;sec_s++)
			  		  {
			  			plaintext1[sec_s+28]=Nonce_R[sec_s];
			  		  }
			  			plaintext1[44]=PTK_Control[0];
			  			plaintext1[45]=PTK_Control[1];
			  		  //��plaintext2��ֵ
			  		  for(sec_s=0;sec_s<6;sec_s++)
			  		  {
			  			plaintext2[sec_s]=macBIB.hubFullAddr.v[sec_s];
			  		  }
			  		  for(sec_s=0;sec_s<6;sec_s++)
			  		  {
			  			plaintext2[sec_s+6]=macBIB.fullAddr.v[sec_s];
			  		  }
			  		  for(sec_s=0;sec_s<16;sec_s++)
			  		  {
			  			plaintext2[sec_s+12]=Nonce_R[sec_s];
			  		  }
			  		  for(sec_s=0;sec_s<16;sec_s++)
			  		  {
			  			plaintext2[sec_s+28]=Nonce_I[sec_s];
			  		  }
			  			plaintext2[44]=PTK_Control[0];
			  			plaintext2[45]=PTK_Control[1];

			  			//����Ӧ�ļ��㴦��
			  			CBC_encrypt(p1,macBIB.PTK,MK,46);    //����PTK
			  			CBC_encrypt(p2,KCK,MK,46);
			  			CBC_encrypt(p1,P,KCK,46);
			  			memcpy(PTK_KMAC_2A,P,2*4*sizeof(char));
			  			memcpy(PTK_KMAC_3A,(char*)P+2*4,2*4*sizeof(char));

		return MACFrameTransmission(mac_I_ACK_FRAME,mac_DIRECT,mac_TxBuffer,9);

		}
		//else
			//return NO_PRIMITIVE;
	//}

	#endif   //ifdef I_AM_NODE


	return NO_PRIMITIVE;
}

WBAN_PRIMITIVE SecurityTasks( WBAN_PRIMITIVE inputPrimitive )
{
#ifdef DEBUG
	printf( "@SecurityTasks(...)" );
#endif

	switch(inputPrimitive){
		//Ŀǰhub�Լ�node�ڵ��յ�ԭ���Ҫ��ԭ����д��param�У�Ȼ�������֡����֡������Ҫ��macBIB�д��붫��
#ifdef I_AM_NODE
	case NME_MLME_SECURITY_request:
		//��һ����֡ͷ��װsecurity association ֡���  pp. 17
#ifdef DEBUG
	    printf("��ʼװ���һ����֡...\n");
#endif
		mac_packetHeader = mac_PACKET_HEADER_START;
		mac_TxBuffer[mac_packetHeader++] = ((params.NME_MLME_SECURITY_request.SecurityLevel<<3)|0x02);    //01: Immediate acknowledgment  securityLevel�ɸ߲�ָ��
		mac_TxBuffer[mac_packetHeader++] = 0x02;   //frame subtype name:security association(0010)
		mac_TxBuffer[mac_packetHeader++] = 0x00;   //sequece number ��0
		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;  //recipient ID
		mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;  //??
		mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;  //BAN ID

		//Security Association payload��װ������Ҫ    pp. 31
		//TxBuffer[packetHeader++] = macBIB.fullAddr.v[0];
		//֡�����ߵ�ַ���޷�ȷ��������Ϊ0 ��
		mac_TxBuffer[mac_packetHeader++]=0;
		mac_TxBuffer[mac_packetHeader++]=0;
		mac_TxBuffer[mac_packetHeader++]=0;
		mac_TxBuffer[mac_packetHeader++]=0;
		mac_TxBuffer[mac_packetHeader++]=0;
		mac_TxBuffer[mac_packetHeader++]=0;
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];//�����߼�������ַ
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];
		mac_TxBuffer[mac_packetHeader++] |=params.NME_MLME_SECURITY_request.SecurityLevel<<3;   //��һλ��Ҫparam����
		mac_TxBuffer[mac_packetHeader++]=0x00;//security_suite_seletor (�ȵ�λ���λ)  0000000000000000������һ��
		mac_TxBuffer[mac_packetHeader++]=0x01;
		mac_TxBuffer[mac_packetHeader++]=0x00;  //reserved ����ȫ0��Association_Control

		macBIB.SecurityLevel=params.NME_MLME_SECURITY_request.SecurityLevel;  //���±���securityLevel

			//֡β��װ��У��ͣ���ȱ
		mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
		mac_TxBuffer[mac_packetHeader] = mac_FCS_LSB;   //�ݶ�Ϊ0x12

		return MACFrameTransmission( mac_SECURITY_ASSOCIATION_FRAME, mac_PROB, mac_TxBuffer,25);//mac_PROB����WbanMAC.h�ж���

	/*************************************************************************************/
#endif

#ifdef I_AM_HUB
	case HME_MLME_SECURITY_response:
#ifdef DEBUG
	    printf("��ʼװ��ڶ�����֡...\n");
#endif
		mac_packetHeader = mac_PACKET_HEADER_START;
		mac_TxBuffer[mac_packetHeader++] = ((macBIB.SecurityLevel<<3)|0x02);  //I_ACK
		mac_TxBuffer[mac_packetHeader++] = 0x02;
		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;   //recipient address
		mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
		mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

			//Security Association payload��װ������Ҫ    pp. 31
			//TxBuffer[packetHeader++] = macBIB.fullAddr.v[0];
			//֡�����ߵ�ַ���޷�ȷ��������Ϊ0 ��
		mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[0];    //��ַ�����⣬��ʱӦ���н��յ�ַ�ˣ�����nodeList
		mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[1];
		mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[2];
		mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[3];
		mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[4];
		mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[5];   // hub address
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];//�����߼�������ַ,hub����
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];
		mac_TxBuffer[mac_packetHeader] = macBIB.SecurityLevel << 3;
		mac_TxBuffer[mac_packetHeader++] |= 1 << 5;   //Control Frame Authentication
		mac_TxBuffer[mac_packetHeader++]=0x00;//security_suite_seletor
		mac_TxBuffer[mac_packetHeader++]=0x02;  //association sequence number ���͵��ǵڼ�֡���趨Ϊ��
		mac_TxBuffer[mac_packetHeader++]=0x00;//Association_Control
			//֡β��װ��У��ͣ���ȱ
		mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
		mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

		return MACFrameTransmission( mac_SECURITY_ASSOCIATION_FRAME, mac_DIRECT, mac_TxBuffer,25);//mac_PROB����WbanMAC.h�ж���
#endif
		}
	return NO_PRIMITIVE;
}

WBAN_PRIMITIVE SecurityPrimitiveTransmission( WBAN_PRIMITIVE inputPrimitive )
{
#ifdef DEBUG
	printf( "@SecurityPrimitiveTransmission(...)" );
#endif

	mac_spiTxInfo.primitive=inputPrimitive;

	switch(inputPrimitive){
#ifdef I_AM_HUB
	case HME_MLME_SECURITY_confirm:
		mac_spiTxInfo.length.v=8;
		*mac_spiTxInfo.p++=mac_spiTxInfo.length.bytes.MSB;    //�ȸ߰�λ����Ͱ�λ
		*mac_spiTxInfo.p++=mac_spiTxInfo.length.bytes.LSB;
		*mac_spiTxInfo.p++=HME_MLME_SECURITY_confirm;

		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_confirm.SenderAddress.v[0];
		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_confirm.SenderAddress.v[1];
		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_confirm.SenderAddress.v[2];
		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_confirm.SenderAddress.v[3];
		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_confirm.SenderAddress.v[4];
		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_confirm.SenderAddress.v[5];
		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_confirm.Status;

		return MACFrameTransmission(mac_I_ACK_FRAME,mac_DIRECT,mac_TxBuffer,9);

	case HME_MLME_SECURITY_indication:
		mac_spiTxInfo.length.v=8;
		*mac_spiTxInfo.p++=mac_spiTxInfo.length.bytes.MSB;    //�ȸ߰�λ����Ͱ�λ
		*mac_spiTxInfo.p++=mac_spiTxInfo.length.bytes.LSB;
		*mac_spiTxInfo.p++=HME_MLME_SECURITY_indication;

		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_indication.SenderAddress.v[0];
		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_indication.SenderAddress.v[1];
		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_indication.SenderAddress.v[2];
		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_indication.SenderAddress.v[3];
		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_indication.SenderAddress.v[4];
		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_indication.SenderAddress.v[5];
		*mac_spiTxInfo.p++=params.HME_MLME_SECURITY_indication.SecurityLevel;

		return MACFrameTransmission(mac_I_ACK_FRAME,mac_DIRECT,mac_TxBuffer,9);
#endif

#ifdef I_AM_NODE
	case NME_MLME_SECURITY_confirm:
		mac_spiTxInfo.length.v=8;
		*mac_spiTxInfo.p++=mac_spiTxInfo.length.bytes.MSB;    //�ȸ߰�λ����Ͱ�λ
		*mac_spiTxInfo.p++=mac_spiTxInfo.length.bytes.LSB;
		*mac_spiTxInfo.p++=NME_MLME_SECURITY_confirm;

		*mac_spiTxInfo.p++=params.NME_MLME_SECURITY_confirm.SenderAddress.v[0];
		*mac_spiTxInfo.p++=params.NME_MLME_SECURITY_confirm.SenderAddress.v[1];
		*mac_spiTxInfo.p++=params.NME_MLME_SECURITY_confirm.SenderAddress.v[2];
		*mac_spiTxInfo.p++=params.NME_MLME_SECURITY_confirm.SenderAddress.v[3];
		*mac_spiTxInfo.p++=params.NME_MLME_SECURITY_confirm.SenderAddress.v[4];
		*mac_spiTxInfo.p++=params.NME_MLME_SECURITY_confirm.SenderAddress.v[5];
		*mac_spiTxInfo.p++=params.NME_MLME_SECURITY_confirm.Status;

		PutToNWK();
		return NO_PRIMITIVE;
#endif
		}

	return NO_PRIMITIVE;
}

WBAN_PRIMITIVE TheSecondSendingProcess()
{
#ifdef I_AM_HUB
			//��ʼ�齨�ڶ�PTK֡
			mac_packetHeader = mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++] = ((macBIB.SecurityLevel<<3)|0x02|0x20);    //01: Immediate acknowledgment
			mac_TxBuffer[mac_packetHeader++] = 0x04;								//subtype: PTK_frame(0100)
			mac_TxBuffer[mac_packetHeader++] = 0x02;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;					 //recipient ID
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;					//sender ID
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;						//BAN ID

			//PTK payload �İ�װ
			mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[0];
			mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[1];
			mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[2];
			mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[3];
			mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[4];
			mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[5];                     // recipient address:hub address
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];//�����߼�������ַ
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];

		for(sec_s=0;sec_s<16;sec_s++)
		{
			mac_TxBuffer[mac_packetHeader++]=Nonce_R[sec_s];
		}
			mac_TxBuffer[mac_packetHeader++] =0x02;					//PTK Control
			mac_TxBuffer[mac_packetHeader++] =0x00;

		for(sec_s=0;sec_s<8;sec_s++)
		{
			mac_TxBuffer[mac_packetHeader++]=PTK_KMAC_2B[sec_s];
		}

			//֡β
			mac_TxBuffer[mac_packetHeader++]=mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader++]=mac_FCS_MSB;

			return MACFrameTransmission(mac_PTK_FRAME, mac_DIRECT,mac_TxBuffer,47);  //���͵ڶ�PTK֡
#endif


#ifdef I_AM_NODE
		if(messageNumber==0x00)
		{
			messageNumber++;
			//�齨֡ͷ
			mac_packetHeader = mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++] = ((macBIB.SecurityLevel<<3)|0x02|0x20);    //01: Immediate acknowledgment
			mac_TxBuffer[mac_packetHeader++] = 0x04;  //subtype: PTK_frame(0100)
			mac_TxBuffer[mac_packetHeader++] = 0x01;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;  //recipient ID
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;  //sender ID
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;  //BAN ID


			//PTK payload �İ�װ
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[0];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[1];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[2];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[3];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[4];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[5];   // recipient address:hub address
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];//�����߼�������ַ
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];

			srand((unsigned)time(NULL));
		for(sec_s=0;sec_s<16;sec_s++)
		{
			Nonce_I[sec_s]=rand()%255;
		}

		for(sec_s=0;sec_s<16;sec_s++)
		{
			mac_TxBuffer[mac_packetHeader++]=Nonce_I[sec_s];
		}

			mac_TxBuffer[mac_packetHeader++]= 0x01;  //PTK control 0x0001
			mac_TxBuffer[mac_packetHeader++]= 0x00;

		for(sec_s=0;sec_s<8;sec_s++)
		{
			mac_TxBuffer[mac_packetHeader++]=0x00;     //PTK_KMAC ��һ֡����Ϊ0
		}

		//֡β��װ
			mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

		return MACFrameTransmission( mac_PTK_FRAME, mac_DIRECT, mac_TxBuffer,47);
		}


		if(messageNumber==0x01)
	{
		if(memcmp(PTK_KMAC_2B,PTK_KMAC_2A,8)==0)
	      {
	     //�����ȣ��齨��һ֡
			mac_packetHeader = mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++] = ((macBIB.SecurityLevel<<3)|0x02|0x20);    //01: Immediate acknowledgment
			mac_TxBuffer[mac_packetHeader++] = 0x04;					 //subtype: PTK_frame(0100)
			mac_TxBuffer[mac_packetHeader++] = 0x03;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;		 //recipient ID
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;		 //sender ID
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;			 //BAN ID������ĵ�ַ�Ǽ�����ַ

			//PTK payload �İ�װ
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[0];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[1];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[2];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[3];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[4];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[5];							// recipient address:hub address
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];	//�����߼�������ַ
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];

		for(sec_s=0;sec_s<16;sec_s++)
		{
			mac_TxBuffer[mac_packetHeader++]=Nonce_I[sec_s];
		}
			mac_TxBuffer[mac_packetHeader++] =0x03;   //PTK Control
			mac_TxBuffer[mac_packetHeader++] =0x00;

		for(sec_s=0;sec_s<8;sec_s++)
		{
			mac_TxBuffer[mac_packetHeader++]=PTK_KMAC_3A[sec_s];
		}

		//֡β
			mac_TxBuffer[mac_packetHeader++]= mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader++]= mac_FCS_MSB;

		return MACFrameTransmission(mac_PTK_FRAME, mac_DIRECT,mac_TxBuffer,47);
		}
	}
#endif
		return NO_PRIMITIVE;
}
