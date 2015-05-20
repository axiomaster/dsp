/*
 * spi0_init.c
 */
//SPI低电平有效，所以选择SPI0，CS为chip select，选择CS1
#include "../registers.h"
#include <stdio.h>
#include "../hardware.h"
#include "../Wban.h"

extern Uint16 FPGA_length;

void SPI0_init()
{
	SPI0_SPIGCR0 &=~(0x00000001);     //复位
	//int i = 1000;
	//while(i--);                      //也许可以不用
	SPI0_SPIGCR0  = 0x00000001;       //结束复位
	SPI0_SPIGCR1 |= 0x00000003;       //3：主模式, 0:非回环模式（第6位为0）
	//SPI_SPIGCR1 |= 0x00010003;       //3：主模式; 1：回环模式
	//SPI_SPIPC(0) = 0x00000F02;       //选择CS1
	SPI0_SPIPC(0) = 0x00000E02;       //选择CS1,4-PIN CHIP SELECT MODE
	//SPI_SPIPC(0) = 0x00000F02;       //选择CS1
	SPI0_SPIDEF   = 0x00000002;       //CS1 inactive high
	SPI0_SPIDAT1  = 0x10020000;       //选SPIFMT0,CSHOLD传输完成保持片选有效，片选低电平
	//SPI0_SPIFMT(0)= 0x00020210;       //时钟50M,MSB先移出，16bit数据，CPU时钟按300M配的
	//SPI0_SPIFMT(0)= 0x00020410;       //时钟30M,MSB先移出，16bit数据，CPU时钟按300M配的
	SPI0_SPIFMT(0)= 0x00020F10;       //时钟10M,MSB先移出，16bit数据，CPU时钟按300M配的,polarity=1,phase=0(收数据只可以是10模式)
	//SPI0_SPIFMT(0)= 0x00010410;    //01
	//SPI0_SPIFMT(0)= 0x00000410;    //00
	//SPI0_SPIFMT(0)= 0x00030410;    //11
	SPI0_SPIGCR1 |= 0x01000000;       //使能SPI
}

void spi0flash_cycletx(Uint16 *buf,Uint16 len)
{
	Uint16 i;
	Uint16 *test_pointer1;
	test_pointer1 = (Uint16*)(0x01C4103C); //SPI0DAT1

	//while(1)
	//{
		//发送数据部分
		for(i=0;i<len;i++)
		{
			//printf("%x ", buf[i]);
			if(i == len-1)
				SPI0_SPIDAT1 = (spidat1 & 0x0ffcffff) | buf[i];
			else
				SPI0_SPIDAT1 = spidat1 | buf[i];
#ifdef DEBUG

			printf("%x ", (Uint16)*test_pointer1);   //16进制输出SPIDAT1中数据
#endif
			//printf("%d ", (Uint16)*test_pointer2);  //*test_pointer2={1,2,3...252,253}//回环模式测试
		}
	//}
		printf("\n");
}

void spi0flash_cyclerx(Uint16 *buf)
{
	Uint16 i;
	SPI0_SPIBUF;

	Uint16 k = 0;

	//Uint16 *test_pointer1;
	//test_pointer1=(Uint16*)(0x01C4103C); //SPI0DAT1
	Uint16 *test_pointer2;
	test_pointer2=(Uint16*)(0x01C41040); //SPI0BUF

	//while(1)
	//{
		//接收数据部分，要发送空数据来提供时钟
		//Uint16 length1 = 2000;
		Uint16 rx_buf[2000] = {0};  //这个长度应该设置为多少？FPGA传送过来的数据长度是否有最大值限制？
		//Uint16 rx_buf[50] = {0};
		for(i=0;i<2000;i++)
		//for(i=0;i<50;i++)
		{
			while( SPI0_SPIBUF & 0x10000000 ); //没有bit error发生
			//SPI0_SPIDAT1 = (spidat1 & 0x0ffcffff) | rx_buf[i]; //发送部分发送空数据0用于产生时钟
			//if(i == 50-1)
			if(i == 2000-1)
				SPI0_SPIDAT1 = (spidat1 & 0x0ffcffff) | rx_buf[i]; //发送部分发送空数据0用于产生时钟
			else
				SPI0_SPIDAT1 = spidat1 | rx_buf[i];
			while(SPI0_SPIBUF & ( 0x80000000 ) );  //（不要注释掉，注释掉接到的第一个为0）要不然跳不出while循环，因为不是回环模式之后，SPIBUF无数据时一直为空，所以while循环跳不出来，无法继续执行
			//printf("%d ", (Uint16)*test_pointer1); //*test_pointer1={0}
			//printf("%x ", (Uint16)*test_pointer2); //*test_pointer2={接收到的数据}
			buf[i] = *test_pointer2;
			k++;
		}
		FPGA_length = k;
		printf("\n ");
		printf("%d ", k);
		printf("\n ");

	//}
}
