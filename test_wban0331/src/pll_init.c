/*
 * PLL_init.c
 *
 *  Created on: 2014-2-27
 *      Author: Administrator
 */
#include "../registers.h"
#include "../hardware.h"
//spi0,EDMA（增强型直接内存存取）由PLL0的clk2提供，GPIO（通用输入输出）由clk4提供，I2C0和timer由AUXCLK提供
void PLL0_init(unsigned int uiCoreFrequency)
{
	CSR &=~(0x1);                  //关闭所有中断
	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_KICK0R) = 0x83E70B13;
	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_KICK1R) = 0x95A4F1E0;//SYSCFG的锁寄存器
	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_CFGCHIP0) &= ~(0x00000010);      //Clear PLL lock bit
	HWREG(SOC_PLLC_0_REGS + PLLCTL_0) &= ~(0x00000020);     //Clear the PLLENSRC bit in PLLCTL to 0
	HWREG(SOC_PLLC_0_REGS + PLLCTL_0) &= ~(0x00000200);     //Use OSCIN for the PLL bypass clock
	HWREG(SOC_PLLC_0_REGS + PLLCTL_0) &= ~(0x00000001);         //PLLEN 为0，禁止PLL
	sw_wait(150);
	HWREG(SOC_PLLC_0_REGS + PLLCTL_0) &= ~(0x00000200);     //Use OSCIN for the PLL bypass clock
	HWREG(SOC_PLLC_0_REGS + PLLCTL_0) &= ~(0x00000008);     //PLLRES 为0，PLL复位
	HWREG(SOC_PLLC_0_REGS + PLLCTL_0) &= ~(0x00000002);      //PLL低功耗
	sw_wait(150);
	HWREG(SOC_PLLC_0_REGS + POSTDIV_0) |=0x8000;              //PLL post-divide enable
	switch(uiCoreFrequency)
	{
	case CORE_FREQ_300M:HWREG(SOC_PLLC_0_REGS + PLLM_0 ) = 23;         //PLLM为24倍频
						HWREG(SOC_PLLC_0_REGS + POSTDIV_0) &= 0xfffffff0;
						HWREG(SOC_PLLC_0_REGS + POSTDIV_0) |=1;
						while(HWREG(SOC_PLLC_0_REGS + PLLSTAT_0) & 0x1);
						//Wait for the GOSTAT bit in PLLSTAT to clear to 0
						HWREG(SOC_PLLC_0_REGS + PLLDIV3_0) = (0x8000 | 5);   //EMIFA 50M
						HWREG(SOC_PLLC_0_REGS + PLLCMD_0) |=0x1;                   //Set the GOSET bit in PLLCMD to 1
						while( HWREG(SOC_PLLC_0_REGS + PLLSTAT_0) & 0x1);
						break;
	case CORE_FREQ_150M:HWREG(SOC_PLLC_0_REGS + PLLM_0 ) = 23;         //PLLM为24倍频
						HWREG(SOC_PLLC_0_REGS + POSTDIV_0) &= 0xfffffff0;
						HWREG(SOC_PLLC_0_REGS + POSTDIV_0) |=3;
						while( HWREG(SOC_PLLC_0_REGS + PLLSTAT_0) & 0x1);
						//Wait for the GOSTAT bit in PLLSTAT to clear to 0
						HWREG(SOC_PLLC_0_REGS + PLLDIV3_0) = (0x8000 | 4);   //EMIFA 30M
						HWREG(SOC_PLLC_0_REGS + PLLCMD_0) |=0x1;                     //Set the GOSET bit in PLLCMD to 1
						while( HWREG(SOC_PLLC_0_REGS + PLLSTAT_0) & 0x1);
						break;
	case CORE_FREQ_100M:HWREG(SOC_PLLC_0_REGS + PLLM_0 ) = 23;         //PLLM为24倍频
						HWREG(SOC_PLLC_0_REGS + POSTDIV_0) &= 0xfffffff0;
						HWREG(SOC_PLLC_0_REGS + POSTDIV_0) |=5;
						while( HWREG(SOC_PLLC_0_REGS + PLLSTAT_0) & 0x1);
						//Wait for the GOSTAT bit in PLLSTAT to clear to 0
						HWREG(SOC_PLLC_0_REGS + PLLDIV3_0) = (0x8000 | 4);   //EMIFA 20M
						HWREG(SOC_PLLC_0_REGS + PLLCMD_0) |=0x1;                     //Set the GOSET bit in PLLCMD to 1
						while( HWREG(SOC_PLLC_0_REGS + PLLSTAT_0) & 0x1);
						break;
	case CORE_FREQ_50M: HWREG(SOC_PLLC_0_REGS + PLLM_0 ) = 23;         //PLLM为24倍频
						HWREG(SOC_PLLC_0_REGS + POSTDIV_0) &= 0xfffffff0;
						HWREG(SOC_PLLC_0_REGS + POSTDIV_0) |=11;
						while( HWREG(SOC_PLLC_0_REGS + PLLSTAT_0) & 0x1);
						//Wait for the GOSTAT bit in PLLSTAT to clear to 0
						HWREG(SOC_PLLC_0_REGS + PLLDIV3_0) = (0x8000 | 4);   //EMIFA  10M
						HWREG(SOC_PLLC_0_REGS + PLLCMD_0) |=0x1;                     //Set the GOSET bit in PLLCMD to 1
						while( HWREG(SOC_PLLC_0_REGS + PLLSTAT_0) & 0x1);
						break;
	}
	sw_wait(128);
	HWREG(SOC_PLLC_0_REGS + PLLCTL_0) |=0x00000008;                 //Set the PLLRST bit in PLLCTL to 1
	sw_wait(2000);
	HWREG(SOC_PLLC_0_REGS + PLLCTL_0) |=0x00000001;     //Set the PLLEN bit in PLLCTL to 1 (removes PLL from bypass mode)
}
void PLL1_init(uiCoreFrequency)       //配成300M，DDR2就是150M，参照DDR2时钟框图，uiCoreFrequency为DDR2频率
{
	unsigned int uiPLLM,uiPOSTDIV,uiPLLDIV1,uiPLLDIV2;
	switch(uiCoreFrequency)
	{
	case 150:
		uiPLLM = 24;
		uiPOSTDIV = 1;
		uiPLLDIV1 = 1;
		uiPLLDIV2 = 2;
		break;
	case 50:
		uiPLLM = 24;
		uiPOSTDIV = 5;
		uiPLLDIV1 = 1;
		uiPLLDIV2 = 2;
		break;
	case 25:
		uiPLLM = 11;
		uiPOSTDIV = 23;
		uiPLLDIV1 = 1;
		uiPLLDIV2 = 2;
		break;
	}
	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_CFGCHIP3) &= ~(0x00000020);
	HWREG(SOC_PLLC_1_REGS + PLLCTL_1) &= ~(0x00000020);     //Clear the PLLENSRC bit in PLLCTL to 0
	HWREG(SOC_PLLC_1_REGS + PLLCTL_1) &= ~(0x00000200);
	HWREG(SOC_PLLC_1_REGS + PLLCTL_1) &= ~(0x00000001);
	sw_wait(150);
	HWREG(SOC_PLLC_1_REGS + PLLCTL_1) &= ~(0x00000008);
	HWREG(SOC_PLLC_1_REGS + PLLCTL_1) |=  (0x00000010);
	HWREG(SOC_PLLC_1_REGS + PLLCTL_1) &= ~(0x00000002);
	HWREG(SOC_PLLC_1_REGS + PLLCTL_1) &= ~(0x00000010);
	sw_wait(150);
	HWREG(SOC_PLLC_1_REGS + PLLM_1 ) = uiPLLM;
	HWREG(SOC_PLLC_1_REGS + POSTDIV_1) = 0x8000|uiPOSTDIV;
	while( HWREG(SOC_PLLC_1_REGS + PLLSTAT_1) & 0x1);
	HWREG(SOC_PLLC_1_REGS + PLLDIV1_1) = 0x8000|uiPLLDIV1;
	HWREG(SOC_PLLC_1_REGS + PLLDIV2_1) = 0x8000|uiPLLDIV2;
	HWREG(SOC_PLLC_1_REGS + PLLCMD_1) |= 0x00000001;
	while( HWREG(SOC_PLLC_1_REGS + PLLSTAT_1) & 0x1);
	sw_wait(128);
	HWREG(SOC_PLLC_1_REGS + PLLCTL_1) |= 0x8;
	sw_wait(2000);
	HWREG(SOC_PLLC_1_REGS + PLLCTL_1) |= 0x1;
	CSR	|=	0x1;
	ISTP = 0x00800000;//定位中断向量表到0x00800000
}
void sw_wait(unsigned int time_delay)
{
	while(time_delay--);
}

