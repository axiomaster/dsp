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

BYTE messageNumber=0;  //用于PTK计数判断,设定一个全局变量
BYTE MK[4*4] =	{0x5B,0xE,0x5B,0xE,
	             0x5B,0xE,0x5B,0xE,
	             0x5B,0xE,0x5B,0xE,
	             0x5B,0xE,0x5B,0xE};
extern unsigned char **expandedkey;   //cbc函数需要
/**********************************************************************
**函数原型:		WBAN_PRIMITIVE SecurityIAckFrameReception( BYTE frameSubtype )
**入口参数:   	BYTE lastFrameSubtype
				传入mac_currentPacket.frameSubtype
**返 回 值：	WBAN_PRIMITIVE
**说    明：	处理MAC层接收到的I-ACK帧，是对安全部分的管理类型的帧的回复
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
**函数原型:		WBAN_PRIMITIVE SecurityMngFrameReception( BYTE frameSubtype )
**入口参数:   	BYTE frameSubtype
**返 回 值：	WBAN_PRIMITIVE
**说    明：	处理安全部分接收到的管理类型的帧
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
		printf("收到的是SecurityAssociation帧\n");
#endif
			  //根据帧头配置macBIB
			  //macBIB.abbrAddr.v=params.MACS_DATA_indication.RecipientID.v;
			  //macBIB.hubAddr.v=params.MACS_DATA_indication.SenderID.v;
			  //macBIB.BANID.v=params.MACS_DATA_indication.BANID.v;    //不需要配置

	          //先生成ACK帧
		if(mac_frameControl.bits.AckPolicy==mac_I_ACK)
		{
			mac_packetHeader=mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++]=((macBIB.SecurityLevel<<3)|0x02);
			mac_TxBuffer[mac_packetHeader++]=0x10;
			mac_TxBuffer[mac_packetHeader++]=0x00;
			mac_TxBuffer[mac_packetHeader++]=0x00;

			mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;   //recipient address
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;   //简明地址
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

			mac_TxBuffer[mac_packetHeader++]=mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader++]=mac_FCS_MSB;

#ifdef DEBUG
			printf("收到的是SecurityAssociationFrame，且需要回复ACK，ACK已成帧\n");
#endif
		}
		mac_TasksPending.bit.ackSending=1;   //等待建立原语，然后发送ACK

			/***********生成原语*********************/

		MACGet();     //需要判断接收地址是不是本机地址
		MACGet();
		MACGet();
		MACGet();
		MACGet();
		MACGet();

		params.HME_MLME_SECURITY_indication.SenderAddress.v[0]=MACGet();
		params.HME_MLME_SECURITY_indication.SenderAddress.v[1]=MACGet();
		params.HME_MLME_SECURITY_indication.SenderAddress.v[2]=MACGet();
		params.HME_MLME_SECURITY_indication.SenderAddress.v[3]=MACGet();
		params.HME_MLME_SECURITY_indication.SenderAddress.v[4]=MACGet();  //HME_MLME_SECURITY_indication只需要向高层传递一个
		params.HME_MLME_SECURITY_indication.SenderAddress.v[5]=MACGet();  //senderAddress以及SecurityLevel

		macBIB.nodeFullAddrTemp.v[0] = params.HME_MLME_SECURITY_indication.SenderAddress.v[0];
		macBIB.nodeFullAddrTemp.v[1] = params.HME_MLME_SECURITY_indication.SenderAddress.v[1];
		macBIB.nodeFullAddrTemp.v[2] = params.HME_MLME_SECURITY_indication.SenderAddress.v[2];
		macBIB.nodeFullAddrTemp.v[3] = params.HME_MLME_SECURITY_indication.SenderAddress.v[3];
		macBIB.nodeFullAddrTemp.v[4] = params.HME_MLME_SECURITY_indication.SenderAddress.v[4];
		macBIB.nodeFullAddrTemp.v[5] = params.HME_MLME_SECURITY_indication.SenderAddress.v[5];

		params.HME_MLME_SECURITY_indication.SecurityLevel |=MACGet()>>3;   //位操作


	/**********************发送原语**********************/
		return HME_MLME_SECURITY_indication;   //会在大循环中启动MACPrimitiveTransmission()函数
	}  //hub端收到的是安全关联帧的情况


	if (frameSubtype==mac_PTK_FRAME)
	{
#ifdef DEBUG
		printf("收到的是PTK_FRAME\n");
#endif
			  //得在这就做判断是第几个PTK帧了
		messageNumber++;
	if(messageNumber==0x01)   //这里先不用判断重传次数
	{
	     if(mac_frameControl.bits.AckPolicy==mac_I_ACK)
		 {
			mac_packetHeader=mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++]=((macBIB.SecurityLevel<<3)|0x02);
			mac_TxBuffer[mac_packetHeader++]=0x10;   //控制帧，Iack
			mac_TxBuffer[mac_packetHeader++]=0x00;
			mac_TxBuffer[mac_packetHeader++]=0x00;

			mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;   //recipient address
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;   //简明地址
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

			mac_TxBuffer[mac_packetHeader++]=mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader++]=mac_FCS_MSB;

#ifdef DEBUG
			printf("收到的是第一PTK_FRAME，且需要回复ACK，ACK已成帧\n");
#endif
		}
		mac_TasksPending.bit.frameWaitingAck=1;    //把此事件置为1然后发送ack帧

		//收帧，处理
				srand((unsigned)time(NULL));  //调用time.h
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

			//给plaintext1赋值
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
				plaintext1[44]=0x02;   //第二PTK帧中的PTK_Control域
				plaintext1[45]=0x00;

			//给plaintext2赋值
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

			//做相应的计算处理
				CBC_encrypt(p1,macBIB.PTK,MK,46);    //生成PTK
				CBC_encrypt(p2,KCK,MK,46);
				CBC_encrypt(p1,P,KCK,46);
				memcpy(PTK_KMAC_2B,P,2*4*sizeof(char));
				memcpy(PTK_KMAC_3B,(char*)P+2*4,2*4*sizeof(char));

			/**************************处理完毕*************************/



		return MACFrameTransmission(mac_I_ACK_FRAME,mac_DIRECT,mac_TxBuffer,9);


			//currentPacket.length = 47;
			//currentPacket.frameType = MANAGEMENT_FRAME;
			//currentPacket.frameSubtype = mac_PTK_FRAME;

			//if( macTasksPending.bit.frameWaitingAck==0)    新建一个事件的函数
				//return MACFrameTransmission(mac_PTK_FRAME, mac_DIRECT,mac_TxBuffer,38);
			//else
				//return MACFrameTransmission(mac_I_ACK_FRAME,mac_DIRECT,mac_TxBuffer,9);

		}
		 else if(messageNumber==0x02)
		 {
		     // macBIB.abbrAddr.v=params.MACS_DATA_indication.RecipientID.v;
			  //macBIB.hubAddr.v=params.MACS_DATA_indication.SenderID.v;
			  //macBIB.BANID.v=params.MACS_DATA_indication.BANID.v;    //更新本地属性库
	          if(mac_frameControl.bits.AckPolicy==mac_I_ACK)
			  {
				  mac_packetHeader=mac_PACKET_HEADER_START;
				  mac_TxBuffer[mac_packetHeader++]=((macBIB.SecurityLevel<<3)|0x02);
				  mac_TxBuffer[mac_packetHeader++]=0x10;   //控制帧，Iack
				  mac_TxBuffer[mac_packetHeader++]=0x00;
				  mac_TxBuffer[mac_packetHeader++]=0x00;

				  mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;   //recipient address
			      mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;   //简明地址
			      mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

				  mac_TxBuffer[mac_packetHeader++]=mac_FCS_LSB;
				  mac_TxBuffer[mac_packetHeader++]=mac_FCS_MSB;

	#ifdef DEBUG
				  printf("收到的是第三PTK_FRAME，且需要回复ACK，ACK已成帧\n");
	#endif

			  }
			   mac_TasksPending.bit.ackSending=1;   //产生原语

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
				 params.HME_MLME_SECURITY_confirm.Status=0x00;   //暂时先判断这两种状态

			 MACGet();

			 for(sec_s=0;sec_s<8;sec_s++)
			 {
			 PTK_KMAC_3A[sec_s]=MACGet();
			 }
	         if(memcpy(PTK_KMAC_3B,PTK_KMAC_3A,8)==0)

			   //往高层上传原语
	             return HME_MLME_SECURITY_confirm;   //会在大循环中启动MACPrimitiveTransmission()函数
			 else
				 return NO_PRIMITIVE;
		 }

		}


	#endif     //I_AM_HUB




	#ifdef I_AM_NODE
			  //收到第二关联帧后，先生成ACK帧，不需要生成原语，发送ACK帧，组建新的第一PTK帧
	if(frameSubtype==mac_SECURITY_ASSOCIATION_FRAME)
	{
	           #ifdef DEBUG
				  printf("收到的是SECURITY_ASSOCIATION_FRAME\n");
	           #endif

				    //根据帧头配置macBIB
			  //macBIB.abbrAddr.v=params.MACS_DATA_indication.RecipientID.v;
			  //macBIB.hubAddr.v=params.MACS_DATA_indication.SenderID.v;
			  //macBIB.BANID.v=params.MACS_DATA_indication.BANID.v;    //更新本地属性库

	          //先生成ACK帧
			  if(mac_frameControl.bits.AckPolicy==mac_I_ACK)
			  {
				  mac_packetHeader=mac_PACKET_HEADER_START;
				  mac_TxBuffer[mac_packetHeader++]=((macBIB.SecurityLevel<<3)|0x02);
				  mac_TxBuffer[mac_packetHeader++]=0x10;   //安全管理帧
				  mac_TxBuffer[mac_packetHeader++]=0x00;
				  mac_TxBuffer[mac_packetHeader++]=0x00;

				  mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;   //recipient address
			      mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;   //简明地址
			      mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

				  mac_TxBuffer[mac_packetHeader++]=mac_FCS_LSB;
				  mac_TxBuffer[mac_packetHeader++]=mac_FCS_MSB;

	#ifdef DEBUG
				  printf("收到的是SecurityAssociationFrame，且需要回复ACK，ACK已成帧\n");
	#endif

			  }
			   mac_TasksPending.bit.frameWaitingAck=1;   //先发送ack再发送新组建的帧

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
		printf("收到的是PTK_FRAME\n");
 #endif

			//  macBIB.abbrAddr.v=params.MACS_DATA_indication.RecipientID.v;
			 // macBIB.hubAddr.v=params.MACS_DATA_indication.SenderID.v;
			 // macBIB.BANID.v=params.MACS_DATA_indication.BANID.v;          //更新本地属性库
	          if(mac_frameControl.bits.AckPolicy==mac_I_ACK)
			  {
				  mac_packetHeader=mac_PACKET_HEADER_START;
				  mac_TxBuffer[mac_packetHeader++]=((macBIB.SecurityLevel<<3)|0x02);
				  mac_TxBuffer[mac_packetHeader++]=0x10;   //控制帧，Iack
				  mac_TxBuffer[mac_packetHeader++]=0x00;
				  mac_TxBuffer[mac_packetHeader++]=0x00;

				  mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;   //recipient address
			      mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;   //简明地址
			      mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

				  //帧尾
				  mac_TxBuffer[mac_packetHeader++]=0x12;
				  mac_TxBuffer[mac_packetHeader++]=0x12;

	#ifdef DEBUG
				  printf("收到的是第二PTK_FRAME，且需要回复ACK，ACK已成帧\n");
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
			 MACGet();    //发送地址需在本地更新
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

			  		  //做相应的计算处理，产生PTK,PTK_KMAC_2...
			  		  //先对plaintext1赋值
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
			  			plaintext1[sec_s+12]=Nonce_I[sec_s];  //在组建第一PTK帧中已经产生并存入Nonce_I中
			  		  }
			  		  for(sec_s=0;sec_s<16;sec_s++)
			  		  {
			  			plaintext1[sec_s+28]=Nonce_R[sec_s];
			  		  }
			  			plaintext1[44]=PTK_Control[0];
			  			plaintext1[45]=PTK_Control[1];
			  		  //对plaintext2赋值
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

			  			//做相应的计算处理
			  			CBC_encrypt(p1,macBIB.PTK,MK,46);    //生成PTK
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
		//目前hub以及node节点收到原语后，要把原语先写进param中，然后进行组帧，发帧，不需要往macBIB中存入东西
#ifdef I_AM_NODE
	case NME_MLME_SECURITY_request:
		//第一关联帧头包装security association 帧打包  pp. 17
#ifdef DEBUG
	    printf("开始装配第一关联帧...\n");
#endif
		mac_packetHeader = mac_PACKET_HEADER_START;
		mac_TxBuffer[mac_packetHeader++] = ((params.NME_MLME_SECURITY_request.SecurityLevel<<3)|0x02);    //01: Immediate acknowledgment  securityLevel由高层指定
		mac_TxBuffer[mac_packetHeader++] = 0x02;   //frame subtype name:security association(0010)
		mac_TxBuffer[mac_packetHeader++] = 0x00;   //sequece number 置0
		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;  //recipient ID
		mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;  //??
		mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;  //BAN ID

		//Security Association payload包装这里需要    pp. 31
		//TxBuffer[packetHeader++] = macBIB.fullAddr.v[0];
		//帧接受者地址，无法确定，设置为0 ；
		mac_TxBuffer[mac_packetHeader++]=0;
		mac_TxBuffer[mac_packetHeader++]=0;
		mac_TxBuffer[mac_packetHeader++]=0;
		mac_TxBuffer[mac_packetHeader++]=0;
		mac_TxBuffer[mac_packetHeader++]=0;
		mac_TxBuffer[mac_packetHeader++]=0;
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];//发送者即本机地址
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];
		mac_TxBuffer[mac_packetHeader++] |=params.NME_MLME_SECURITY_request.SecurityLevel<<3;   //这一位需要param给出
		mac_TxBuffer[mac_packetHeader++]=0x00;//security_suite_seletor (先低位后高位)  0000000000000000，测试一下
		mac_TxBuffer[mac_packetHeader++]=0x01;
		mac_TxBuffer[mac_packetHeader++]=0x00;  //reserved 域置全0，Association_Control

		macBIB.SecurityLevel=params.NME_MLME_SECURITY_request.SecurityLevel;  //更新本地securityLevel

			//帧尾包装，校验和，暂缺
		mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
		mac_TxBuffer[mac_packetHeader] = mac_FCS_LSB;   //暂定为0x12

		return MACFrameTransmission( mac_SECURITY_ASSOCIATION_FRAME, mac_PROB, mac_TxBuffer,25);//mac_PROB需在WbanMAC.h中定义

	/*************************************************************************************/
