#include<string.h>
#include<stdlib.h>

//#include<stdio.h>

#include"AES.h"

#define Nb 4
#define Nk 4
#define Nr 10
//#define BUFFSIZE 128
extern unsigned char **expandedkey;



 
/********************************************************************************************************/
/*                                                                                                              AES加密算法                                                                                                */
/******************************************************************************************************* */

void AES_Encrypt(unsigned char (*state)[4]){	
	    int i, j, k, t;
		unsigned char temp[4][4];
	  //  FILE *fp;  // 声明一个文件流类型的变量，FILE 为 stdio.h 里定义的
        //  fp=fopen("aes.txt","a+"); //用 fopen 函数打开文件，第一个参数表示文件名，若不是当前
                               // 路径请加 \\ 号，如 C:\\Windows\\System32，"w" 表示写(write)
	     // if( (fp= fopen("aes.txt", "r+"))==NULL )
             // {
             //      puts("文件打开出错！");
             //             return;
             // }	
		//fprintf(fp,"\n");
		//fprintf(fp,"****************************** 1 ******************************");
		//fprintf(fp,"\n");
	    //for(i = 0; i < 4;i++){
		//	for(j = 0; j < 4; j++)
		//		fprintf(fp,"Start: %x ",state[i][j]);
		//	fprintf(fp,"\n");
		//}
		//fprintf(fp,"\n");
		for(i = 0; i < 4;i++)
			for(j = 0; j < 4; j++)
			temp[i][j] = expandedkey[j][i];         //将消息矩阵state[][]和轮密钥矩阵expandedkey[][]
		//for(i = 0; i < 4;i++){
		//	for(j = 0; j < 4; j++)
		//		fprintf(fp,"Key: %x ",temp[i][j]);
		//	fprintf(fp,"\n");
		//}
		//fprintf(fp,"\n");
		 AddRoundkey(state,temp);              //进行异或运算AddRoundkey(state,expandedkey) 
				//fprintf(fp,"\n");
		 for(i = 1; i < Nr; i++){                    //加密算法的前N -1轮	
			//fprintf(fp,"\n");
			//fprintf(fp,"****************************** % x ******************************\n", (i + 1));
			//fprintf(fp,"\n");
			ByteSubs(state);
			//fprintf(fp,"\n");
			ShiftRows(state);
			//fprintf(fp,"\n");
			MixColumns(state);
			//fprintf(fp,"\n");
			for(k = 0; k < 4; k++)
				for(t = 0; t < 4; t++)
					temp[k][t] = expandedkey[i * 4 + t][k];	
			//for(m = 0; m < 4; m++){
			//	for(n = 0; n < 4; n++)
			//		fprintf(fp,"RoundKey: %x ",temp[m][n]);
			//	fprintf(fp,"\n");}
			//fprintf(fp,"\n");
			 AddRoundkey(state,temp);	
			 //fprintf(fp,"\n");
			
		}
		//fprintf(fp,"\n");
		//fprintf(fp,"****************************** %x ******************************",(Nr + 1));
		//fprintf(fp,"\n");
	        ByteSubs(state);
		//	fprintf(fp,"\n");
		    ShiftRows(state);
			//fprintf(fp,"\n");
		    for(i = 0; i < 4;i++)
			     for(j = 0; j < 4; j++)
				      temp[i][j] = expandedkey[10 * 4 + j][i];
			//fprintf(fp,"\n");
			//for(m = 0; m < 4; m++){
				//for(n = 0; n < 4; n++)
					//fprintf(fp,"RoundKey: %x ",temp[m][n]);
				//fprintf(fp,"\n");}
		//	fprintf(fp,"\n");
		//	fprintf(fp,"\n");
		    AddRoundkey(state,temp);
			//fprintf(fp,"\n");
	/* for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			fprintf(fp,"ciphermessage = %x ", state[i][j]);
		}
		fprintf(fp,"\n");
	}	
	 fclose(fp);*/
}

/*========================================== S盒的构造==========================================*/

unsigned char Sbox(unsigned char row, unsigned char column){
	unsigned char sbox[16][16] = {{0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},      
                                                     {0xca,  0x82, 0xc9,  0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4,  0xa2,  0xaf, 0x9c,0xa4, 0x72, 0xc0},          
                                                     {0xb7, 0xfd,  0x93,  0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},       
                                                     {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},    
                                                     {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},       
                                                     {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},      
                                                     {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},          
                                                     {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2},        
                                                     {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},      
                                                     {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb},        
                                                     {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79},    
                                                     {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08},       
                                                     {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a},       
                                                     {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e},   
                                                     {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf},    
                                                     {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16},};   

              return  sbox[row][column];
}


