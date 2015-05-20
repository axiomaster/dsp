/*********************************************************************
 *
 *                  WbanPHY C File
 *
 *********************************************************************
 * FileName:        WbanPHY.c
 * Company:         Xidian University
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Yueyang Song       2013.10.02
 *Yueyang Song 		 2013.11.18		v1.2
 *Yueyang Song		 2014.11.22		v2.0
 ********************************************************************/
#include "WbanTasks.h"
#include "WbanMAC.h"		//Delay()
#include "WbanPHY.h"
#include "PHYrx.h"
#include "PHYtx.h"
#include "complex.h"
#include "registers.h"
#include "hardware.h"
#include <stdlib.h>
#include "FPGAtx.h"
#include <stdio.h>


BYTE mac_RxBuffer[mac_PACKET_BUFFER_SIZE];
WORD mac_rxBufferlen;
int PSDU_LEN;  //�Ǹ��仯��������Ϊ��������PSDU frame body�ĳ��Ȳ�ȷ������ʾ����psdu��λ������01������mac�㴫������psdu���ֽ�����ʮ�����Ʊ�ʾ�ģ�
int PSDU_Mod_len, PLCP_Mod_len; //����tx�ͽ���rx��������
complex *PPDU_Mod;
Uint16 fpga_buf[2000] = {0}; //�Ƿ������malloc���棿����������fpga��������16λ01���ݴ�ŵ����飬��Ϊ��֪���������Խ����̶���������
complex FPGArx[2000] = {0}; //��fpga��������16�ֽ�����ת���ɸ�������
Uint16 FPGA_length; //spi��fpga�����������ݵĳ���

extern CURRENT_PACKET mac_currentPacket;
/**********************************************************************
**����ԭ��:		WBAN_PRIMITIVE PHYTasks(WBAN_PRIMITIVE inputPrimitive)
**��ڲ���:   	WBAN_PRIMITIVE inputPrimitive
**��   �� 	ֵ��		WBAN_PRIMITIVE
**˵      	����		PHY��ԭ�ﴦ����򣬸��ݲ�ͬ��ԭ�������Ӧ�Ĵ���
***********************************************************************/
WBAN_PRIMITIVE PHYTasks(WBAN_PRIMITIVE inputPrimitive)
{
	BYTE temp[mac_PACKET_BUFFER_SIZE];
	WORD i;
	BYTE* p;

#ifdef DEBUG
	printf("\n���ݴ�����PHY�� @PHYTasks(...) @%ld\n", TickGet().Val);
#endif

#ifdef DEBUG1
	p = params.PL_DATA_request.psdu;
	printf( "\n֡Ϊ��" );
	for( i = 0; i < params.PL_DATA_request.psduLength; i++ )
		printf( "0x%x ", *(p++) );
	printf("\n֡��Ϊ��%d\n", params.PL_DATA_request.psduLength);
#endif


	p = params.PL_DATA_request.psdu;
	for( i = 0; i < params.PL_DATA_request.psduLength; i++ )
		temp[i] = *(p++);

	int mac_rate;

	switch( inputPrimitive ){
	case PL_DATA_request:
		switch( params.PL_DATA_request.informationDataRate ){
		case DATA_RATE_121_4:
			mac_rate = 121;
			break;
		case DATA_RATE_242_9:
			mac_rate = 242;
			break;
		case DATA_RATE_485_7:
			mac_rate = 485;
			break;
		case DATA_RATE_971_4:
			mac_rate = 971;
			break;
		default:
			;
		}

		//���ղ���Ϊmac�����飬���ȣ����ʣ����ص��ǵ�����ĸ������顣�����ĳ���������psdu���ֽ���
		//���Ͷ�û���õ�ȫ�ֵ����飬�õ���ȫ�ֱ���PLCP_Mod_len��PSDU_Mod_len
		PPDU_Mod = PHYtx( temp, params.PL_DATA_request.psduLength, mac_rate );
		SPI0_init();
		unsigned short *FPGAtx;
		FPGAtx = FPGAtxfun(PPDU_Mod, PLCP_Mod_len+PSDU_Mod_len); //����free��ppdu_mod���飬��̬������FPGAtx����
		spi0flash_cycletx(FPGAtx,(PLCP_Mod_len+PSDU_Mod_len+3)/2);
		free(FPGAtx);
		FPGAtx=NULL;

		break;
	default:
#ifdef DEBUG
		printf("\nWbanPHY.c PHYTasks() �Ҳ������case\n");
#endif
		;
	}

	if( (mac_currentPacket.frameType_Subtype & 0xF0) == mac_DATA_FRAME && mac_currentPacket.ackPolicy == mac_N_ACK && mac_currentPacket.non_finalFragment == FALSE){
		params.MACS_DATA_confirm.RecipientID.v = mac_currentPacket.RecipientID.v;
#ifdef I_AM_NODE
		params.MACS_DATA_confirm.TxDSN = macBIB.macTxDSN;
#endif
#ifdef I_AM_HUB
		params.MACS_DATA_confirm.TxDSN = macBIB.macTxDSN[getIndexFromNodeList(mac_currentPacket.RecipientID)];
#endif
		params.MACS_DATA_confirm.status = mac_SUCCESS;

		return MACS_DATA_confirm;
	}

	return NO_PRIMITIVE;
}

