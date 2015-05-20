#include<stdlib.h>
#include<string.h>
#include"AES.h"
#define Nb 4
#define Nk 4
#define Nr 10
extern unsigned char **expandedkey;

/*****************************************************************************************************/
/*                                     AES�����㷨                                                                                                      */
/*****************************************************************************************************/
unsigned char InvSbox(unsigned char row, unsigned char column){
unsigned char Invsbox[16][16] = {{0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb},
                                 {0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb},
                                 {0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e},
                                 {0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25},
                                 {0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92},
                                 {0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84},
                                 {0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06},
                                 {0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b},
                                 {0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73},
                                 {0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e},
                                 {0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b},
                                 {0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4},
                                 {0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f},
                                 {0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef},
                                 {0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61},
                                 {0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d},};
     return Invsbox[row][column];
}
void AES_Decrypt(unsigned char (*state)[4]){
	int i, j, m, n;
	unsigned char temporary[4][4];
	//printf("\n");
	//printf("***************************************************************\n");
	//printf("***************************************************************\n");
	//printf("\n");
	//printf("***************************** %x ******************************\n",(Nr + 1));
	//printf("\n");
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			temporary[i][j] = expandedkey[4 * 10 + j][i];
	//for(m = 0; m < 4; m++){
	//	for(n = 0; n < 4; n++)
		//	printf("Roundkey: %x ",temporary[m][n]);
		//printf("\n");
	//}
	      //   printf("\n");
			 AddRoundkey(state,temporary);
			// printf("\n");
			 InvShiftRows(state);
			// printf("\n");
			 InvSubBytes(state);
			// printf("\n");
		
	for(i = (Nr -1); i >= 1; i--){
	//printf("\n");
	//printf("***************************** %x ******************************",(i + 1));
//	printf("\n");
	    for(m = 0; m < 4; m++)
			for(n = 0; n < 4; n++)
				temporary[m][n] = expandedkey[4 * i + n][m];
		//for(m = 0; m < 4; m++){
		   // for(n = 0; n < 4; n++)
		//	printf("Roundkey: %x ",temporary[m][n]);
	//	printf("\n");
//	}
		// printf("\n");
		AddRoundkey(state,temporary);
		// printf("\n");
		InvMixColumns(state);
		// printf("\n");
		InvShiftRows(state);
		// printf("\n");
		InvSubBytes(state);	
		// printf("\n");
	}
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			temporary[i][j] = expandedkey[j][i];
			//for(m = 0; m < 4; m++){
		   // for(n = 0; n < 4; n++)
			//printf("Roundkey: %x ",temporary[m][n]);
		//printf("\n");
	//}
		//	printf("\n");
	AddRoundkey(state,temporary);
	//printf("\n");
	//for(i = 0; i < 4; i++){
		//for(j = 0; j < 4; j++){
		//	printf("plainmessage = %x ", state[i][j]);
	//	}
	//	printf("\n");
	//}	
	//printf("\n");
}

void InvSubBytes(unsigned char (*state)[4]){
	int i, j;
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			state[i][j] = InvSbox(state[i][j] >> 4,state[i][j] & 0x0f);
	//for(i = 0; i < 4; i++){
		//for(j = 0; j < 4; j++)
		//	printf("InvSubBytes = %x ",state[i][j]);
		//printf("\n");
//	}
}

void InvShiftRows(unsigned char (*state)[4]){
	int i, j, m, n;
	unsigned char temp1[4][4];
	for(m = 0; m < 4; m++)
		for(n = 0; n < 4; n++)
			temp1[m][n] = state[m][n];
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			state[i][(i + j) % 4] = temp1[i][j];
	//for(i = 0; i < 4; i++){
	//	for(j = 0; j < 4; j++)
		//	printf("InvShiftRows = %x ",state[i][j]);
	//	printf("\n");
	//}
}

void InvMixColumns(unsigned char (*state)[4]){
	int i, j, k, m, n;
	unsigned char temp = 0;
	static unsigned char temp1[4][4];
	unsigned char invmixcolumn[4][4] = {{0x0e,0x0b,0x0d,0x09},{0x09,0x0e,0x0b,0x0d},
	                                    {0x0d,0x09,0x0e,0x0b},{0x0b,0x0d,0x09,0x0e}};
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++){
			for(k = 0; k < 4; k++)		
				temp ^= Xtime(state[k][i],invmixcolumn[j][k]);
			temp1[j][i] = temp;
			temp = 0;
		}
	for(m = 0; m < 4; m++)
		for(n = 0; n < 4; n++)
			state[m][n] = temp1[m][n];
	//for(i = 0; i < 4; i++){
	//	for(j = 0; j < 4; j++)
	//		printf("InvMixColumns = %x ",state[i][j]);
	//	printf("\n");
	//}
}

#define AES_LOOP_BITS 128
#define AES_LOOP_BYTES 16

void mac_decrypt(unsigned char *p, unsigned char MK[16], int nBytesOfp)
{
	int iLoopTime=0;//�ܹ�Ҫ���ܵĴ���

	

	if (nBytesOfp % AES_LOOP_BYTES != 0)
	{
		iLoopTime = nBytesOfp / AES_LOOP_BYTES + 1;
		
	}
	else
	{
		iLoopTime = nBytesOfp / AES_LOOP_BYTES;
	}
// 	iLoopTime=nBytesOfp/AES_LOOP_BYTES;
	unsigned char *plaintext;
	plaintext = (unsigned char*) malloc(sizeof(unsigned char) * (iLoopTime*16));//����ѭ��������̬�������Ŀռ�
	unsigned char Input[4][4] = {0};


	//��Կ��չ
	 expandedkey = (unsigned char**) malloc(sizeof(unsigned char *) * (Nb * (Nr + 1)));
	 int i;
	for(i = 0; i < Nb * (Nr + 1); i++)
		*(expandedkey + i ) =(unsigned char*) malloc(sizeof(unsigned char) * Nb);
		  
    Keyexpansion(MK,expandedkey);
	//ǰilooptime-1��ѭ��
	//int i=0;
	for(i = 0;i<iLoopTime;i++)
	{
		//printf("loopTime :%d\n\n", i+1);
	  memcpy(Input,p + 16*i,4*4*sizeof(char));
	
	  AES_Decrypt(Input);
	  memcpy(plaintext + 16*i, Input, 4*4*sizeof(char));
	}
	//���һ����Ҫ�ں��油��0
	//����ʣ���ֽ���
	/*int nLeftBytes=nBytesOfp-i*4*4;
	memcpy_s(Input,nLeftBytes,p+i*4*4,nLeftBytes);
	for (int j = nLeftBytes; j < 4*4; j++)
	{
		Input[j / 4][j % 4] = 0;
	}
	 AES_Encrypt(Input);
	 memcpy_s(ciphertext++, 4*4*sizeof(char),Input, 4*4*sizeof(char));*/
	 memcpy(p,plaintext,4*4*iLoopTime*sizeof(char));
	 free(plaintext);
	 free(expandedkey);
}




                                                            
