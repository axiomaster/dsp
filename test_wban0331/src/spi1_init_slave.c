/*
 * spi1_init.c
 *
 */
#include "../registers.h"
#include "../Wban.h"

void SPI1_init()
{
	SPI_SPIGCR0 &=~(0x00000001);     //��λ
	//int i = 1000;
	//while(i--);                      //Ҳ����Բ���
	SPI_SPIGCR0  = 0x00000001;       //������λ
	SPI_SPIGCR1 |= 0x00000000;       //0����ģʽ; 0���ǻػ�ģʽ
	//SPI_SPIPC(0) = 0x00000F01;       //��Ϊ��SPI1������ѡ��CS0,5-pin ģʽ
	SPI_SPIPC(0) = 0x00000E01;       //��Ϊ��SPI1������ѡ��CS0,4-pin chip selectģʽ
	SPI_SPIDEF   = 0x000000FF;       // slave: FFh
	SPI_SPIDAT1  = 0x10020000;       //ѡSPIFMT0,CSHOLD:1���ԣ�Ƭѡ�͵�ƽ
	SPI_SPIFMT(0)= 0x00020408;       //ʱ��30M,MSB���Ƴ���8bit���ݣ�CPUʱ�Ӱ�300M���,polarity=1,phase=0,��������Ϊunsigned char:8λ
	SPI_SPIGCR1 |= 0x01000000;       //ʹ��SPI�������㣩
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
		//�������ݲ���
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


//����ʱ�����յ���ǰ��������ʱ��ת��Ϊ���ȣ��ٽ���������ȵĺ������ݡ�
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
		while(SPI_SPIBUF & ( 0x80000000 ) );//��֤���ݽ��ռĴ����Ƿ�Ϊ�գ���Ϊ�շ����������У���֪���Ƿ�Ҫע�͵���
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

		//�������ݲ���
		//for(i=0;i<len;i++)
		//for(i=0;i<1988;i++)//����255�ǹ̶�����ĳ��ȣ��������жϺ���ܻ��Ϊ�ж�λ�ı�־�ȣ�
	 for(i=2;i<length+2;i++)
		{
		    //int m = 1000;
		 	//while(m--);
		 	while( SPI_SPIBUF & 0x10000000 );//no bit error
			while(SPI_SPIBUF & ( 0x80000000 ) );//��֤���ݽ��ռĴ����Ƿ�Ϊ�գ���Ϊ�շ����������У���֪���Ƿ�Ҫע�͵���
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