/*========================================== 密钥扩展 ===========================================*/

void Keyexpansion(unsigned char *cipher,unsigned char **expandedkey){
	int i, j, m;
	unsigned char temp[4] ;
	unsigned char Rcon[11][4] = {{0x00, 0x00, 0x00, 0x00},
		                         {0x01, 0x00, 0x00, 0x00},
		                         {0x02, 0x00, 0x00, 0x00},
		                         {0x04, 0x00, 0x00, 0x00},
		                         {0x08, 0x00, 0x00, 0x00},
		                         {0x10, 0x00, 0x00, 0x00},
		                         {0x20, 0x00, 0x00, 0x00},
		                         {0x40, 0x00, 0x00, 0x00},
		                         {0x80, 0x00, 0x00, 0x00},
		                         {0x1b, 0x00, 0x00, 0x00},
		                         {0x36, 0x00, 0x00, 0x00}};
    for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			expandedkey[i][j] = cipher[4 * i + j]; 

	for(i = Nk; i < Nb * (Nr + 1); i ++){
		for(j = 0; j < Nb; j++)
				temp[j] = expandedkey[i - 1][j];
		if(i % Nk == 0){
			     Rotword(temp);
		         Subword(temp);
			 for(m = 0; m < Nb; m++){
				 temp[m] =  temp[m] ^ Rcon[i / 4][m];
			 }
		}
		else if(Nk > 6 && i % Nk == 4)
		       Subword(temp);
			for(j = 0; j < 4; j++)
			      expandedkey[i][j] = expandedkey[i - Nk][j] ^ temp[j];	
	   }
//	for(i = 0; i < Nb * (Nr + 1); i ++){
	//	for(j = 0; j < Nb; j++)
		//	printf("expandedkey = %x ",expandedkey[i][j]);
	//	printf("%d",i);
	//	printf("\n");
//	}
}

void Rotword(unsigned char *word){
	int i;
	unsigned char temp = word[0];
	for(i = 0; i < 3;i++)
		word[i] = word[(i + 1) % 4];
	     word[3] = temp;
}

void Subword(unsigned char *rotword){
	int i, row, column;
	for(i = 0; i < 4;i++){
		row = rotword[i] >> 4;
		column = rotword[i] &0x0f;
		rotword[i] = Sbox(row,column);
	}
	
}

/*===================================== 密钥加 ===========================================*/
void AddRoundkey(unsigned char (*state)[4],unsigned char (*temp)[4]){
	
	int i, j;
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			state[i][j] = state[i][j] ^ temp[i][j];
	//for(i = 0; i < 4; i++){
		//for(j = 0; j < 4; j++)
	   //     printf("AddRoundkey = %x ",state[i][j]);
	//printf("\n");
	//}
}

/*========================================= 字节替换 ========================================*/
 void ByteSubs(unsigned char (*state)[4]){
   int i, j, row, column;
   for(i= 0; i < 4; i++)
	   for(j = 0; j < 4; j++){
		   row = state[i][j] >> 4;
		   column = state[i][j] & 0x0f;
		   state[i][j] = Sbox(row,column);		
	   }
   //for(i = 0; i < 4; i++){
	//   for(j = 0; j < 4; j++)
	//        printf("ByteSubs = %x  ",state[i][j]);
	//printf("\n");
	//}
   }

 /*======================================== 行移位 ===========================================*/
 void ShiftRows(unsigned char (*state)[4]){       //对消息矩阵state做移位操作
	 unsigned char temp[4][4];
	 int i, j, m, n;
	for(m = 0; m < 4; m++)
		for(n = 0; n < 4; n++)
			temp[m][n] = state[m][n];
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			state[i][j] = temp[i][(i + j) % 4];
	//for(i = 0; i < 4; i++){
	  // for(j = 0; j < 4; j++)
	    //   printf("ShiftRows = %x  ",state[i][j]);
	//printf("\n");
	//}
}

 /*========================================= 列混合 ==========================================*/