#endif

#ifdef I_AM_HUB
	case HME_MLME_SECURITY_response:
#ifdef DEBUG
	    printf("开始装配第二关联帧...\n");
#endif
		mac_packetHeader = mac_PACKET_HEADER_START;
		mac_TxBuffer[mac_packetHeader++] = ((macBIB.SecurityLevel<<3)|0x02);  //I_ACK
		mac_TxBuffer[mac_packetHeader++] = 0x02;
		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = 0x00;
		mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;   //recipient address
		mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;
		mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;

			//Security Association payload包装这里需要    pp. 31
			//TxBuffer[packetHeader++] = macBIB.fullAddr.v[0];
			//帧接受者地址，无法确定，设置为0 ；
		mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[0];    //地址的问题，此时应该有接收地址了，利用nodeList
		mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[1];
		mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[2];
		mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[3];
		mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[4];
		mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[5];   // hub address
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];//发送者即本机地址,hub发送
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[1];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[2];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[3];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[4];
		mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[5];
		mac_TxBuffer[mac_packetHeader] = macBIB.SecurityLevel << 3;
		mac_TxBuffer[mac_packetHeader++] |= 1 << 5;   //Control Frame Authentication
		mac_TxBuffer[mac_packetHeader++]=0x00;//security_suite_seletor
		mac_TxBuffer[mac_packetHeader++]=0x02;  //association sequence number 发送的是第几帧就设定为几
		mac_TxBuffer[mac_packetHeader++]=0x00;//Association_Control
			//帧尾包装，校验和，暂缺
		mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
		mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

		return MACFrameTransmission( mac_SECURITY_ASSOCIATION_FRAME, mac_DIRECT, mac_TxBuffer,25);//mac_PROB需在WbanMAC.h中定义
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
		*mac_spiTxInfo.p++=mac_spiTxInfo.length.bytes.MSB;    //先高八位，后低八位
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
		*mac_spiTxInfo.p++=mac_spiTxInfo.length.bytes.MSB;    //先高八位，后低八位
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
		*mac_spiTxInfo.p++=mac_spiTxInfo.length.bytes.MSB;    //先高八位，后低八位
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
			//开始组建第二PTK帧
			mac_packetHeader = mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++] = ((macBIB.SecurityLevel<<3)|0x02|0x20);    //01: Immediate acknowledgment
			mac_TxBuffer[mac_packetHeader++] = 0x04;								//subtype: PTK_frame(0100)
			mac_TxBuffer[mac_packetHeader++] = 0x02;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = mac_UNCONNECTED_NID;					 //recipient ID
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;					//sender ID
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;						//BAN ID

			//PTK payload 的包装
			mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[0];
			mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[1];
			mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[2];
			mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[3];
			mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[4];
			mac_TxBuffer[mac_packetHeader++]=macBIB.nodeFullAddrTemp.v[5];                     // recipient address:hub address
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];//发送者即本机地址
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

			//帧尾
			mac_TxBuffer[mac_packetHeader++]=mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader++]=mac_FCS_MSB;

			return MACFrameTransmission(mac_PTK_FRAME, mac_DIRECT,mac_TxBuffer,47);  //发送第二PTK帧
