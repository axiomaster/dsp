#ifdef DEBUG
#include <stdio.h>
#endif
#include<stdlib.h>
#include<math.h>
#define uc unsigned char
/*扰码*/
//输入：待处理的数组指针，数组的大小，扰码种子
void Scrambler(uc* data,int N,int S_seed)
{
	int i,j;
	uc temp1,temp2,*reg;
	uc reg1[]={0,0,1,0,1,1,1,1,0,0,1,1,0,1};
	uc reg2[]={0,0,0,0,0,0,0,1,0,0,1,1,1,1};
	if(S_seed==0)
		reg=reg1;
	else
		reg=reg2;
	for(i=0;i<N;i++)
	{
		temp1=data[i];
		temp2=reg[13]^(reg[12]^(reg[1]^reg[11]));
		data[i]=temp1^temp2;
		for(j=13;j>0;j--)
			reg[j]=reg[j-1];
		reg[j]=temp2;
	}

}

/*解扰*/
//输入：待处理的数组指针，数组长度，扰码种子，调制结束
void Scrambler_Soft(double* softInfo,int len,int S_seed,int M)
{
	uc *flag_data;
	int i;
	flag_data=(uc*)malloc(len*sizeof(double));
	for(i=0;i<len;i++)
		flag_data[i]=0;
	Scrambler(flag_data,len,S_seed);
	for(i=0;i<len;i++)
	{
		if(1==flag_data[i])
		{
			softInfo[i]=-softInfo[i];
			if(8==M&&0==i%3)
				softInfo[i]=sqrt(2)-softInfo[i];
		}		
	}
	free(flag_data);
	if(flag_data!=NULL)
		flag_data=NULL;
}
