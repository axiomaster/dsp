/*
 * main.c
 */
#include "registers.h"
#include "hardware.h"
#include <stdio.h>  //有printf时要加上

unsigned int flag = 0; //数据初始化，flag全局变量，初始化为0，中断触发时执行将flag置为1操作


void main(void) {
	
	disable_int(); //关闭全局中断

	//设备初始化
	PLL0_init( CORE_FREQ_300M );  //pll初始化，其中设置ISTP = 0x11800000 ;即定位中断向量表到0x11800000
	psc();     //电源管理
	pinmux();  //管脚复用

	INT_MUX(15, 65); //将GPIObank0的中断（中断事件号为65）挂到15号的可屏蔽中断上
	GPIO_int_init();

	enable_int(); //使能全局中断

	//中断测试部分：若发生中断，则flag置为1，执行else，然后停止在while(1)。若没有触发中断，则flag一直为0，循环if语句。
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