/**********************************************************************
**����ԭ��:		WBAN_PRIMITIVE dataIndicationProcess()
**��ڲ���:   	void
**��   �� 	ֵ��		WBAN_PRIMITIVE
**˵      	����		�������ݵ���PHYʱ������param.PL_DATA_indication
***********************************************************************/
WBAN_PRIMITIVE dataIndicationProcess(){
	WORD j;
	SPI0_init();
	spi0flash_cyclerx(fpga_buf); //������Ǵ�fpga��������16λ01���ݴ�ŵ����飺fpga_buf

	for(j = 0; j < FPGA_length - 1; j++){
		fpga_buf[j] = fpga_buf[j + 1];
		}
	fpga_buf[FPGA_length-1] = 0;
	FPGArxfun(fpga_buf); //������ǽ�16λ01ת���ɵĸ������飺FPGArx��ȫ�ָ������飩

	/*printf("\n");
	for(j=0;j<FPGA_length;j++)
		printf("%f+%fI  ",FPGArx[j].re,FPGArx[j].im);
	printf("\n");*/

	params.PL_DATA_indication.psduLength = PHYrx(FPGArx);
	//params.PL_DATA_indication.psduLength = mac_rxBufferlen;	//������
	params.PL_DATA_indication.psdu = mac_RxBuffer;


#ifdef DEBUG1
	printf("\n�յ����ݣ������յ������ݵ�Buffer��ָ��ͳ��ȸ�ֵ��\nPL_DATA_indication @dataIndicationProcess() @%ld\n", TickGet().Val);
	WORD i;
	BYTE* p;
	p = params.PL_DATA_indication.psdu;
	printf( "֡Ϊ��" );
	for( i = 0; i < params.PL_DATA_indication.psduLength; i++ )
		printf( "0x%x ", *(p++) );
	printf("\n֡��Ϊ:%d\n", params.PL_DATA_indication.psduLength);
#endif

	return PL_DATA_indication;
}

/**********************************************************************
**����ԭ��:		BYTE CCA()
**��ڲ���:   	void
**��   �� 	ֵ��		BYTE�����ص�ǰ���ŵ�״̬
**˵      	����		CCA���
***********************************************************************/
BYTE CCA(){
	Delay(105); 	//mac_pCCATime
	return mac_IDLE;	//mac_BUSY or mac_IDLE
}

/**********************************************************************
**����ԭ��:		BYTE Scan()
**��ڲ���:   	BYTE scanType, BYTE channelFreBand, BYTE ChannelNum
**��   �� 	ֵ��		BYTE�����ص�ǰ���ŵ�״̬
**˵      	����		�ŵ�ɨ�裨�����䣩
***********************************************************************/
BYTE Scan(BYTE scanType, BYTE channelFreBand, BYTE ChannelNum){
	BYTE status = mac_SUCCESS;

	return status;
}
/**********************************************************************
**����ԭ��:		BYTE getMACFullAddr(BYTE n)		MAC��ַ: 01:23:45:67:89:01
**��ڲ���:   	n	mac��ַ�ĵڼ�λ				0		1		2		3		4		5
**��   �� 	ֵ��		BYTE������mac�� 5 -nλ�ϵ���		0x01	0x89	0x67	0x45	0x23	0x01
**˵      	����		����MACȫ��ַ
***********************************************************************/
BYTE getMACFullAddr(BYTE n){
	BYTE fullAddr[] = MAC_FULL_ADDRESS;
	return fullAddr[5 - n];
}

