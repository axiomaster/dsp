/*
 * gpio_init.c
 *
 *  Created on: 2014-2-27
 *      Author: Administrator
 */
#include "../registers.h"
#include "../hardware.h"
void GPIO_init()
{
	GPIODirModeSet( GP0_7 ,GPIO_DIR_OUTPUT );  //GO0[7],AIC3106 复位
	GPIODirModeSet( GP0_13,GPIO_DIR_OUTPUT ); //LED
	GPIODirModeSet( GP0_14,GPIO_DIR_OUTPUT );
	GPIODirModeSet( GP0_15,GPIO_DIR_OUTPUT );
}

void GPIODirModeSet(unsigned int pinNumber,unsigned int pinDir)  //设置GP口管脚方向
{
    unsigned int regNumber = 0;
    unsigned int pinOffset = 0;
    regNumber = (pinNumber - 1)/32;
    pinOffset = (pinNumber - 1) % 32;
    if(GPIO_DIR_OUTPUT == pinDir)
    {
        HWREG( SOC_GPIO_REGS + GPIO_DIR(regNumber)) &= ~(1 << pinOffset);
    }
    else
    {
        HWREG( SOC_GPIO_REGS + GPIO_DIR(regNumber)) |= (1 << pinOffset);
    }
}
void GPIOPinWrite(unsigned int pinNumber, unsigned int bitValue)
{
    unsigned int regNumber = 0;
    unsigned int pinOffset = 0;
    regNumber = (pinNumber - 1)/32;
    pinOffset = (pinNumber - 1) % 32;

    if(GPIO_PIN_LOW == bitValue)
    {
        HWREG(SOC_GPIO_REGS + GPIO_CLR_DATA(regNumber)) = (1 << pinOffset);
    }
    else if(GPIO_PIN_HIGH == bitValue)
    {
        HWREG(SOC_GPIO_REGS + GPIO_SET_DATA(regNumber)) = (1 << pinOffset);
    }
}

void GPIO_LED()
{
	int i = 0;
	while(1)
	{
		i = 100000;
		GPIOPinWrite(GP0_13,GPIO_PIN_HIGH);
		while(i--);
		i = 100000;
		GPIOPinWrite(GP0_14,GPIO_PIN_HIGH);
		while(i--);
		i = 100000;
		GPIOPinWrite(GP0_15,GPIO_PIN_HIGH);
		while(i--);
		i = 100000;
		GPIOPinWrite(GP0_13,GPIO_PIN_LOW );
		while(i--);
		i = 100000;
		GPIOPinWrite(GP0_14,GPIO_PIN_LOW );
		while(i--);
		i = 100000;
		GPIOPinWrite(GP0_15,GPIO_PIN_LOW );
		while(i--);

	}
}
