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
/*#define FrameNum 1   //�����趨�����֡��Ϊ1֡
#define PLCP_H_LEN 31
#define PLCP_H_SP_LEN (PLCP_H_LEN*PLCP_H_SpreadFactor)
#define PLCP_P_LEN 90
#define PLCP_LEN (PLCP_H_SP_LEN+PLCP_P_LEN) //��Ƶ���PLCP_header��PLCP Preamble�ĳ��Ⱥ�
#define PSDU_SP_LEN (PSDU_LEN*PSDU_SpreadFactor)
#define PPDU_LEN (PLCP_LEN+PSDU_LEN)
#define PI 3.1415926
#define uc unsigned char*/
extern complex FPGArx[2000];
//extern int PSDU_LEN;
extern int PSDU_Mod_len, PLCP_Mod_len;
extern Uint16 FPGA_length;

WORD PHYrx(complex *PPDU_Mod1) //�����ȫ�ָ�������FPGArx
{

//�Լ��趨��ֵ��ʵ����Ŀ����Ҫmac���߲�ȷ������
	int m;
	int ChannelNum=2; //�ŵ����,P187
	//int BurstMode=1; //�Ƿ�ͻ��ģʽ��1��0���Լ��趨��û�д�ͷ��Ϣ�еõ���λ������ֱ�Ӱ�����������ġ��Ƿ�mac��ȷ��֮�󼴿ɰ��ճ����ã�����������������������������������
	//int PSDU_S_seed = 0;//PSDU��scrambler seed��plcp header��ssһ������Channel number������Ȼ��ÿ�ζ�����,�����Ƚ����ʼ��Ϊ0�������ٸ����ŵ�����ż�Խ��и�ֵ
	int PLCP_H_SpreadFactor = 4; //��2360MHz-2483.5MHzƵ�ʷ�Χ,PLCPͷ����չ����ȡ4�����ն˵���֪�á�
	int PLCP_H_M = 2; ///��2360MHz-2483.5MHzƵ�ʷ�Χ,PLCP header�ĵ��ƽ���Ϊ2
	//int MAC_frame_body_len_uc = 0;//��mac���������ʵ�ʵ�macframebody���ֽ���
	//double Total_len;
	int  F_err=0;

	//�����ڲ��������Ҫ���Ա���
	//int *PSDU_LEN;//�Լ�д��
	//int FrameNum=1;
	//uc *PSDU;
	int i, j,PSDU_init_len;
	//int MAC_frame_body_len;
	int PLCP_H_S_seed;  //PSDU_M��ʾPSDU���ƽ��� 
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
	int temp = 0, PSDU_MAC_length;  //PSDU_MAC_lengthΪ����mac���16���Ƶ�����ĳ���


	if(ChannelNum%2==0)
	{
		PLCP_H_S_seed=0;
		//PLCP_P = PLCP_P1;
	}

	else
	{
		PLCP_H_S_seed=1;

	}


	for(j=0;j<FrameNum;j++)  //�˴��趨ֻ��һ֡����
	{

		MACFrameBodyLength_dec=0;
		//PSDU_S_seed=(PSDU_S_seed+1)%2;//ÿ֡�任1�Σ��ĵ� P185

		//MAC_frame_body_len = MAC_frame_body_len_uc * 8;//�����а�macframebody��ÿһλ��ŵ�һ���ֽ����ΪҪ�õ�λ��������Ҫ����8
		//MAC_frame_body_len_uc = length -9;
		//PSDU_LEN = (7+MAC_frame_body_len_uc)*8+16;//PSDU����=MAC֡������+ͷ���ݵĳ���+FCS*/

//���뷢�Ͷ˵õ��ĸ������飬��������Ĳ�����Ϊ��Ҫ�ĸ������飬��˲���Ҫר�ŵĳ���

//ͨ���������ó�PLCP��ͷ��Ϣ�����ɴ˵õ����PSDU����Ҫ�Ĳ���
/*---------------------------------���նˣ�PLCP�������--------------------------------------*/
		//Mod_len = PLCP_Mod_len + PSDU_Mod_len;
		//printf("\n");
		//printf("��fpga��������һ֡��FPGArx���鳤��Ϊ:%d\n",FPGA_length); //315
		/*for(i=0;i<FPGA_length;i++)
			printf("%f+%fI  ",PPDU_Mod1[i].re,PPDU_Mod1[i].im);
		printf("\n");*/
		//PLCP_Mod_R=(complex*)malloc(FPGA_length*sizeof(complex));
		PLCP_Mod_R = PPDU_Mod1;
		/*printf("\n");
		for(i=0;i<FPGA_length;i++)
			printf("%f+%fI  ",FPGArx[i].re,FPGArx[i].im);
		printf("\n");*/

		PLCP_DeMod_SoftInfo = DPSK_DeMod_SoftInfo(PLCP_Mod_R,PLCP_LEN,PLCP_H_M); //��������̬���������飩
		//printf("PLCP���֮�󳤶�Ϊ��%d\n PLCP���֮�����Ϊ��\n", PLCP_LEN);  //214��PLCP�ĳ���һֱ�����䣩
		/*printf("\n");
		printf("%d ", PLCP_LEN);
		printf("\n");
		for(i=0;i<PLCP_LEN;i++)
			printf("%d: %f ",i+1, PLCP_DeMod_SoftInfo[i]);
		printf("\n");*/

		PLCP_H_DeMod_SoftInfo = PLCP_DeMod_SoftInfo + PLCP_P_LEN;//ָ����Լӳ������ȣ���ʾָ�������ƶ���λ��
		//printf("PLCP header���֮�󳤶�Ϊ��%d\n PLCP header���֮�����Ϊ��\n", PLCP_H_SP_LEN);  //124=214-90
		/*printf("����ǰ��\n");
		for(i=0;i<PLCP_H_SP_LEN;i++)
			printf("%f ",PLCP_H_DeMod_SoftInfo[i]);
		printf("\n");*/

//**********PLCP header������һϵ�й��̣�����PLCP Preambleֻ�����˵��ƣ��������һϵ�еĽ⶯���������PLCP Header�ģ�������Ҫ��ָ��ָ��PLCP Header��Ȼ���ٽ������¶���************************************//

		Scrambler_Soft(PLCP_H_DeMod_SoftInfo, PLCP_H_SP_LEN, PLCP_H_S_seed, PLCP_H_M);//Descramble
		/*printf("���ź�\n");
		for(i=0;i<PLCP_H_SP_LEN;i++)
		printf("%f ",PLCP_H_DeMod_SoftInfo[i]);
		printf("\n");*/

		DeInterleaver(PLCP_H_DeMod_SoftInfo,PLCP_H_SP_LEN,PLCP_H_SpreadFactor); //DeInterleave
		/*printf("�⽻֯��\n");
		for(i=0;i<PLCP_H_SP_LEN;i++)
			printf("%f ",PLCP_H_DeMod_SoftInfo[i]);
		printf("\n");*/

		PLCP_H_DeSpread_SoftInfo=DeSpreader(PLCP_H_DeMod_SoftInfo,PLCP_H_SP_LEN,PLCP_H_SpreadFactor); //DeSpread
		//�˴���̬�����˽���֮��������ڴ棬����֮ǰ��Ҫ�ͷ�
		/*printf("������\n");
		for(i=0; i<PLCP_H_SP_LEN / PLCP_H_SpreadFactor; i++)
			printf("%f ",PLCP_H_DeSpread_SoftInfo[i]);
		printf("\n");*/

		free(PLCP_DeMod_SoftInfo);
		PLCP_DeMod_SoftInfo=NULL;

		PLCP_H_Dec=DPSK_DeMod_SoftDec(PLCP_H_DeSpread_SoftInfo,PLCP_H_LEN,PLCP_H_M); //PLCPͷ��Ϣ�о�

		//printf("PLCP header���������ǰ��\n");  //PLCP_H_LEN = 31
		printf("\n");
		for(i=0;i<PLCP_H_LEN;i++)
		printf("%d ",PLCP_H_Dec[i]);
		printf("\n");

		free(PLCP_H_DeSpread_SoftInfo);  //�ͷſռ䲢���øÿռ�ָ��Ϊ��
		PLCP_H_DeSpread_SoftInfo=NULL;
		
		PLCP_H_Decoded= BCHDecoding_PHYH(PLCP_H_Dec);   //PHY_H��BCH����
		free(PLCP_H_Dec);
		PLCP_H_Dec=NULL;

		printf("PLCP header����������\n");  //�������19λ���ǶԵ�VVVVVVVVVVVVV
		for(i=0;i<19;i++)
		printf("%d ",PLCP_H_Decoded[i]);
		printf("\n");

/*---------------------------------���նˣ�PSDU�������--------------------------------------*/

		CRCCheck_resulst = PHYHeadCRCCheck(PLCP_H_Decoded,19); //�����ͷ���ݵ�CRCУ��λ����Ӧ�ڷ��Ͷ˵�HCSУ��
		if(CRCCheck_resulst == 0)
		{
			F_err++;
			printf("ͷ����ʧ�ܣ���֡����\n");//У��ʧ���������

			continue;
		}
		else  //У��ɹ���������У�������κζ���
		{
			//Total_len += (7+MAC_frame_body_len)*8+16;
			rate_dec = PLCP_H_Decoded[0]*4 + PLCP_H_Decoded[1]*2 + PLCP_H_Decoded[2];
		}

		//printf("PLCP header���֮��õ�������Ϊ��%d\n", rate_dec);  //0,������Ϊ121

		switch(rate_dec)//��2360MHz-24830.5MHzƵ�ʷ�Χ��  //��ͷ��Ϣ�Ľ������еõ�PSDU�����ʣ�rate_dec
		{
		case 0: //[0 0 0]
			PSDU_SpreadFactor_R=4;
			PSDU_M_R=2; //PSDU���ƽ���
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
			PSDU_SpreadFactor_R=100; //��ֵ�϶������ֵ
			PSDU_M_R=100;
			break;
		}
		flag=1;
		for(i=4;i<12;i++)  //��ͷ��Ϣ�Ľ������еõ�PSDU�ĳ��ȣ�MACFrameBodyLength_dec
		{
			MACFrameBodyLength_dec += PLCP_H_Decoded[i]*flag;
			flag=flag*2;
		}
		//printf("PSDU��MACFrameBody����Ϊ��%d\n", MACFrameBodyLength_dec);//100-9=91

		PSDU_S_seed_R = PLCP_H_Decoded[14];  //��ͷ��Ϣ�Ľ������еõ�PSDU���������ӣ�PSDU_S_seed_R
		//printf("PSDU��MACFrameBody��������Ϊ��%d\n", PSDU_S_seed_R);//1

		free(PLCP_H_Decoded);
		PLCP_H_Decoded=NULL;

		//printf("\n%d ", PLCP_Mod_len);
		m=(int)ceil(log(PLCP_H_M)/log(2)); //ceil��������������С�ڸ���ʵ������С����
    	//printf("%d\n",m); // m=1
		PLCP_Mod_len = PLCP_LEN/m; //PLCP����֮��ĳ��� = 214

		PSDU_Mod_R = PPDU_Mod1 + PLCP_Mod_len; //PSDU_Mod_R��PPDU_Mod1�Ǹ������͵�ָ�룬PLCP_Mod_len�ǳ���
		/*printf("PSDU���֮ǰ������Ϊ:\n");
		for(i=0;i<416;i++)
			printf("%f+%fI  ",PSDU_Mod_R[i].re,PSDU_Mod_R[i].im);
		printf("\n");*/

		PSDU_init_len = 72 + MACFrameBodyLength_dec * 8;//PSDUԭʼ����

		//printf("PSDU�ĳ���Ϊ��%d\n", PSDU_init_len);  //  208=136+72 ��ΪMACFrameBodyLength_dec=136   

		PSDU_DeMod_len = PSDUcodelen(PSDU_init_len,PSDU_M_R,PSDU_SpreadFactor_R);
		//printf("PSDU�Ľ��ն˵ĵ���ָ����%d\n", PSDU_M_R);//2
		//printf("PSDU�Ľ��ն˵���Ƶָ����%d\n", PSDU_SpreadFactor_R);//4
		//printf("ԭʼ��PSDU�ĵ���֮�󣨼����֮ǰ���ĳ���Ϊ��%d\n", PSDU_DeMod_len);//592,��Ϊ���Ͷ˺ͽ��ն��Ƿֿ��ģ�������Ҫ���¼���
		/*printf("PSDU���֮ǰ������Ϊ:\n");
		for(i=0;i<PSDU_DeMod_len;i++)
			printf("%f+%fI  ",PSDU_Mod_R[i].re,PSDU_Mod_R[i].im);
		printf("\n");*/

		PSDU_DeMod_SoftInfo = DPSK_DeMod_SoftInfo(PSDU_Mod_R,PSDU_DeMod_len,PSDU_M_R); //PSDU�������������Ŀ������ڴ棬������Ϊԭ������ȫ�֣����Բ����ͷţ�
		/*printf("PSDU���֮�����֮ǰ�ĳ���Ϊ��%d\n, ����Ϊ:\n",PSDU_DeMod_len);
		for(i=0; i<PSDU_DeMod_len; i++)
			printf("%f ",PSDU_DeMod_SoftInfo[i]);
		printf("\n");*/

		Scrambler_Soft(PSDU_DeMod_SoftInfo,PSDU_DeMod_len,PSDU_S_seed_R,PSDU_M_R);//PSDU����
		DeInterleaver(PSDU_DeMod_SoftInfo,PSDU_DeMod_len,PSDU_SpreadFactor_R); //PSDU�⽻֯


		PSDU_DeSpread_SoftInfo = DeSpreader(PSDU_DeMod_SoftInfo,PSDU_DeMod_len,PSDU_SpreadFactor_R); //PSDU����


		free(PSDU_DeMod_SoftInfo);
		PSDU_DeMod_SoftInfo=NULL;

		PSDU_Dec = DPSK_DeMod_SoftDec(PSDU_DeSpread_SoftInfo,PSDU_DeMod_len/PSDU_SpreadFactor_R,PSDU_M_R); //PSDU�о�

		//printf("ԭʼ��PSDU�Ľ���֮�󣨼�����֮ǰ���ĳ���Ϊ��%d\n", PSDU_DeMod_len/PSDU_SpreadFactor_R);// 148 = 592/4
		/*printf("PSDU����֮ǰ����Ϊ:\n");
		for(i=0; i<PSDU_DeMod_len/PSDU_SpreadFactor_R; i++)
			printf("%d ",PSDU_Dec[i]);
		printf("\n");*/

		PSDU_Decode = BCHDecoding_PSDU(PSDU_Dec,PSDU_DeMod_len/PSDU_SpreadFactor_R,PSDU_M_R);//PSDU����


		free(PSDU_DeSpread_SoftInfo);
		PSDU_DeSpread_SoftInfo = NULL;
		//free(FPGArx);
		//FPGArx = NULL;
		free(PSDU_Dec);
		PSDU_Dec = NULL;

		printf("PSDU����Ϊ��%d\n", PSDU_init_len);
		printf("PSDU���ն�:\n");
		for(i=0; i<PSDU_init_len; i++)
			printf("%d ",PSDU_Decode[i]);
		printf("\n");

//******************2����ת��16����******************************************//
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
	
	  if(F_err != 0)  //������������ʾ����֡У��ɹ������ۼ�
		  printf("ͷУ��ʧ�ܣ���֡��Ϊ%d\n",F_err);

	  //��̬���鸳ֵ����̬����
	  for(i=0;i<PSDU_MAC_length;i++)
	  {
		  mac_RxBuffer[i] = PSDU_MAC[i];

	  }

	  free(PSDU_MAC);
	  PSDU_MAC = NULL;

	}
	return ((WORD)PSDU_MAC_length);
}
