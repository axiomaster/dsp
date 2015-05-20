/*
 * spi1_init.c
 *
 */
#include "../registers.h"
#include "../Wban.h"

void SPI1_init()
{
	SPI_SPIGCR0 &=~(0x00000001);     //复位
	//int i = 1000;
	//while(i--);                      //也许可以不用
	SPI_SPIGCR0  = 0x00000001;       //结束复位
	SPI_SPIGCR1 |= 0x00000000;       //0：从模式; 0：非回环模式
	//SPI_SPIPC(0) = 0x00000F01;       //因为是SPI1，所以选择CS0,5-pin 模式
	SPI_SPIPC(0) = 0x00000E01;       //因为是SPI1，所以选择CS0,4-pin chip select模式
	SPI_SPIDEF   = 0x000000FF;       // slave: FFh
	SPI_SPIDAT1  = 0x10020000;       //选SPIFMT0,CSHOLD:1忽略，片选低电平
	SPI_SPIFMT(0)= 0x00020408;       //时钟30M,MSB先移出，8bit数据，CPU时钟按300M配的,polarity=1,phase=0,传输数据为unsigned char:8位
	SPI_SPIGCR1 |= 0x01000000;       //使能SPI（或运算）
}

void spiflash_cycletx(Uint8 *buf,int len)
{
	int i;
	SPI_SPIBUF;
	Uint8 num_buf[2000] = {0};

	Uint8 *test_pointer1;
	test_pointer1=(Uint8*)(0x01F0E03C); //SPI1DAT1
	//while(1)
	//{
		//发送数据部分
	for(i=0;i<len;i++)
		{
			while(SPI_SPIBUF & ( 0x80000000 ) );
			if(i == len-1)
				SPI_SPIDAT1 = (spidat1 & 0x0ffcffff) | buf[i];
			else
				SPI_SPIDAT1 = spidat1 | buf[i];

			num_buf[i] = *test_pointer1;
		}
#ifdef DEBUG1
	for(i = 0; i < len; i++)
		printf("0x%x ", num_buf[i]);  //*test_pointer1={1,2,3...252,253}
	printf("\n");
#endif

	//}
}


//接收时，接收到的前两个数据时先转换为长度，再接收这个长度的后面数据。
void spiflash_cyclerx(Uint8 *buf)
{
	int i;
	Uint16 length;
	//Uint8 *buf;
	SPI_SPIBUF;

	Uint8 *test_pointer2;
	test_pointer2=(Uint8*)(0x01F0E040); //SPI1BUF
	//while(1)
	//{

	for(i=0; i<2; i++)
	{
		while( SPI_SPIBUF & 0x10000000 );//no bit error
		while(SPI_SPIBUF & ( 0x80000000 ) );//验证数据接收寄存器是否为空，不为空方可向下运行，不知道是否要注释掉？
		buf[i] = *test_pointer2;
	}
	/*for(i=0; i<2; i++)
	{
		printf("%d ", buf[i]);
	}
	printf("\n");*/
	length = buf[0];
	length = length << 8;
	length = length | buf[1];

		//接收数据部分
		//for(i=0;i<len;i++)
		//for(i=0;i<1988;i++)//其中255是固定数组的长度（后面有中断后可能会改为中断位的标志等）
	 for(i=2;i<length+2;i++)
		{
		    //int m = 1000;
		 	//while(m--);
		 	while( SPI_SPIBUF & 0x10000000 );//no bit error
			while(SPI_SPIBUF & ( 0x80000000 ) );//验证数据接收寄存器是否为空，不为空方可向下运行，不知道是否要注释掉？
			buf[i] = *test_pointer2;
			//printf("%x ", *test_pointer2);
		}
	 printf("%d ",length);
	 printf("\n");
	 for(i=0; i<length + 2; i++)
	 	{
	 		printf("%d ", buf[i]);
	 	}
	 printf("\n");

	//}
}
