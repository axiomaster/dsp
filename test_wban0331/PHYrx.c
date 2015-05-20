#ifdef DEBUG
#include <stdio.h>
#endif
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include"complex.h"
#include"PHYHeadGen.h"
#include"PHYHeadCRC.h"
#include"BCH.h"
#include"spread.h"
#include"interleave.h"
#include"scrambler.h"
#include "PHYrx.h"
#include "PHYtx.h"
#include"dpsk.h"
#include "PHY.h"
#include "registers.h"

#include "WbanPHY.h"
#include "Wban.h"
/*#define FrameNum 1   //仿真设定传输的帧数为1帧
#define PLCP_H_LEN 31
#define PLCP_H_SP_LEN (PLCP_H_LEN*PLCP_H_SpreadFactor)
#define PLCP_P_LEN 90
#define PLCP_LEN (PLCP_H_SP_LEN+PLCP_P_LEN) //扩频后的PLCP_header和PLCP Preamble的长度和
#define PSDU_SP_LEN (PSDU_LEN*PSDU_SpreadFactor)
#define PPDU_LEN (PLCP_LEN+PSDU_LEN)
#define PI 3.1415926
#define uc unsigned char*/
extern complex FPGArx[2000];
//extern int PSDU_LEN;
extern int PSDU_Mod_len, PLCP_Mod_len;
extern Uint16 FPGA_length;

