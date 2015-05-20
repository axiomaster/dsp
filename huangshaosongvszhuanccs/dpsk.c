#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include"complex.h"
#define PI 3.1415926
#define uc unsigned char
/***************相位计算函数******************/
double CalcPhase(uc* BitVector,int M)
{
	double Phase;

	if(M==2)      //二进制的星座映射，调制阶数，频率和速率确定后即可确定调制阶数
	{
		switch(BitVector[0])
		{
		     case 0: Phase=PI/2;  break;//[0]case
		     case 1: Phase=-PI/2;  break;//[1]case
			 default:printf("映射失败！\n");
		}
	}

	else if(M==4)  //四进制的星座映射
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
			 default:printf("映射失败！\n");
		 }
	}
	else if(M==8) //八进制的星座映射
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
			 default:printf("映射失败！\n");
		 }
	}
	}
	return Phase;
}

/***********DPSK调制函数*****************/
//输入：待调制的数组，数组长度，调制阶数
//输出：已调制信号
complex* DPSK_Mod(uc* BitStream, int LEN, int M)
{
	int i,j,m1;
	uc **sub_BitStream;
	double *pha;
	complex Init_sym={0,1};
	complex *Data_Mod;
	m1=(int)ceil(log(M)/log(2));
	Data_Mod=(complex*)malloc(LEN/m1*sizeof(complex));//每一对符号的相对相位变化
	//sub_BitStream=(uc**)malloc((LEN/m1)*sizeof(uc*));//串并变换为二维数组
	for(j=0; j<LEN/m1; j++)
	{
      sub_BitStream[j]=(uc*)malloc(sizeof(uc)*m1);
	}
	pha=(double(*))malloc(LEN/m1*sizeof(double));

	//printf("%d\n", LEN/m1);//9920

	for(i=0;i<(LEN/m1);i++)    //串并变换
	{
		for(j=0;j<m1;j++)
		{
			sub_BitStream[i][j] = BitStream[i*m1+j];
		}
	}

	/*for(i=0;i<(LEN/m1);i++)    //串并变换
			{
				for(j=0;j<m1;j++)
				{
					printf("%d", sub_BitStream[i][j]);
				}
				printf("\n");
			}*/

	pha[0] = CalcPhase(sub_BitStream[0],M);  //对第一对数据进行相位运算
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


/************DPSK解调函数****************/
//输入：待处理的数组，解调之后的数组大小，调制指数
//输出：已解调数组的首地址
double* DPSK_DeMod_SoftInfo(complex* Data_Mod_re,int LEN,int M)
{
	int m1,i,j;
	int N;
	double* Data_DeMod=(double*)malloc(LEN*sizeof(double));
    complex Init_sym={0,1};
	complex *D_data;//差分之后检测之前的数据初始化（数据长度的设定可以修改）
    m1=(int)ceil(log(M)/log(2));
	N=LEN/m1;
	D_data=(complex*)malloc(N*sizeof(complex));
	D_data[0]=fmul(fconj(Init_sym),Data_Mod_re[0]);
	for(i=1;i<N;i++)
	{
		D_data[i]=fmul(fconj(Data_Mod_re[i-1]),Data_Mod_re[i]);  //差分之后的数据序列
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
/*******************软判决********************/
//输入：软判决信息，信息长度，调制阶数
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
