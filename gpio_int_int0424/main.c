/*
 * main.c
 */
#include "registers.h"
#include "hardware.h"
#include <stdio.h>  //��printfʱҪ����

unsigned int flag = 0; //���ݳ�ʼ����flagȫ�ֱ�������ʼ��Ϊ0���жϴ���ʱִ�н�flag��Ϊ1����


void main(void) {
	
	disable_int(); //�ر�ȫ���ж�

	//�豸��ʼ��
	PLL0_init( CORE_FREQ_300M );  //pll��ʼ������������ISTP = 0x11800000 ;����λ�ж�������0x11800000
	psc();     //��Դ����
	pinmux();  //�ܽŸ���

	INT_MUX(15, 65); //��GPIObank0���жϣ��ж��¼���Ϊ65���ҵ�15�ŵĿ������ж���
	GPIO_int_init();

	enable_int(); //ʹ��ȫ���ж�

	//�жϲ��Բ��֣��������жϣ���flag��Ϊ1��ִ��else��Ȼ��ֹͣ��while(1)����û�д����жϣ���flagһֱΪ0��ѭ��if��䡣
	Main_Loop:

	if(flag == 0)
	{
		goto Main_Loop;
	}
	else
	{
		printf("\n");
				printf("interrupt jumped out!\n");
				printf("\n");
	}
	while(1);

}
