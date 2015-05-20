//码元交织
//输入：待处理的数组首地址，数组长度，扩展因子
#include<stdlib.h>
#define uc unsigned char

void Interleaver(uc* data,int N,int factor)
{
int N_total,k_start;
int k,j,n,m;
uc *temp;
temp=(uc*)malloc(sizeof(uc)*3*factor);
 if(factor==2||factor==4)//交织
 {
      N_total=N/factor; //计算N_total
      if(N_total%2==0)  k_start=0;
      else
	  {
         k_start=3*factor;
         for(k=0;k<3*factor;k++)
		    temp[k]=data[k];
         for(n=0;n<3*factor;n++) 
			data[n]=temp[factor*(n%3)+(n/3)];//交织码
	  }
   for(m=k_start;m<N;m+=2*factor)
   {
	   for(j=0;j<2*factor;j++)  
		   temp[j]=data[j+m];
	   for(n=0;n<=2*factor-1;n++) 
		   data[m+n]=temp[factor*(n%2)+(n/2)];
   }
 }
	free(temp);
}

//解交织
//输入：待处理数组首地址，扩展因子
void DeInterleaver(double* data,int N,int factor)
{
int N_total,k_start;
int k,j,n,m;
double *temp;
temp=(double*)malloc(sizeof(double)*3*factor);
 if(factor==2||factor==4)//交织
 {
      N_total=N/factor; //计算N_total
      if(N_total%2==0)  k_start=0;
      else
	  {
         k_start=3*factor;
         for(k=0;k<3*factor;k++)
		    temp[k]=data[k];
         for(n=0;n<3*factor;n++) 
			data[factor*(n%3)+(n/3)]=temp[n];//交织码
	  }
   for(m=k_start;m<N;m+=2*factor)
   {
	   for(j=0;j<2*factor;j++)  
		   temp[j]=data[j+m];
	   for(n=0;n<=2*factor-1;n++) 
		   data[m+factor*(n%2)+(n/2)]=temp[n];
   }
 }
	free(temp);
}
