#include<string.h>
#include<stdlib.h>

#include<stdio.h>
#include "AES.h"

#include"AES_CBC.h"

#define Nb 4
#define Nk 4
#define Nr 10
//#define BUFFSIZE 128
extern unsigned char **expandedkey;

void XOR(unsigned char* a, unsigned char* b, int nBytes)
{
	int i;
	for (i = 0; i < nBytes; i++)
		a[i] = a[i] ^ b[i];
}

#define AES_LOOP_BITS 128		//AES每轮加密的比特数
#define AES_LOOP_BYTES  16		//AES每轮加密的字节数

void CBC_encrypt(
	unsigned char *p,
	unsigned char *c,
	unsigned char MK[16],
	int nBytesOfp
	)
{
	int i,j;
	int iLoopTime = 0;  //总共要循环的次数
	if (nBytesOfp % AES_LOOP_BYTES != 0)
	{
		iLoopTime = nBytesOfp / AES_LOOP_BYTES + 1;
	}
	else
	{
		iLoopTime = nBytesOfp / AES_LOOP_BYTES;
	}

	unsigned char Output[4][4] = {0};
	unsigned char Input[4][4] = {0};
	//密钥扩展
	 expandedkey = (unsigned char**) malloc(sizeof(unsigned char *) * (Nb * (Nr + 1)));
	 
	for(i=0;i < Nb * (Nr + 1); i++)
		*(expandedkey + i ) =(unsigned char*) malloc(sizeof(unsigned char) * Nb);
		  
    Keyexpansion(MK,expandedkey);
	i = 0;
	//一次拷贝128bit
	for (; i < iLoopTime - 1; i++)
	{
		memcpy(p,(char*)p + i*4*4, 4*4*sizeof(char));
		memcpy(Input,p, 4*4*sizeof(char));

		//进行异或运算
		XOR((unsigned char*)Input, (unsigned char*)Output, 4*4);
		//进入AES加密
		AES_Encrypt(Input);

		memcpy(Output,Input, 4*4*sizeof(char));
	}
	//最后一次需要在后面补充0
	//计算剩余字节数
	int nLeftBytes = nBytesOfp - i * 4 * 4;
	memcpy(Input,p + i * 4 * 4, nLeftBytes);

	for (j = nLeftBytes; j < 4*4; j++)
	{
		Input[j / 4][j % 4] = 0;
	}
	//进行异或运算
	XOR((unsigned char*)Input, (unsigned char*)Output, 4*4);
	//进入AES加密
	AES_Encrypt(Input);

	//最后的输出赋值给c
	memcpy(c, Input, 4*4*sizeof(char));
}

