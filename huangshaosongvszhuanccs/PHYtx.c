#include<stdio.h>
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

/*#define FrameNum 1   //仿真设定传输的帧数为1帧
 #define PLCP_H_LEN 31
 #define PLCP_H_SP_LEN (PLCP_H_LEN*PLCP_H_SpreadFactor)
 #define PLCP_P_LEN 90
 #define PLCP_LEN (PLCP_H_SP_LEN+PLCP_P_LEN) //扩频后的PLCP_header和PLCP Preamble的长度和
 #define PSDU_SP_LEN (PSDU_LEN*PSDU_SpreadFactor)
 #define PPDU_LEN (PLCP_LEN+PSDU_LEN)
 #define PI 3.1415926
 #define uc unsigned char*/

extern int PSDU_LEN;
extern int PSDU_Mod_len, PLCP_Mod_len;

complex *PHYtx(unsigned char *PSDU_MAC, uc length, int rate) {

	/**************************************************仿真参数设置*******************************************************/

	//自己设定的值，实际项目中需要mac层或高层确定给出
	int ChannelNum = 1; //信道编号,P187
	int BurstMode = 1; //是否突发模式，1是0否，自己设定
	int PSDU_S_seed = 0; //变量的初始化，PSDU的scrambler seed和plcp header的ss一样，由Channel number决定，然后每次都叠加,这里先将其初始化为0，后面再根据信道的奇偶性进行赋值
	int PLCP_H_SpreadFactor = 4; //在2360MHz-2483.5MHz频率范围,PLCP头的扩展因子取4，接收端当已知用。
	int PLCP_H_M = 2; ///在2360MHz-2483.5MHz频率范围,PLCP header的调制阶数为2
	int MAC_frame_body_len_uc = 0; //变量的初始化，从mac层接收来的实际的macframebody的字节数

	//函数内部定义的需要的自变量
	uc *PSDU;
	int i, j, m, i1;
	int MAC_frame_body_len, PLCP_H_S_seed, PSDU_M;  //PSDU_M表示PSDU调制阶数
	uc *PLCP_H;
	uc *PLCP_H_Spread, *PLCP_P, *PLCP;
	complex *PLCP_Mod, *PSDU_Mod;
	uc *PSDU_Spread, *PSDU_Coded_Pad;
	int PSDU_SpreadFactor;  //rate_dec, PSDU_SpreadFactor_R, PSDU_M_R指的是接收端的
	complex *PPDU_Mod;
	int Mod_len;

	uc PLCP_P1[90] = { 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,
			1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1,
			0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1,
			0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,
			0, 1 };
	//PLCP preamble sequence#1
	uc PLCP_P2[90] = { 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0,
			1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1,
			1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1,
			0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,
			0, 1 };
	//PLCP preamble sequence#2

	if (ChannelNum % 2 == 0) {
		PLCP_H_S_seed = 0;
		PLCP_P = PLCP_P1;
	}

	else {
		PLCP_H_S_seed = 1;
		PLCP_P = PLCP_P2;
	}

	switch (rate) //在2360MHz-2483.5MHz频率范围内
	{
	case 121:
		PSDU_SpreadFactor = 4;
		PSDU_M = 2; //PSDU调制阶数
		break;
	case 242:
		PSDU_SpreadFactor = 2;
		PSDU_M = 2;
		break;
	case 485:
		PSDU_SpreadFactor = 1;
		PSDU_M = 2;
		break;
	case 971:
		PSDU_SpreadFactor = 1;
		PSDU_M = 4;
		break;
	default:
		printf("Error Rate!");
		printf("No matched rate found!\n");
		break;
	}
	for (j = 0; j < FrameNum; j++)  //此处设定只传一帧数据
			{

		/********************************PSDU原始数据(仿真模拟接收数据)**********************************************************/
		//MACFrameBodyLength_dec=0;
		PSDU_S_seed = (PSDU_S_seed + 1) % 2;  //每帧变换1次，文档 P185

		MAC_frame_body_len_uc = length - 9;
		MAC_frame_body_len = MAC_frame_body_len_uc * 8; //程序中把macframebody的每一位存放到一个字节里，因为要用到位处理，所以要乘以8

		PSDU_LEN = (7 + MAC_frame_body_len_uc) * 8 + 16; //PSDU长度=MAC帧体数据+头数据的长度+FCS
		//PSDU_LEN = (7+MAC_frame_body_len_uc)*16+32;//PSDU长度=MAC帧体数据+头数据的长度+FCS
		PSDU = (uc*) malloc(PSDU_LEN * sizeof(uc));

//*************将接受到的psdu的16进制数据转成8位二进制数据********************************

		for (i = 0; i < length; i++) {
			for (i1 = 0; i1 < 8; i1++) {
				PSDU[i * 8 + i1] = (PSDU_MAC[i] & 0x80) >> 7;
				PSDU_MAC[i] = PSDU_MAC[i] << 1;
			}
		}

		printf("1:PSDU原始长度为：%d\n", PSDU_LEN);		//112个
		for (i = 0; i < PSDU_LEN; i++) {
			if (i % 8 == 0)
				printf("\n");
			printf("%d ", PSDU[i]);
		}
		printf("\n");

		/*****************************************************PLCP部分*******************************************************************/

//******************PLCP Header************************************************//
		PLCP_H = (uc*) calloc(PLCP_H_LEN, sizeof(uc));//为PLCP头信息分配空间并且初始化为0, PLCP_H_LEN=31
		PHYHeadGen(PLCP_H, rate, MAC_frame_body_len / 8, BurstMode,
				PSDU_S_seed); //产生物理层头
		/*for(i=0;i<31;i++)
		 printf("%d ",PLCP_H[i]);
		 printf("\n");*/

		PHYHeadCRCCoding(PLCP_H);  //产生物理层头数据的CRC校验位
		//printf("PLCP原始数据：\n");
		/*for(i=0;i<PLCP_H_LEN;i++) 
		 printf("%d ",PLCP_H[i]);
		 printf("\n");*/

		BCHEncoding_PHYH(PLCP_H);  //物理层头数据的BCH编码
		//printf("PLCP编码后的原始数据：\n");
		/*for(i=0;i<PLCP_H_LEN;i++)  //与rx的第140行输出相同
		 printf("%d ",PLCP_H[i]);
		 printf("\n");*/

		PLCP_H_Spread = Spreader(PLCP_H, PLCP_H_LEN, PLCP_H_SpreadFactor);//PLCP头信息码元扩展
		free(PLCP_H);  //释放不用的内存
		PLCP_H = NULL;   //将不用的指针制为空指针
		/*printf("PLCP码元扩展后：\n ");
		 for(i=0;i<PLCP_H_LEN*PLCP_H_SpreadFactor;i++)
		 printf("%d ",PLCP_H_Spread[i]);
		 printf("\n");*/

		Interleaver(PLCP_H_Spread, PLCP_H_SP_LEN, PLCP_H_SpreadFactor); //码元交织
		/*printf("PLCP码元交织后：\n ");
		 for(i=0;i<PLCP_H_LEN*PLCP_H_SpreadFactor;i++)
		 printf("%d ",PLCP_H_Spread[i]);
		 printf("\n");*/

		Scrambler(PLCP_H_Spread, PLCP_H_SP_LEN, PLCP_H_S_seed); //扰码
		/*printf("PLCP码元扰码后长度为：%d\n ", PLCP_H_SP_LEN);  //31 * 4
		 for(i=0;i<PLCP_H_SP_LEN;i++) printf("%d ",PLCP_H_Spread[i]);
		 printf("\n");*/

//******************PLCP Preamble************************************************
		PLCP = (uc*) malloc(PLCP_LEN * sizeof(uc));    //PLCP前导码 + 头信息
		for (i = 0; i < PLCP_LEN; i++) {
			if (i < PLCP_P_LEN)
				PLCP[i] = PLCP_P[i];
			else
				PLCP[i] = PLCP_H_Spread[i - PLCP_P_LEN];  //PLCP
		}
		free(PLCP_H_Spread);  //PLCP Header一直放在spread时开辟的动态空间里
		PLCP_H_Spread = NULL;
		/*printf("PLCP加前导码后长度为%d：\n ", PLCP_LEN);  //214 = 31*4 +90
		 for(i=0;i<PLCP_LEN;i++) printf("%d ",PLCP[i]);
		 printf("\n");*/

		PLCP_Mod = DPSK_Mod(PLCP, PLCP_LEN, PLCP_H_M); //PLCP调制(PLCP Header经历bch编码、扩频、交织、扰码一系列之后，再加上前导码，然后一起进行调制)
		free(PLCP);
		PLCP = NULL;
		m = (int) ceil(log(PLCP_H_M) / log(2)); //ceil函数的作用是求不小于给定实数的最小整数

		//printf("%d\n",m); // m=1

		PLCP_Mod_len = PLCP_LEN / m; //PLCP调制之后的长度 = 214

		/*printf("PLCP调制之后长度为：%d\n PLCP调制之后输出为：\n", PLCP_Mod_len);
		 for(i=0;i<PLCP_Mod_len;i++)
		 printf("%f+%fI  ",PLCP_Mod[i].re,PLCP_Mod[i].im);*/

		/********************************************PSDU部分，psdu利用mac空间，自己开辟的需要free*********************************************/

		printf("2:PSDU编码前的长度为：%d\n ", PSDU_LEN);
		printf("PSDU原始数据\n");  //共有208 = 26 * 8个
		for (i = 0; i < PSDU_LEN; i++) {
			if (i % 8 == 0)
				printf("\n");
			printf("%d ", PSDU[i]);
		}
		printf("\n");

		PSDU_Coded_Pad = BCHEncoding_PSDU(PSDU, PSDU_M); //用于PSDU的BCH编码

		printf("PSDU编码之后扩频之前的长度为：%d\n ", PSDU_LEN); //268
		printf("PSDU编码之后扩频之前数据为:\n");

		for (i = 0; i < PSDU_LEN; i++) {
			if (i % 8 == 0) {
				printf("\n");
			}
			printf("%d ", (int) PSDU_Coded_Pad[i]);
		}
		printf("\n");

		PSDU_Spread = Spreader(PSDU_Coded_Pad, PSDU_LEN, PSDU_SpreadFactor); //Spread
		free(PSDU_Coded_Pad);
		PSDU_Coded_Pad = NULL;

		/*printf("PSDU扩频之后长度为：%d\n ", PSDU_SP_LEN);  //1072 = 268 * 4
		 for(i=0;i<PSDU_SP_LEN;i++)
		 printf("%d ",PSDU_Spread[i]);
		 printf("\n");*/

		Interleaver(PSDU_Spread, PSDU_SP_LEN, PSDU_SpreadFactor); //interleave

		Scrambler(PSDU_Spread, PSDU_SP_LEN, PSDU_S_seed); //scramble
		/*printf("PSDU扰码之后长度为：%d\n ", PSDU_SP_LEN);
		 for(i=0;i<PSDU_SP_LEN;i++)
		 printf("%d ",PSDU_Spread[i]);//扰码之后长度不变，且依然放在PSDU_Spread[i]数组中,为 1072个01数据
		 printf("\n");*/

		PSDU_Mod = DPSK_Mod(PSDU_Spread, PSDU_SP_LEN, PSDU_M); //PSDU调制

		free(PSDU_Spread);
		PSDU_Spread = NULL;

		m = (int) ceil(log(PSDU_M) / log(2));
		PSDU_Mod_len = PSDU_SP_LEN / m; //PSDU调制之后的长度

		/*printf("PSDU调制之后长度为：%d\n PSDU调制之后输出为：\n", PSDU_Mod_len);  //1072，  其中m = 1
		 for(i=0;i<PSDU_Mod_len;i++)
		 printf("%f+%fI  ",PSDU_Mod[i].re,PSDU_Mod[i].im);
		 printf("\n");*/

//************************组合成PPDU********************************************************************************/       	
		Mod_len = PLCP_Mod_len + PSDU_Mod_len;

		PPDU_Mod = (complex*) malloc(Mod_len * sizeof(complex));

		for (i = 0; i < Mod_len; i++) {
			if (i < PLCP_Mod_len)
				PPDU_Mod[i] = PLCP_Mod[i];
			else
				PPDU_Mod[i] = PSDU_Mod[i - PLCP_Mod_len];
		}

		printf("PPDU调制之后长度为：%d\n PPDU调制之后输出为：\n", Mod_len); //1286
		for (i = 0; i < Mod_len; i++)
			printf("%f+%fI  ", PPDU_Mod[i].re, PPDU_Mod[i].im);
		printf("\n");

		free(PLCP_Mod);    //释放空间
		PLCP_Mod = NULL;
		free(PSDU_Mod);
		PSDU_Mod = NULL;

	}

	return (PPDU_Mod);

}