WORD PHYrx(complex *PPDU_Mod1) //入参数全局复数数组FPGArx
{

//自己设定的值，实际项目中需要mac层或高层确定给出
	int m;
	int ChannelNum=2; //信道编号,P187
	//int BurstMode=1; //是否突发模式，1是0否，自己设定，没有从头信息中得到此位，将其直接按照这个数做的。是否mac层确定之后即可按照常数用？？？？？？？？？？？？？？？？、、
	//int PSDU_S_seed = 0;//PSDU的scrambler seed和plcp header的ss一样，由Channel number决定，然后每次都叠加,这里先将其初始化为0，后面再根据信道的奇偶性进行赋值
	int PLCP_H_SpreadFactor = 4; //在2360MHz-2483.5MHz频率范围,PLCP头的扩展因子取4，接收端当已知用。
	int PLCP_H_M = 2; ///在2360MHz-2483.5MHz频率范围,PLCP header的调制阶数为2
	//int MAC_frame_body_len_uc = 0;//从mac层接收来的实际的macframebody的字节数
	//double Total_len;
	int  F_err=0;

	//函数内部定义的需要的自变量
	//int *PSDU_LEN;//自己写的
	//int FrameNum=1;
	//uc *PSDU;
	int i, j,PSDU_init_len;
	//int MAC_frame_body_len;
	int PLCP_H_S_seed;  //PSDU_M表示PSDU调制阶数 
	//uc *PLCP_P;
	complex *PLCP_Mod_R;  
	uc *PLCP_H_Dec, *PLCP_H_Decoded, *PSDU_Decode;
	int CRCCheck_resulst, rate_dec, PSDU_SpreadFactor_R, PSDU_M_R;
	double *PLCP_DeMod_SoftInfo, *PLCP_H_DeMod_SoftInfo;
	double *PLCP_H_DeSpread_SoftInfo;
	int MACFrameBodyLength_dec=0, flag, PSDU_S_seed_R;
	int PSDU_DeMod_len;
	uc *PSDU_Dec;
	double *PSDU_DeMod_SoftInfo, *PSDU_DeSpread_SoftInfo;
	complex *PSDU_Mod_R;
	//int Mod_len;
	uc *PSDU_MAC;
	int temp = 0, PSDU_MAC_length;  //PSDU_MAC_length为传给mac层的16进制的数组的长度


	if(ChannelNum%2==0)
	{
		PLCP_H_S_seed=0;
		//PLCP_P = PLCP_P1;
	}

	else
	{
		PLCP_H_S_seed=1;

	}


	for(j=0;j<FrameNum;j++)  //此处设定只传一帧数据
	{

		MACFrameBodyLength_dec=0;
		//PSDU_S_seed=(PSDU_S_seed+1)%2;//每帧变换1次，文档 P185

		//MAC_frame_body_len = MAC_frame_body_len_uc * 8;//程序中把macframebody的每一位存放到一个字节里，因为要用到位处理，所以要乘以8
		//MAC_frame_body_len_uc = length -9;
		//PSDU_LEN = (7+MAC_frame_body_len_uc)*8+16;//PSDU长度=MAC帧体数据+头数据的长度+FCS*/

//输入发送端得到的复数数组，函数输入的参数即为需要的复数数组，因此不需要专门的程序

//通过解调译码得出PLCP的头信息，并由此得到解调PSDU所需要的参数
/*---------------------------------接收端，PLCP解调译码--------------------------------------*/
		//Mod_len = PLCP_Mod_len + PSDU_Mod_len;
		//printf("\n");
		//printf("从fpga接收来的一帧的FPGArx数组长度为:%d\n",FPGA_length); //315
		/*for(i=0;i<FPGA_length;i++)
			printf("%f+%fI  ",PPDU_Mod1[i].re,PPDU_Mod1[i].im);
		printf("\n");*/
		//PLCP_Mod_R=(complex*)malloc(FPGA_length*sizeof(complex));
		PLCP_Mod_R = PPDU_Mod1;
		/*printf("\n");
		for(i=0;i<FPGA_length;i++)
			printf("%f+%fI  ",FPGArx[i].re,FPGArx[i].im);
		printf("\n");*/

		PLCP_DeMod_SoftInfo = DPSK_DeMod_SoftInfo(PLCP_Mod_R,PLCP_LEN,PLCP_H_M); //软解调（动态开辟了数组）
		//printf("PLCP解调之后长度为：%d\n PLCP解调之后输出为：\n", PLCP_LEN);  //214（PLCP的长度一直都不变）
		/*printf("\n");
		printf("%d ", PLCP_LEN);
		printf("\n");
		for(i=0;i<PLCP_LEN;i++)
			printf("%d: %f ",i+1, PLCP_DeMod_SoftInfo[i]);
		printf("\n");*/

		PLCP_H_DeMod_SoftInfo = PLCP_DeMod_SoftInfo + PLCP_P_LEN;//指针可以加常数长度，表示指针往后移动的位数
		//printf("PLCP header解调之后长度为：%d\n PLCP header解调之后输出为：\n", PLCP_H_SP_LEN);  //124=214-90
		/*printf("解扰前：\n");
		for(i=0;i<PLCP_H_SP_LEN;i++)
			printf("%f ",PLCP_H_DeMod_SoftInfo[i]);
		printf("\n");*/

//**********PLCP header经历了一系列过程，但是PLCP Preamble只经过了调制，因此以下一系列的解动作都是针对PLCP Header的，所以需要将指针指向PLCP Header，然后再进行以下动作************************************//

		Scrambler_Soft(PLCP_H_DeMod_SoftInfo, PLCP_H_SP_LEN, PLCP_H_S_seed, PLCP_H_M);//Descramble
		/*printf("解扰后：\n");
		for(i=0;i<PLCP_H_SP_LEN;i++)
		printf("%f ",PLCP_H_DeMod_SoftInfo[i]);
		printf("\n");*/

		DeInterleaver(PLCP_H_DeMod_SoftInfo,PLCP_H_SP_LEN,PLCP_H_SpreadFactor); //DeInterleave
		/*printf("解交织后：\n");
		for(i=0;i<PLCP_H_SP_LEN;i++)
			printf("%f ",PLCP_H_DeMod_SoftInfo[i]);
		printf("\n");*/

		PLCP_H_DeSpread_SoftInfo=DeSpreader(PLCP_H_DeMod_SoftInfo,PLCP_H_SP_LEN,PLCP_H_SpreadFactor); //DeSpread
		//此处动态开辟了解扩之后的数组内存，所以之前的要释放
		/*printf("解扩后：\n");
		for(i=0; i<PLCP_H_SP_LEN / PLCP_H_SpreadFactor; i++)
			printf("%f ",PLCP_H_DeSpread_SoftInfo[i]);
		printf("\n");*/

		free(PLCP_DeMod_SoftInfo);
		PLCP_DeMod_SoftInfo=NULL;

		PLCP_H_Dec=DPSK_DeMod_SoftDec(PLCP_H_DeSpread_SoftInfo,PLCP_H_LEN,PLCP_H_M); //PLCP头信息判决

		//printf("PLCP header解调后译码前：\n");  //PLCP_H_LEN = 31
		printf("\n");
		for(i=0;i<PLCP_H_LEN;i++)
		printf("%d ",PLCP_H_Dec[i]);
		printf("\n");

		free(PLCP_H_DeSpread_SoftInfo);  //释放空间并且置该空间指针为空
		PLCP_H_DeSpread_SoftInfo=NULL;
		
		PLCP_H_Decoded= BCHDecoding_PHYH(PLCP_H_Dec);   //PHY_H的BCH译码
		free(PLCP_H_Dec);
		PLCP_H_Dec=NULL;

		printf("PLCP header解调后译码后：\n");  //译码后结果19位都是对的VVVVVVVVVVVVV
		for(i=0;i<19;i++)
		printf("%d ",PLCP_H_Decoded[i]);
		printf("\n");

/*---------------------------------接收端，PSDU解调译码--------------------------------------*/

		CRCCheck_resulst = PHYHeadCRCCheck(PLCP_H_Decoded,19); //物理层头数据的CRC校验位，对应于发送端的HCS校验
		if(CRCCheck_resulst == 0)
		{
			F_err++;
			printf("头检验失败，此帧错误！\n");//校验失败输出此行

			continue;
		}
		else  //校验成功则继续运行，不输出任何东西
		{
			//Total_len += (7+MAC_frame_body_len)*8+16;
			rate_dec = PLCP_H_Decoded[0]*4 + PLCP_H_Decoded[1]*2 + PLCP_H_Decoded[2];
		}

		//printf("PLCP header解调之后得到的速率为：%d\n", rate_dec);  //0,即速率为121

		switch(rate_dec)//在2360MHz-24830.5MHz频率范围内  //从头信息的解调结果中得到PSDU的速率：rate_dec
		{
		case 0: //[0 0 0]
			PSDU_SpreadFactor_R=4;
			PSDU_M_R=2; //PSDU调制阶数
			break;
		case 4: //[1 0 0]
			PSDU_SpreadFactor_R=2;
			PSDU_M_R=2;
			break;
		case 2: //[0 1 0]485
			PSDU_SpreadFactor_R=1;
			PSDU_M_R=2;
			break;
		case 6: //[1 1 0]
			PSDU_SpreadFactor_R=1;
			PSDU_M_R=4;
			break;
		default:
			PSDU_SpreadFactor_R=100; //赋值肯定错误的值
			PSDU_M_R=100;
			break;
		}
		flag=1;
		for(i=4;i<12;i++)  //从头信息的解调结果中得到PSDU的长度：MACFrameBodyLength_dec
		{
			MACFrameBodyLength_dec += PLCP_H_Decoded[i]*flag;
			flag=flag*2;
		}
		//printf("PSDU的MACFrameBody长度为：%d\n", MACFrameBodyLength_dec);//100-9=91

		PSDU_S_seed_R = PLCP_H_Decoded[14];  //从头信息的解调结果中得到PSDU的扰码种子：PSDU_S_seed_R
		//printf("PSDU的MACFrameBody扰码种子为：%d\n", PSDU_S_seed_R);//1

		free(PLCP_H_Decoded);
		PLCP_H_Decoded=NULL;

		//printf("\n%d ", PLCP_Mod_len);
		m=(int)ceil(log(PLCP_H_M)/log(2)); //ceil函数的作用是求不小于给定实数的最小整数
    	//printf("%d\n",m); // m=1
		PLCP_Mod_len = PLCP_LEN/m; //PLCP调制之后的长度 = 214

		PSDU_Mod_R = PPDU_Mod1 + PLCP_Mod_len; //PSDU_Mod_R和PPDU_Mod1是复数类型的指针，PLCP_Mod_len是长度
		/*printf("PSDU解调之前的数据为:\n");
		for(i=0;i<416;i++)
			printf("%f+%fI  ",PSDU_Mod_R[i].re,PSDU_Mod_R[i].im);
		printf("\n");*/

		PSDU_init_len = 72 + MACFrameBodyLength_dec * 8;//PSDU原始长度

		//printf("PSDU的长度为：%d\n", PSDU_init_len);  //  208=136+72 因为MACFrameBodyLength_dec=136   

		PSDU_DeMod_len = PSDUcodelen(PSDU_init_len,PSDU_M_R,PSDU_SpreadFactor_R);
		//printf("PSDU的接收端的调制指数：%d\n", PSDU_M_R);//2
		//printf("PSDU的接收端的扩频指数：%d\n", PSDU_SpreadFactor_R);//4
		//printf("原始的PSDU的调制之后（即解调之前）的长度为：%d\n", PSDU_DeMod_len);//592,因为发送端和接收端是分开的，所以需要重新计算
		/*printf("PSDU解调之前的数据为:\n");
		for(i=0;i<PSDU_DeMod_len;i++)
			printf("%f+%fI  ",PSDU_Mod_R[i].re,PSDU_Mod_R[i].im);
		printf("\n");*/

		PSDU_DeMod_SoftInfo = DPSK_DeMod_SoftInfo(PSDU_Mod_R,PSDU_DeMod_len,PSDU_M_R); //PSDU软解调（函数内心开辟了内存，但是因为原来的是全局，所以不用释放）
		/*printf("PSDU解调之后解扰之前的长度为：%d\n, 数据为:\n",PSDU_DeMod_len);
		for(i=0; i<PSDU_DeMod_len; i++)
			printf("%f ",PSDU_DeMod_SoftInfo[i]);
		printf("\n");*/

		Scrambler_Soft(PSDU_DeMod_SoftInfo,PSDU_DeMod_len,PSDU_S_seed_R,PSDU_M_R);//PSDU解扰
		DeInterleaver(PSDU_DeMod_SoftInfo,PSDU_DeMod_len,PSDU_SpreadFactor_R); //PSDU解交织


		PSDU_DeSpread_SoftInfo = DeSpreader(PSDU_DeMod_SoftInfo,PSDU_DeMod_len,PSDU_SpreadFactor_R); //PSDU解扩


		free(PSDU_DeMod_SoftInfo);
		PSDU_DeMod_SoftInfo=NULL;

		PSDU_Dec = DPSK_DeMod_SoftDec(PSDU_DeSpread_SoftInfo,PSDU_DeMod_len/PSDU_SpreadFactor_R,PSDU_M_R); //PSDU判决

		//printf("原始的PSDU的解扩之后（即译码之前）的长度为：%d\n", PSDU_DeMod_len/PSDU_SpreadFactor_R);// 148 = 592/4
		/*printf("PSDU译码之前数据为:\n");
		for(i=0; i<PSDU_DeMod_len/PSDU_SpreadFactor_R; i++)
			printf("%d ",PSDU_Dec[i]);
		printf("\n");*/

		PSDU_Decode = BCHDecoding_PSDU(PSDU_Dec,PSDU_DeMod_len/PSDU_SpreadFactor_R,PSDU_M_R);//PSDU译码


		free(PSDU_DeSpread_SoftInfo);
		PSDU_DeSpread_SoftInfo = NULL;
		//free(FPGArx);
		//FPGArx = NULL;
		free(PSDU_Dec);
		PSDU_Dec = NULL;

		printf("PSDU长度为：%d\n", PSDU_init_len);
		printf("PSDU接收端:\n");
		for(i=0; i<PSDU_init_len; i++)
			printf("%d ",PSDU_Decode[i]);
		printf("\n");

//******************2进制转化16进制******************************************//
		PSDU_MAC_length = PSDU_init_len /8;
		PSDU_MAC = (uc*)malloc(PSDU_MAC_length * sizeof(uc));
		for(i=0;i<PSDU_MAC_length;i++)
		{
			for(j=0;j<8;j++)
			{
				temp |= PSDU_Decode[i*8+j];
				temp = temp <<1;
			}
			temp = temp >>1;
			PSDU_MAC[i] = (uc)temp;
			temp = 0;
			printf("%x ",PSDU_MAC[i] );
		}
		
		//free(PSDU);
		//PSDU=NULL;
		free(PSDU_Decode);
		PSDU_Decode = NULL;
	
	  if(F_err != 0)  //如果不输出，表示所有帧校验成功，会累加
		  printf("头校验失败，误帧数为%d\n",F_err);

	  //动态数组赋值给静态数组
	  for(i=0;i<PSDU_MAC_length;i++)
	  {
		  mac_RxBuffer[i] = PSDU_MAC[i];

	  }

	  free(PSDU_MAC);
	  PSDU_MAC = NULL;

	}
	return ((WORD)PSDU_MAC_length);
}
