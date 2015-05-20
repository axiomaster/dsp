//#include "../registers.h"
//#include "../hardware.h"
#include "registers.h"
#include "hardware.h"
#include <stdio.h> //有printf时要加上


extern unsigned int flag;

void GPIO_int_init(void)
{
	GPIO_BINTEN = 0x00000001; //bank0 使能
	GPIO_DIR(0)	= 0x00000014; //bank0的pin4管脚中断用设置为输入（读1）,pin1为输出（写0），pin2为输入（读1）:00010100（pin1和pin2为CCA用）
	GPIO_CLR_FAL_TRIG(0) = 0x00000010; //先清除bank0的pin4中断寄存器信息
	//int i=100;
	//while(i--);
	//GPIO_CLR_FAL_TRIG(0) &= 0x0;
	GPIO_SET_FAL_TRIG(0) = 0x00000010; //将bank0的pin4设置为拉低时触发中断（因为为输入，所以只需读取状态即可，不进行设置）
}


//disable interrupt
void disable_int()
{
	ISTP = 0x11800000 ;
	CSR	= 0x0;
	IER	= 0x0; //关闭全部全局中断（不可屏蔽与可屏蔽中断）
}

// enable interrupt
void enable_int()
{
	//ICR	= 0xFFF0; //Interrupt 4-15 Clear
	IER |= 0x8003; // Interrupt 4-15 and Nonmaskable interrupt enabled
	CSR |= 1;
}

//中断服务函数
 interrupt void  fpga(void)
{
	flag = 1;
	printf("\n");
	printf("interrupt occured~~\n");
	printf("\n");

	//再重新设置一遍，保证下次中断再次正确触发
	GPIO_CLR_FAL_TRIG(0) = 0x00000010; //清除bank0的pin4中断寄存器信息
	GPIO_SET_FAL_TRIG(0) = 0x00000010; //bank0的pin4设置为拉低触发中断
}