#endif


#ifdef I_AM_NODE
		if(messageNumber==0x00)
		{
			messageNumber++;
			//组建帧头
			mac_packetHeader = mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++] = ((macBIB.SecurityLevel<<3)|0x02|0x20);    //01: Immediate acknowledgment
			mac_TxBuffer[mac_packetHeader++] = 0x04;  //subtype: PTK_frame(0100)
			mac_TxBuffer[mac_packetHeader++] = 0x01;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;  //recipient ID
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;  //sender ID
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;  //BAN ID


			//PTK payload 的包装
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[0];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[1];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[2];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[3];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[4];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[5];   // recipient address:hub address
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];//发送者即本机地址
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
			mac_TxBuffer[mac_packetHeader++]=0x00;     //PTK_KMAC 第一帧设置为0
		}

		//帧尾包装
			mac_TxBuffer[mac_packetHeader++] = mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader] = mac_FCS_MSB;

		return MACFrameTransmission( mac_PTK_FRAME, mac_DIRECT, mac_TxBuffer,47);
		}


		if(messageNumber==0x01)
	{
		if(memcmp(PTK_KMAC_2B,PTK_KMAC_2A,8)==0)
	      {
	     //如果相等，组建下一帧
			mac_packetHeader = mac_PACKET_HEADER_START;
			mac_TxBuffer[mac_packetHeader++] = ((macBIB.SecurityLevel<<3)|0x02|0x20);    //01: Immediate acknowledgment
			mac_TxBuffer[mac_packetHeader++] = 0x04;					 //subtype: PTK_frame(0100)
			mac_TxBuffer[mac_packetHeader++] = 0x03;
			mac_TxBuffer[mac_packetHeader++] = 0x00;
			mac_TxBuffer[mac_packetHeader++] = macBIB.hubAddr.v;		 //recipient ID
			mac_TxBuffer[mac_packetHeader++] = macBIB.abbrAddr.v;		 //sender ID
			mac_TxBuffer[mac_packetHeader++] = macBIB.BANID.v;			 //BAN ID，这里的地址是简明地址

			//PTK payload 的包装
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[0];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[1];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[2];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[3];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[4];
			mac_TxBuffer[mac_packetHeader++]=macBIB.hubFullAddr.v[5];							// recipient address:hub address
			mac_TxBuffer[mac_packetHeader++] = macBIB.fullAddr.v[0];	//发送者即本机地址
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

		//帧尾
			mac_TxBuffer[mac_packetHeader++]= mac_FCS_LSB;
			mac_TxBuffer[mac_packetHeader++]= mac_FCS_MSB;

		return MACFrameTransmission(mac_PTK_FRAME, mac_DIRECT,mac_TxBuffer,47);
		}
	}
#endif
		return NO_PRIMITIVE;
}
