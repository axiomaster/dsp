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
int PSDU_LEN;  //是个变化的量，因为传输来的PSDU frame body的长度不确定，表示的是psdu的位数（即01），但mac层传来的是psdu的字节数（十六进制表示的）
int PSDU_Mod_len, PLCP_Mod_len; //发送tx和接收rx都可以用
complex *PPDU_Mod;
Uint16 fpga_buf[2000] = {0}; //是否可以用malloc代替？？？？？从fpga接收来的16位01数据存放的数组，因为不知道长度所以建立固定长度数组
complex FPGArx[2000] = {0}; //放fpga接收来的16字节数据转化成复数数据
Uint16 FPGA_length; //spi从fpga接收来的数据的长度

extern CURRENT_PACKET mac_currentPacket;
/**********************************************************************
**函数原型:		WBAN_PRIMITIVE PHYTasks(WBAN_PRIMITIVE inputPrimitive)
**入口参数:   	WBAN_PRIMITIVE inputPrimitive
**返   回 	值：		WBAN_PRIMITIVE
**说      	明：		PHY层原语处理程序，根据不同的原语，作出相应的处理
***********************************************************************/
WBAN_PRIMITIVE PHYTasks(WBAN_PRIMITIVE inputPrimitive)
{
	BYTE temp[mac_PACKET_BUFFER_SIZE];
	WORD i;
	BYTE* p;

#ifdef DEBUG
	printf("\n数据传输至PHY层 @PHYTasks(...) @%ld\n", TickGet().Val);
#endif

#ifdef DEBUG1
	p = params.PL_DATA_request.psdu;
	printf( "\n帧为：" );
	for( i = 0; i < params.PL_DATA_request.psduLength; i++ )
		printf( "0x%x ", *(p++) );
	printf("\n帧长为：%d\n", params.PL_DATA_request.psduLength);
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

		//接收参数为mac层数组，长度，速率，返回的是调制完的复数数组。传来的长度是整个psdu的字节数
		//发送端没有用到全局的数组，用到了全局变量PLCP_Mod_len、PSDU_Mod_len
		PPDU_Mod = PHYtx( temp, params.PL_DATA_request.psduLength, mac_rate );
		SPI0_init();
		unsigned short *FPGAtx;
		FPGAtx = FPGAtxfun(PPDU_Mod, PLCP_Mod_len+PSDU_Mod_len); //里面free了ppdu_mod数组，动态开辟了FPGAtx数组
		spi0flash_cycletx(FPGAtx,(PLCP_Mod_len+PSDU_Mod_len+3)/2);
		free(FPGAtx);
		FPGAtx=NULL;

		break;
	default:
#ifdef DEBUG
		printf("\nWbanPHY.c PHYTasks() 找不到这个case\n");
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
**函数原型:		WBAN_PRIMITIVE dataIndicationProcess()
**入口参数:   	void
**返   回 	值：		WBAN_PRIMITIVE
**说      	明：		当有数据到达PHY时，配置param.PL_DATA_indication
***********************************************************************/
WBAN_PRIMITIVE dataIndicationProcess(){
	WORD j;
	SPI0_init();
	spi0flash_cyclerx(fpga_buf); //输出的是从fpga接收来的16位01数据存放的数组：fpga_buf

	for(j = 0; j < FPGA_length - 1; j++){
		fpga_buf[j] = fpga_buf[j + 1];
		}
	fpga_buf[FPGA_length-1] = 0;
	FPGArxfun(fpga_buf); //输出的是将16位01转化成的复数数组：FPGArx（全局复数数组）

	/*printf("\n");
	for(j=0;j<FPGA_length;j++)
		printf("%f+%fI  ",FPGArx[j].re,FPGArx[j].im);
	printf("\n");*/

	params.PL_DATA_indication.psduLength = PHYrx(FPGArx);
	//params.PL_DATA_indication.psduLength = mac_rxBufferlen;	//测试用
	params.PL_DATA_indication.psdu = mac_RxBuffer;


#ifdef DEBUG1
	printf("\n收到数据！将接收到的数据的Buffer的指针和长度赋值给\nPL_DATA_indication @dataIndicationProcess() @%ld\n", TickGet().Val);
	WORD i;
	BYTE* p;
	p = params.PL_DATA_indication.psdu;
	printf( "帧为：" );
	for( i = 0; i < params.PL_DATA_indication.psduLength; i++ )
		printf( "0x%x ", *(p++) );
	printf("\n帧长为:%d\n", params.PL_DATA_indication.psduLength);
#endif

	return PL_DATA_indication;
}

/**********************************************************************
**函数原型:		BYTE CCA()
**入口参数:   	void
**返   回 	值：		BYTE，返回当前的信道状态
**说      	明：		CCA检测
***********************************************************************/
BYTE CCA(){
	Delay(105); 	//mac_pCCATime
	return mac_IDLE;	//mac_BUSY or mac_IDLE
}

/**********************************************************************
**函数原型:		BYTE Scan()
**入口参数:   	BYTE scanType, BYTE channelFreBand, BYTE ChannelNum
**返   回 	值：		BYTE，返回当前的信道状态
**说      	明：		信道扫描（待补充）
***********************************************************************/
BYTE Scan(BYTE scanType, BYTE channelFreBand, BYTE ChannelNum){
	BYTE status = mac_SUCCESS;

	return status;
}
/**********************************************************************
**函数原型:		BYTE getMACFullAddr(BYTE n)		MAC地址: 01:23:45:67:89:01
**入口参数:   	n	mac地址的第几位				0		1		2		3		4		5
**返   回 	值：		BYTE，返回mac第 5 -n位上的数		0x01	0x89	0x67	0x45	0x23	0x01
**说      	明：		返回MAC全地址
***********************************************************************/
BYTE getMACFullAddr(BYTE n){
	BYTE fullAddr[] = MAC_FULL_ADDRESS;
	return fullAddr[5 - n];
}

unsigned short int *FPGAtxfun(complex *PPDU_Mod, int length)
	{
	//************将复数的实部和虚部存储为3位二进制数据表示一个点的格式*******************************

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

		if(length%2 == 0) //length为偶数
		{
			for(i=0; i<length/2; i++)
			{
				FPGAtx[i] = PPDUtrans[2*i];
				FPGAtx[i] = (FPGAtx[i] << 8) | PPDUtrans[2*i+1];
			}
		}
		else  //length为奇数
		{
			for(i=0; i<length/2; i++)
			{
				FPGAtx[i] = PPDUtrans[2*i];
				FPGAtx[i] = (FPGAtx[i] << 8) | PPDUtrans[2*i+1];
			}
			FPGAtx[(length-1)/2] = PPDUtrans[length-1];
			FPGAtx[(length-1)/2] = FPGAtx[(length-1)/2] << 8;
		}

		//验证测试结果
		//printf("FPGAtx的16位2进制表示为: \n");
		//length为偶数
		/*for(i=0;i<(length+1)/2;i++)
			printf("%x ",FPGAtx[i]);
		printf("\n");*/
		//length为奇数
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
	//将buf数组每个的高8位做实部，低8位做虚部，组成一个复数，放到FPGArx数组中
	signed char FPGAre; //变量用来存放复数的实部
	signed char FPGAim; //变量用来存放复数的虚部
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
