#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include"complex.h"
#define PI 3.1415926
#define uc unsigned char
/***************��λ���㺯��******************/
double CalcPhase(uc* BitVector,int M)
{
	double Phase;

	if(M==2)      //�����Ƶ�����ӳ�䣬���ƽ�����Ƶ�ʺ�����ȷ���󼴿�ȷ�����ƽ���
	{
		switch(BitVector[0])
		{
		     case 0: Phase=PI/2;  break;//[0]case
		     case 1: Phase=-PI/2;  break;//[1]case
			 default:printf("ӳ��ʧ�ܣ�\n");
		}
	}

	else if(M==4)  //�Ľ��Ƶ�����ӳ��
	{
        switch(BitVector[0])
		 {
		     case 0:switch(BitVector[1])
					{
			          case 0:Phase=PI/4;break;//[0,0]case
                      case 1:Phase=PI*3/4;break;//[0,1]case
					}break;
             case 1:switch(BitVector[1])
					{
				     case 0:Phase=-PI/4;break;//[1,0]
                     case 1:Phase=-PI*3/4;break;//[1,1]
					}break;
			 default:printf("ӳ��ʧ�ܣ�\n");
		 }
	}
	else if(M==8) //�˽��Ƶ�����ӳ��
	{
        switch(BitVector[0])
		 {
		     case 0:switch(BitVector[1])
					{
			          case 0:switch(BitVector[2])
							 {
					           case 0:Phase=PI/8;break;//[0,0,0]
							   case 1:Phase=3*PI/8;break;//[0,0,1]
							 }break;
                      case 1:switch(BitVector[2])
							 {
					           case 0:Phase=7*PI/8;break;//[0,1,0]
							   case 1:Phase=5*PI/8;break;//[0,1,1]
							 }break;
					}break;
             case 1:switch(BitVector[1])
					{
			          case 0:switch(BitVector[2])
							 {
					           case 0:Phase=15*PI/8;break;//[1,0,0]
							   case 1:Phase=13*PI/8;break;//[1,0,1]
							 }break;
                      case 1:switch(BitVector[2])
							 {
					           case 0:Phase=9*PI/8;break;//[1,1,0]
							   case 1:Phase=11*PI/8;break;//[1,1,1]
							 }break;
			 default:printf("ӳ��ʧ�ܣ�\n");
		 }
	}
	}
	return Phase;
}

/***********DPSK���ƺ���*****************/
//���룺�����Ƶ����飬���鳤�ȣ����ƽ���
//������ѵ����ź�
complex* DPSK_Mod(uc* BitStream, int LEN, int M)
{
	int i,j,m1;
	uc **sub_BitStream;
	double *pha;
	complex Init_sym={0,1};
	complex *Data_Mod;
	m1=(int)ceil(log(M)/log(2));
	Data_Mod=(complex*)malloc(LEN/m1*sizeof(complex));//ÿһ�Է��ŵ������λ�仯
	//sub_BitStream=(uc**)malloc((LEN/m1)*sizeof(uc*));//�����任Ϊ��ά����
	for(j=0; j<LEN/m1; j++)
	{
      sub_BitStream[j]=(uc*)malloc(sizeof(uc)*m1);
	}
	pha=(double(*))malloc(LEN/m1*sizeof(double));

	//printf("%d\n", LEN/m1);//9920

	for(i=0;i<(LEN/m1);i++)    //�����任
	{
		for(j=0;j<m1;j++)
		{
			sub_BitStream[i][j] = BitStream[i*m1+j];
		}
	}

	/*for(i=0;i<(LEN/m1);i++)    //�����任
			{
				for(j=0;j<m1;j++)
				{
					printf("%d", sub_BitStream[i][j]);
				}
				printf("\n");
			}*/

	pha[0] = CalcPhase(sub_BitStream[0],M);  //�Ե�һ�����ݽ�����λ����
    Data_Mod[0] = fmul(Init_sym,fget(pha[0]));  
    for(i=1;i<(LEN/m1);i++)
	{
		pha[i]=CalcPhase(sub_BitStream[i],M);
        Data_Mod[i]=fmul(Data_Mod[i-1],fget(pha[i]));
	}
	for(j=0;j<LEN/m1;j++)
	{
      free(sub_BitStream[j]);
	}
	free(sub_BitStream);
	free(pha);
    //for(i=0;i<10;i++)
		//printf("%f+%fI  ",Data_Mod[i].re,Data_Mod[i].im);
	//printf("\n");
	return Data_Mod;

    //Data_Mod=Data_Mod;  
	//for(i=0;i<LEN/m1;i++) printf("%f",Data_Mod[i]);
   
}


/************DPSK�������****************/
//���룺����������飬���֮��������С������ָ��
//������ѽ��������׵�ַ
double* DPSK_DeMod_SoftInfo(complex* Data_Mod_re,int LEN,int M)
{
	int m1,i,j;
	int N;
	double* Data_DeMod=(double*)malloc(LEN*sizeof(double));
    complex Init_sym={0,1};
	complex *D_data;//���֮����֮ǰ�����ݳ�ʼ�������ݳ��ȵ��趨�����޸ģ�
    m1=(int)ceil(log(M)/log(2));
	N=LEN/m1;
	D_data=(complex*)malloc(N*sizeof(complex));
	D_data[0]=fmul(fconj(Init_sym),Data_Mod_re[0]);
	for(i=1;i<N;i++)
	{
		D_data[i]=fmul(fconj(Data_Mod_re[i-1]),Data_Mod_re[i]);  //���֮�����������
	}
	for(i=0,j=0;i<LEN;i+=m1)
	{
		if(1<=m1)
		Data_DeMod[i]=D_data[j].im;  
		if(2<=m1)
		Data_DeMod[i+1]=D_data[j].re;
		if(3<=m1)
        Data_DeMod[i+2]=fabs(D_data[j].im);
		j++;
	}
	free(D_data);
	if(D_data!=NULL)
		D_data=NULL;
	return Data_DeMod;   	
}
/*******************���о�********************/
//���룺���о���Ϣ����Ϣ���ȣ����ƽ���
uc* DPSK_DeMod_SoftDec(double* D_ang,int len,int M)
{
	int m1,i;
	uc *Data_DeMod_softDec=(uc*)malloc(len*sizeof(uc));
	m1=(int)(log(M)/log(2));
	for(i=0;i<len;i++) //2/4
		Data_DeMod_softDec[i]=D_ang[i]<0;
	if(3==m1) //8
	{
		for(i=2;i<len;i+=3)
			Data_DeMod_softDec[i]=D_ang[i]>sqrt(2)/2;
	}
	return Data_DeMod_softDec; 
}
