//�����ͷ��Ϣ��PHY_Header
//���룺�����ͷ��Ϣ�����ַ���������ʣ�MAC֡�峤�ȣ�ͻ��ģʽ����������
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define uc unsigned char
/*--------------------���������ͷ����------------------------*/
void PHYHeadGen(uc *PHY_H, int rate, int len, int BM, int S)
{
  int i;
  uc reserveBit=0;
  uc len_code[8]={0}; 
  uc rate_code[3]={0};
  switch(rate)
   { 
      case 121:;                        
	    	break;  //121Ϊ000
      case 242:rate_code[0]=1;       
		    break;  //242Ϊ100
      case 485:rate_code[1]=1;  
	    	break;  //485Ϊ010
      case 971:rate_code[0]=1;
		       rate_code[1]=1; 
		    break;  //971Ϊ110
	  default:printf("No matching rate! Error in PHYHeadGen funtion.\n");
		    break;
	}
    //MAC֡�峤��ת��Ϊ�����ƣ�����������PHY_H��4~11λ����Ϊ1~8λ������00010100��ʾ����len=40
   	for(i=0;i<8;i++)
   {
    len_code[i]=len%2;
    len=len/2;
   }
   for(i=0;i<15;i++)
   {
   if(i<3) PHY_H[i]=rate_code[i];
   else if(i==3||i==12) PHY_H[i]=reserveBit;//Ԥ������
   else if(i>3&&i<12) PHY_H[i]=len_code[i-4];
   else if(i==13) PHY_H[i]=BM;
   else if(i==14) PHY_H[i]=S;
   }
}

