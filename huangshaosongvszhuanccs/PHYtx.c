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

/*#define FrameNum 1   //�����趨�����֡��Ϊ1֡
 #define PLCP_H_LEN 31
 #define PLCP_H_SP_LEN (PLCP_H_LEN*PLCP_H_SpreadFactor)
 #define PLCP_P_LEN 90
 #define PLCP_LEN (PLCP_H_SP_LEN+PLCP_P_LEN) //��Ƶ���PLCP_header��PLCP Preamble�ĳ��Ⱥ�
 #define PSDU_SP_LEN (PSDU_LEN*PSDU_SpreadFactor)
 #define PPDU_LEN (PLCP_LEN+PSDU_LEN)
 #define PI 3.1415926
 #define uc unsigned char*/

extern int PSDU_LEN;
extern int PSDU_Mod_len, PLCP_Mod_len;

complex *PHYtx(unsigned char *PSDU_MAC, uc length, int rate) {

	/**************************************************�����������*******************************************************/

	//�Լ��趨��ֵ��ʵ����Ŀ����Ҫmac���߲�ȷ������
	int ChannelNum = 1; //�ŵ����,P187
	int BurstMode = 1; //�Ƿ�ͻ��ģʽ��1��0���Լ��趨
	int PSDU_S_seed = 0; //�����ĳ�ʼ����PSDU��scrambler seed��plcp header��ssһ������Channel number������Ȼ��ÿ�ζ�����,�����Ƚ����ʼ��Ϊ0�������ٸ����ŵ�����ż�Խ��и�ֵ
	int PLCP_H_SpreadFactor = 4; //��2360MHz-2483.5MHzƵ�ʷ�Χ,PLCPͷ����չ����ȡ4�����ն˵���֪�á�
	int PLCP_H_M = 2; ///��2360MHz-2483.5MHzƵ�ʷ�Χ,PLCP header�ĵ��ƽ���Ϊ2
	int MAC_frame_body_len_uc = 0; //�����ĳ�ʼ������mac���������ʵ�ʵ�macframebody���ֽ���

	//�����ڲ��������Ҫ���Ա���
	uc *PSDU;
	int i, j, m, i1;
	int MAC_frame_body_len, PLCP_H_S_seed, PSDU_M;  //PSDU_M��ʾPSDU���ƽ���
	uc *PLCP_H;
	uc *PLCP_H_Spread, *PLCP_P, *PLCP;
	complex *PLCP_Mod, *PSDU_Mod;
	uc *PSDU_Spread, *PSDU_Coded_Pad;
	int PSDU_SpreadFactor;  //rate_dec, PSDU_SpreadFactor_R, PSDU_M_Rָ���ǽ��ն˵�
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

	switch (rate) //��2360MHz-2483.5MHzƵ�ʷ�Χ��
	{
	case 121:
		PSDU_SpreadFactor = 4;
		PSDU_M = 2; //PSDU���ƽ���
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
	for (j = 0; j < FrameNum; j++)  //�˴��趨ֻ��һ֡����
			{

		/********************************PSDUԭʼ����(����ģ���������)**********************************************************/
		//MACFrameBodyLength_dec=0;
		PSDU_S_seed = (PSDU_S_seed + 1) % 2;  //ÿ֡�任1�Σ��ĵ� P185

		MAC_frame_body_len_uc = length - 9;
		MAC_frame_body_len = MAC_frame_body_len_uc * 8; //�����а�macframebody��ÿһλ��ŵ�һ���ֽ����ΪҪ�õ�λ��������Ҫ����8

		PSDU_LEN = (7 + MAC_frame_body_len_uc) * 8 + 16; //PSDU����=MAC֡������+ͷ���ݵĳ���+FCS
		//PSDU_LEN = (7+MAC_frame_body_len_uc)*16+32;//PSDU����=MAC֡������+ͷ���ݵĳ���+FCS
		PSDU = (uc*) malloc(PSDU_LEN * sizeof(uc));

//*************�����ܵ���psdu��16��������ת��8λ����������********************************

		for (i = 0; i < length; i++) {
			for (i1 = 0; i1 < 8; i1++) {
				PSDU[i * 8 + i1] = (PSDU_MAC[i] & 0x80) >> 7;
				PSDU_MAC[i] = PSDU_MAC[i] << 1;
			}
		}

		printf("1:PSDUԭʼ����Ϊ��%d\n", PSDU_LEN);		//112��
		for (i = 0; i < PSDU_LEN; i++) {
			if (i % 8 == 0)
				printf("\n");
			printf("%d ", PSDU[i]);
		}
		printf("\n");

		/*****************************************************PLCP����*******************************************************************/

//******************PLCP Header************************************************//
		PLCP_H = (uc*) calloc(PLCP_H_LEN, sizeof(uc));//ΪPLCPͷ��Ϣ����ռ䲢�ҳ�ʼ��Ϊ0, PLCP_H_LEN=31
		PHYHeadGen(PLCP_H, rate, MAC_frame_body_len / 8, BurstMode,
				PSDU_S_seed); //���������ͷ
		/*for(i=0;i<31;i++)
		 printf("%d ",PLCP_H[i]);
		 printf("\n");*/

		PHYHeadCRCCoding(PLCP_H);  //���������ͷ���ݵ�CRCУ��λ
		//printf("PLCPԭʼ���ݣ�\n");
		/*for(i=0;i<PLCP_H_LEN;i++) 
		 printf("%d ",PLCP_H[i]);
		 printf("\n");*/

		BCHEncoding_PHYH(PLCP_H);  //�����ͷ���ݵ�BCH����
		//printf("PLCP������ԭʼ���ݣ�\n");
		/*for(i=0;i<PLCP_H_LEN;i++)  //��rx�ĵ�140�������ͬ
		 printf("%d ",PLCP_H[i]);
		 printf("\n");*/

		PLCP_H_Spread = Spreader(PLCP_H, PLCP_H_LEN, PLCP_H_SpreadFactor);//PLCPͷ��Ϣ��Ԫ��չ
		free(PLCP_H);  //�ͷŲ��õ��ڴ�
		PLCP_H = NULL;   //�����õ�ָ����Ϊ��ָ��
		/*printf("PLCP��Ԫ��չ��\n ");
		 for(i=0;i<PLCP_H_LEN*PLCP_H_SpreadFactor;i++)
		 printf("%d ",PLCP_H_Spread[i]);
		 printf("\n");*/

		Interleaver(PLCP_H_Spread, PLCP_H_SP_LEN, PLCP_H_SpreadFactor); //��Ԫ��֯
		/*printf("PLCP��Ԫ��֯��\n ");
		 for(i=0;i<PLCP_H_LEN*PLCP_H_SpreadFactor;i++)
		 printf("%d ",PLCP_H_Spread[i]);
		 printf("\n");*/

		Scrambler(PLCP_H_Spread, PLCP_H_SP_LEN, PLCP_H_S_seed); //����
		/*printf("PLCP��Ԫ����󳤶�Ϊ��%d\n ", PLCP_H_SP_LEN);  //31 * 4
		 for(i=0;i<PLCP_H_SP_LEN;i++) printf("%d ",PLCP_H_Spread[i]);
		 printf("\n");*/

//******************PLCP Preamble************************************************
		PLCP = (uc*) malloc(PLCP_LEN * sizeof(uc));    //PLCPǰ���� + ͷ��Ϣ
		for (i = 0; i < PLCP_LEN; i++) {
			if (i < PLCP_P_LEN)
				PLCP[i] = PLCP_P[i];
			else
				PLCP[i] = PLCP_H_Spread[i - PLCP_P_LEN];  //PLCP
		}
		free(PLCP_H_Spread);  //PLCP Headerһֱ����spreadʱ���ٵĶ�̬�ռ���
		PLCP_H_Spread = NULL;
		/*printf("PLCP��ǰ����󳤶�Ϊ%d��\n ", PLCP_LEN);  //214 = 31*4 +90
		 for(i=0;i<PLCP_LEN;i++) printf("%d ",PLCP[i]);
		 printf("\n");*/

		PLCP_Mod = DPSK_Mod(PLCP, PLCP_LEN, PLCP_H_M); //PLCP����(PLCP Header����bch���롢��Ƶ����֯������һϵ��֮���ټ���ǰ���룬Ȼ��һ����е���)
		free(PLCP);
		PLCP = NULL;
		m = (int) ceil(log(PLCP_H_M) / log(2)); //ceil��������������С�ڸ���ʵ������С����

		//printf("%d\n",m); // m=1

		PLCP_Mod_len = PLCP_LEN / m; //PLCP����֮��ĳ��� = 214

		/*printf("PLCP����֮�󳤶�Ϊ��%d\n PLCP����֮�����Ϊ��\n", PLCP_Mod_len);
		 for(i=0;i<PLCP_Mod_len;i++)
		 printf("%f+%fI  ",PLCP_Mod[i].re,PLCP_Mod[i].im);*/

		/********************************************PSDU���֣�psdu����mac�ռ䣬�Լ����ٵ���Ҫfree*********************************************/

		printf("2:PSDU����ǰ�ĳ���Ϊ��%d\n ", PSDU_LEN);
		printf("PSDUԭʼ����\n");  //����208 = 26 * 8��
		for (i = 0; i < PSDU_LEN; i++) {
			if (i % 8 == 0)
				printf("\n");
			printf("%d ", PSDU[i]);
		}
		printf("\n");

		PSDU_Coded_Pad = BCHEncoding_PSDU(PSDU, PSDU_M); //����PSDU��BCH����

		printf("PSDU����֮����Ƶ֮ǰ�ĳ���Ϊ��%d\n ", PSDU_LEN); //268
		printf("PSDU����֮����Ƶ֮ǰ����Ϊ:\n");

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

		/*printf("PSDU��Ƶ֮�󳤶�Ϊ��%d\n ", PSDU_SP_LEN);  //1072 = 268 * 4
		 for(i=0;i<PSDU_SP_LEN;i++)
		 printf("%d ",PSDU_Spread[i]);
		 printf("\n");*/

		Interleaver(PSDU_Spread, PSDU_SP_LEN, PSDU_SpreadFactor); //interleave

		Scrambler(PSDU_Spread, PSDU_SP_LEN, PSDU_S_seed); //scramble
		/*printf("PSDU����֮�󳤶�Ϊ��%d\n ", PSDU_SP_LEN);
		 for(i=0;i<PSDU_SP_LEN;i++)
		 printf("%d ",PSDU_Spread[i]);//����֮�󳤶Ȳ��䣬����Ȼ����PSDU_Spread[i]������,Ϊ 1072��01����
		 printf("\n");*/

		PSDU_Mod = DPSK_Mod(PSDU_Spread, PSDU_SP_LEN, PSDU_M); //PSDU����

		free(PSDU_Spread);
		PSDU_Spread = NULL;

		m = (int) ceil(log(PSDU_M) / log(2));
		PSDU_Mod_len = PSDU_SP_LEN / m; //PSDU����֮��ĳ���

		/*printf("PSDU����֮�󳤶�Ϊ��%d\n PSDU����֮�����Ϊ��\n", PSDU_Mod_len);  //1072��  ����m = 1
		 for(i=0;i<PSDU_Mod_len;i++)
		 printf("%f+%fI  ",PSDU_Mod[i].re,PSDU_Mod[i].im);
		 printf("\n");*/

//************************��ϳ�PPDU********************************************************************************/       	
		Mod_len = PLCP_Mod_len + PSDU_Mod_len;

		PPDU_Mod = (complex*) malloc(Mod_len * sizeof(complex));

		for (i = 0; i < Mod_len; i++) {
			if (i < PLCP_Mod_len)
				PPDU_Mod[i] = PLCP_Mod[i];
			else
				PPDU_Mod[i] = PSDU_Mod[i - PLCP_Mod_len];
		}

		printf("PPDU����֮�󳤶�Ϊ��%d\n PPDU����֮�����Ϊ��\n", Mod_len); //1286
		for (i = 0; i < Mod_len; i++)
			printf("%f+%fI  ", PPDU_Mod[i].re, PPDU_Mod[i].im);
		printf("\n");

		free(PLCP_Mod);    //�ͷſռ�
		PLCP_Mod = NULL;
		free(PSDU_Mod);
		PSDU_Mod = NULL;

	}

	return (PPDU_Mod);

}

