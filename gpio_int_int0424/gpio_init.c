//#include "../registers.h"
//#include "../hardware.h"
#include "registers.h"
#include "hardware.h"
#include <stdio.h> //��printfʱҪ����


extern unsigned int flag;

void GPIO_int_init(void)
{
	GPIO_BINTEN = 0x00000001; //bank0 ʹ��
	GPIO_DIR(0)	= 0x00000014; //bank0��pin4�ܽ��ж�������Ϊ���루��1��,pin1Ϊ�����д0����pin2Ϊ���루��1��:00010100��pin1��pin2ΪCCA�ã�
	GPIO_CLR_FAL_TRIG(0) = 0x00000010; //�����bank0��pin4�жϼĴ�����Ϣ
	//int i=100;
	//while(i--);
	//GPIO_CLR_FAL_TRIG(0) &= 0x0;
	GPIO_SET_FAL_TRIG(0) = 0x00000010; //��bank0��pin4����Ϊ����ʱ�����жϣ���ΪΪ���룬����ֻ���ȡ״̬���ɣ����������ã�
}


//disable interrupt
void disable_int()
{
	ISTP = 0x11800000 ;
	CSR	= 0x0;
	IER	= 0x0; //�ر�ȫ��ȫ���жϣ�����������������жϣ�
}

// enable interrupt
void enable_int()
{
	//ICR	= 0xFFF0; //Interrupt 4-15 Clear
	IER |= 0x8003; // Interrupt 4-15 and Nonmaskable interrupt enabled
	CSR |= 1;
}

//�жϷ�����
 interrupt void  fpga(void)
{
	flag = 1;
	printf("\n");
	printf("interrupt occured~~\n");
	printf("\n");

	//����������һ�飬��֤�´��ж��ٴ���ȷ����
	GPIO_CLR_FAL_TRIG(0) = 0x00000010; //���bank0��pin4�жϼĴ�����Ϣ
	GPIO_SET_FAL_TRIG(0) = 0x00000010; //bank0��pin4����Ϊ���ʹ����ж�
}
