/*
 * spi0_init.c
 */
//SPI�͵�ƽ��Ч������ѡ��SPI0��CSΪchip select��ѡ��CS1
#include "../registers.h"
#include <stdio.h>
#include "../hardware.h"
#include "../Wban.h"

extern Uint16 FPGA_length;

void SPI0_init()
{
	SPI0_SPIGCR0 &=~(0x00000001);     //��λ
	//int i = 1000;
	//while(i--);                      //Ҳ����Բ���
	SPI0_SPIGCR0  = 0x00000001;       //������λ
	SPI0_SPIGCR1 |= 0x00000003;       //3����ģʽ, 0:�ǻػ�ģʽ����6λΪ0��
	//SPI_SPIGCR1 |= 0x00010003;       //3����ģʽ; 1���ػ�ģʽ
	//SPI_SPIPC(0) = 0x00000F02;       //ѡ��CS1
	SPI0_SPIPC(0) = 0x00000E02;       //ѡ��CS1,4-PIN CHIP SELECT MODE
	//SPI_SPIPC(0) = 0x00000F02;       //ѡ��CS1
	SPI0_SPIDEF   = 0x00000002;       //CS1 inactive high
	SPI0_SPIDAT1  = 0x10020000;       //ѡSPIFMT0,CSHOLD������ɱ���Ƭѡ��Ч��Ƭѡ�͵�ƽ
	//SPI0_SPIFMT(0)= 0x00020210;       //ʱ��50M,MSB���Ƴ���16bit���ݣ�CPUʱ�Ӱ�300M���
	//SPI0_SPIFMT(0)= 0x00020410;       //ʱ��30M,MSB���Ƴ���16bit���ݣ�CPUʱ�Ӱ�300M���
	SPI0_SPIFMT(0)= 0x00020F10;       //ʱ��10M,MSB���Ƴ���16bit���ݣ�CPUʱ�Ӱ�300M���,polarity=1,phase=0(������ֻ������10ģʽ)
	//SPI0_SPIFMT(0)= 0x00010410;    //01
	//SPI0_SPIFMT(0)= 0x00000410;    //00
	//SPI0_SPIFMT(0)= 0x00030410;    //11
	SPI0_SPIGCR1 |= 0x01000000;       //ʹ��SPI
}

void spi0flash_cycletx(Uint16 *buf,Uint16 len)
{
	Uint16 i;
	Uint16 *test_pointer1;
	test_pointer1 = (Uint16*)(0x01C4103C); //SPI0DAT1

	//while(1)
	//{
		//�������ݲ���
		for(i=0;i<len;i++)
		{
			//printf("%x ", buf[i]);
			if(i == len-1)
				SPI0_SPIDAT1 = (spidat1 & 0x0ffcffff) | buf[i];
			else
				SPI0_SPIDAT1 = spidat1 | buf[i];
#ifdef DEBUG

			printf("%x ", (Uint16)*test_pointer1);   //16�������SPIDAT1������
#endif
			//printf("%d ", (Uint16)*test_pointer2);  //*test_pointer2={1,2,3...252,253}//�ػ�ģʽ����
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
		//�������ݲ��֣�Ҫ���Ϳ��������ṩʱ��
		//Uint16 length1 = 2000;
		Uint16 rx_buf[2000] = {0};  //�������Ӧ������Ϊ���٣�FPGA���͹��������ݳ����Ƿ������ֵ���ƣ�
		//Uint16 rx_buf[50] = {0};
		for(i=0;i<2000;i++)
		//for(i=0;i<50;i++)
		{
			while( SPI0_SPIBUF & 0x10000000 ); //û��bit error����
			//SPI0_SPIDAT1 = (spidat1 & 0x0ffcffff) | rx_buf[i]; //���Ͳ��ַ��Ϳ�����0���ڲ���ʱ��
			//if(i == 50-1)
			if(i == 2000-1)
				SPI0_SPIDAT1 = (spidat1 & 0x0ffcffff) | rx_buf[i]; //���Ͳ��ַ��Ϳ�����0���ڲ���ʱ��
			else
				SPI0_SPIDAT1 = spidat1 | rx_buf[i];
			while(SPI0_SPIBUF & ( 0x80000000 ) );  //����Ҫע�͵���ע�͵��ӵ��ĵ�һ��Ϊ0��Ҫ��Ȼ������whileѭ������Ϊ���ǻػ�ģʽ֮��SPIBUF������ʱһֱΪ�գ�����whileѭ�������������޷�����ִ��
			//printf("%d ", (Uint16)*test_pointer1); //*test_pointer1={0}
			//printf("%x ", (Uint16)*test_pointer2); //*test_pointer2={���յ�������}
			buf[i] = *test_pointer2;
			k++;
		}
		FPGA_length = k;
		printf("\n ");
		printf("%d ", k);
		printf("\n ");

	//}
}