void MixColumns(unsigned char (*state)[4]){
	int i, j, k;
    unsigned char temp=0;
	static unsigned char state1[4][4];
	unsigned char mixcolumn[4][4] =  {{0x02, 0x03, 0x01, 0x01},
		                              {0x01, 0x02, 0x03, 0x01},
	                                  {0x01, 0x01, 0x02, 0x03},
	                                  {0x03, 0x01, 0x01, 0x02}};

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
		{
			for(k=0; k<4; k++)
			{
			
				temp ^=Xtime(state[k][i],mixcolumn[j][k]);
			}
			state1[j][i]=temp; 
			temp=0;
		}

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			state[i][j]=state1[i][j];
	//for(i = 0; i < 4; i++){
	 //  for(j = 0; j < 4; j++)
	  //      printf("MixColumns = %x  ",state[i][j]);
	//printf("\n");
	//}
	
}

unsigned char Xtime(unsigned char elem,unsigned char num){
	unsigned char temp=0;
	unsigned char temp1 =0;
	unsigned char temp2 = 0;
	if(num == 0x01)
		 elem *= num;
	else if(num == 0x02){
		if(elem >= 0x80){
			elem <<= 1;
			elem ^= 0x1b;	 
		}
		else   
			elem <<= 1;
	}
	else if(num == 0x03){
		temp = elem;	
		elem = Xtime(elem,(num-1));
		elem ^= temp;
	}
	else if (num == 4){
		temp = Xtime(elem,0x02);
	  if(temp > 0x11b)
		 temp ^= 0x11b;
	  	  elem = Xtime(temp,0x02);
		  if(elem  > 0x11b)
			  elem ^= 0x11b;
	}
	else if(num == 0x08){
		temp = Xtime(elem,0x04);
		elem = Xtime(temp,0x02);		
	}  
	else if(num == 0x09){
	  temp = elem;
	  elem = Xtime(elem,0x08);
	  elem ^= temp;
	}
	else if(num == 0x0b){
		temp = elem;
		elem = Xtime(temp,0x08);
		temp1= Xtime(temp,0x02);
	    elem = elem ^ temp1 ^ temp;
	}
	else if(num == 0x0d){
		temp = elem;
		temp1 = Xtime(temp,0x04) ;
		elem = Xtime(temp,0x08);
		elem = elem ^ temp1^ temp;
	}
	else if(num == 0x0e){
		temp = elem;
		temp1 = Xtime(temp,0x04);
		temp2 = Xtime(temp,0x02);
		elem = Xtime(temp,0x08);
		elem = elem ^ temp1 ^ temp2;
	}
	return elem;
}  


#define AES_LOOP_BITS 128
#define AES_LOOP_BYTES 16

void mac_encrypt(unsigned char *p, unsigned char MK[16], int nBytesOfp)
{
	int iLoopTime=0;//总共要加密的次数

	if (nBytesOfp % AES_LOOP_BYTES != 0)
	{
		iLoopTime = nBytesOfp / AES_LOOP_BYTES + 1;
	}
	else
	{
		iLoopTime = nBytesOfp / AES_LOOP_BYTES;
	}
	unsigned char *ciphertext;
	ciphertext = (unsigned char*) malloc(sizeof(unsigned char) * (iLoopTime*16));//根据循环次数动态开辟密文空间
	unsigned char Input[4][4] = {0};

	//密钥扩展
	 expandedkey = (unsigned char**) malloc(sizeof(unsigned char *) * (Nb * (Nr + 1)));
	 int i=0;
	for(; i < Nb * (Nr + 1); i++)
		*(expandedkey + i ) =(unsigned char*) malloc(sizeof(unsigned char) * Nb);
		  
    Keyexpansion(MK,expandedkey);
	//前ilooptime-1次循环
	i=0;
	for(;i<iLoopTime-1;i++)
	{
	  memcpy(Input,p + i * 16,4*4*sizeof(char));
	  AES_Encrypt(Input);
	  memcpy(ciphertext + i * 16, Input, 4*4*sizeof(char));
	}
	//最后一次需要在后面补充0
	//计算剩余字节数
	int nLeftBytes=nBytesOfp-i*4*4;
	memcpy(Input,p+i*4*4,nLeftBytes);
	int j;
	for (j = nLeftBytes; j < 4*4; j++)
	{
		Input[j / 4][j % 4] = 0;
	}
	 AES_Encrypt(Input);
	 memcpy(ciphertext + i * 16,Input, 4*4*sizeof(char));
	 memcpy(p,ciphertext,4*4*iLoopTime*sizeof(char));
	 free(ciphertext);
	 free(expandedkey);
}