unsigned short int *FPGAtxfun(complex *PPDU_Mod, int length)
	{
	//************��������ʵ�����鲿�洢Ϊ3λ���������ݱ�ʾһ����ĸ�ʽ*******************************

		unsigned short int* FPGAtx;
		unsigned char* PPDUtrans;
		PPDUtrans = (unsigned char*)malloc(length * sizeof(unsigned char));
		//FPGAtx = (unsigned short int*)malloc((length+1)/2 * sizeof(unsigned short int));
		FPGAtx = (unsigned short int*)malloc((length+3)/2 * sizeof(unsigned short int));

		int i;
		for(i=0; i<length; i++)
		{
			//printf("%f+%fI  ",PPDU_Mod[i].re,PPDU_Mod[i].im);
			if(PPDU_Mod[i].re > 0.9 && PPDU_Mod[i].re < 1.1 )
				PPDUtrans[i] = 1;
			else if(PPDU_Mod[i].re > -1.1 && PPDU_Mod[i].re < -0.9)
				PPDUtrans[i] = 8;
			else if(PPDU_Mod[i].im > 0.9 && PPDU_Mod[i].im < 1.1)
				PPDUtrans[i] = 4;
			else if(PPDU_Mod[i].im > -1.1 && PPDU_Mod[i].im < -0.9)
				PPDUtrans[i] = 12;
			/*else if((int)(PPDU_Mod[i].re * 1000) == 707 && (int)(PPDU_Mod[i].im * 1000) == 707)
				PPDUtrans[i] = 1;
			else if((int)(PPDU_Mod[i].re * 1000) == -707 && (int)(PPDU_Mod[i].im * 1000) == 707)
				PPDUtrans[i] = 3;
			else if((int)(PPDU_Mod[i].re * 1000) == -707 && (int)(PPDU_Mod[i].im * 1000) == -707)
				PPDUtrans[i] = 5;
			else if((int)(PPDU_Mod[i].re * 1000) == 707 && (int)(PPDU_Mod[i].im * 1000) == -707)
				PPDUtrans[i] = 7;*/
			else if(PPDU_Mod[i].re > 0.6 && PPDU_Mod[i].re < 0.8 && PPDU_Mod[i].im > 0.6 && PPDU_Mod[i].im < 0.8)
				PPDUtrans[i] = 2;
			else if(PPDU_Mod[i].re > -0.8 && PPDU_Mod[i].re < -0.6 && PPDU_Mod[i].im > 0.6 && PPDU_Mod[i].im < 0.8)
				PPDUtrans[i] = 6;
			else if(PPDU_Mod[i].re > -0.8 && PPDU_Mod[i].re < -0.6 && PPDU_Mod[i].im > -0.8 && PPDU_Mod[i].im < -0.6)
				PPDUtrans[i] = 10;
			else if(PPDU_Mod[i].re > 0.6 && PPDU_Mod[i].re < 0.8 && PPDU_Mod[i].im > -0.8 && PPDU_Mod[i].im < -0.6)
				PPDUtrans[i] = 14;
			//else
				//printf("NO ");
			//printf("%f ", PPDU_Mod[i].re);
		}
		//printf("\n");

		if(length%2 == 0) //lengthΪż��
		{
			for(i=0; i<length/2; i++)
			{
				FPGAtx[i] = PPDUtrans[2*i];
				FPGAtx[i] = (FPGAtx[i] << 8) | PPDUtrans[2*i+1];
			}
		}
		else  //lengthΪ����
		{
			for(i=0; i<length/2; i++)
			{
				FPGAtx[i] = PPDUtrans[2*i];
				FPGAtx[i] = (FPGAtx[i] << 8) | PPDUtrans[2*i+1];
			}
			FPGAtx[(length-1)/2] = PPDUtrans[length-1];
			FPGAtx[(length-1)/2] = FPGAtx[(length-1)/2] << 8;
		}

		//��֤���Խ��
		//printf("FPGAtx��16λ2���Ʊ�ʾΪ: \n");
		//lengthΪż��
		/*for(i=0;i<(length+1)/2;i++)
			printf("%x ",FPGAtx[i]);
		printf("\n");*/
		//lengthΪ����
		/*for(i=0;i<(length+1)/2;i++)
			printf("%x ",FPGAtx[i]);
		printf("\n");*/

		free(PPDUtrans);
		PPDUtrans=NULL;
		free(PPDU_Mod);
		PPDU_Mod=NULL;

		return(FPGAtx);
	}

void FPGArxfun(Uint16 *buf)
{
	//��buf����ÿ���ĸ�8λ��ʵ������8λ���鲿�����һ���������ŵ�FPGArx������
	signed char FPGAre; //����������Ÿ�����ʵ��
	signed char FPGAim; //����������Ÿ������鲿
	Uint16 i;
	//while(buf[i] != 0xFFFF)
	//{
	for(i=0; i<FPGA_length; i++)
	{
		FPGAim = buf[i];
		buf[i] = buf[i] >> 8;
		FPGAre = buf[i];
		FPGArx[i].re = (double)FPGAre;
		FPGArx[i].im = (double)FPGAim;
		//printf("%d: %f+%fI  ",i+1, FPGArx[i].re,FPGArx[i].im);
	}
		//buf[i] = 0xFFFF;
		//i++;
	//}
}

BOOL spiflash_cycletest(void)
{
	Uint8 buff[1] = {0xCC};
	printf("%d ", buff[0]);
	int i;
	SPI_SPIBUF;

	Uint8 *test_pointer2;
	test_pointer2=(Uint8*)(0x01F0E040); //SPI1BUF
	//Uint8 *test_pointer1;
	//test_pointer1=(Uint8*)(0x01F0E03C);
	SPI1_init();
	for(i=0; i<1; i++)
	{
		while( SPI_SPIBUF & 0x80000000 );
		//if(!(SPI_SPIBUF & ( 0x80000000 )) )
		//{
			buff[0] = *test_pointer2;
			printf("%d ", buff[0]);
		//}
		//else
			//break;
	}
	if(buff[0] == 0xA0)
	{
		SPI_SPIDAT1 = (spidat1 & 0x0ffcffff) | buff[0];
		return TRUE;
	}
	else
		return FALSE;
}
