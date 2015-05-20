//���룺������������׵�ַ�����鳤�ȣ���չ����
//���������չ������׵�ַ
#ifdef DEBUG
#include <stdio.h>
#endif
#include<stdlib.h>
#define uc unsigned char

uc* Spreader(uc* data,int len,int factor)
{
uc *dataSpread;
int i,j,k,n=0;
dataSpread=(uc*)malloc(sizeof(uc)*len*factor);
   if (factor==4||factor==2) //��չ����Ϊ2����4
   { 
     for(k=0;k<len;k++)
	 {
       for(j=0;j<factor;j++)
	   {
    	 dataSpread[n+j]=data[k];
	   }
        n+=factor;
		if(n>=len*factor) break;
	 }
   }
   else
    for(i=0;i<len*factor;i++) 
		dataSpread[i]=data[i]; //��չ����Ϊ1
	return dataSpread;  
}


/*��Ԫ��������*/
//���룺�����������飬���鳤�ȣ���չ����
double* DeSpreader(double* data,int len,int factor)
{
	int N=len/factor,n,k;
	double *dataDeSpread=(double*)malloc(sizeof(double)*N);
	if(4==factor)
	{
		n=0;
		for(k=0;k<N;k++)
			{
				dataDeSpread[k]=(data[n]+data[n+1]+data[n+2]+data[n+3])/4;
				n=n+factor;
			}
	}
	else if(2==factor)
	{
		n=0;
		for(k=0;k<N;k++)
		{
			dataDeSpread[k]=(data[n]+data[n+1])/2;
			n=n+factor;
		}
	}
	else if(1==factor)
		for(k=0;k<N;k++)
		dataDeSpread[k]=data[k];
#ifdef DEBUG
	else
		printf("Error DeSpread!!");
#endif
	return dataDeSpread;
}



