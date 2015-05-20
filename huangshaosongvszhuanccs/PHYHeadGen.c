//物理层头信息：PHY_Header
//输入：物理层头信息数组地址，传输速率，MAC帧体长度，突发模式，扰码种子
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define uc unsigned char
/*--------------------产生物理层头函数------------------------*/
void PHYHeadGen(uc *PHY_H, int rate, int len, int BM, int S)
{
  int i;
  uc reserveBit=0;
  uc len_code[8]={0}; 
  uc rate_code[3]={0};
  switch(rate)
   { 
      case 121:;                        
	    	break;  //121为000
      case 242:rate_code[0]=1;       
		    break;  //242为100
      case 485:rate_code[1]=1;  
	    	break;  //485为010
      case 971:rate_code[0]=1;
		       rate_code[1]=1; 
		    break;  //971为110
	  default:printf("No matching rate! Error in PHYHeadGen funtion.\n");
		    break;
	}
    //MAC帧体长度转化为二进制，二进制数从PHY_H的4~11位依次为1~8位，所以00010100表示的是len=40
   	for(i=0;i<8;i++)
   {
    len_code[i]=len%2;
    len=len/2;
   }
   for(i=0;i<15;i++)
   {
   if(i<3) PHY_H[i]=rate_code[i];
   else if(i==3||i==12) PHY_H[i]=reserveBit;//预留比特
   else if(i>3&&i<12) PHY_H[i]=len_code[i-4];
   else if(i==13) PHY_H[i]=BM;
   else if(i==14) PHY_H[i]=S;
   }
}

