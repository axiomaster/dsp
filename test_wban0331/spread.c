//输入：待处理数组的首地址，数组长度，扩展因子
//输出：已扩展数组的首地址
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
   if (factor==4||factor==2) //扩展因子为2或者4
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
		dataSpread[i]=data[i]; //扩展因子为1
	return dataSpread;  
}


/*码元解扩程序*/
//输入：带解扩的数组，数组长度，扩展因子
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



