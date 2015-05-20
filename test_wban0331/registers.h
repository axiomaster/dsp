/*
 * registers.h
 *
 *  Created on: 2014-2-26
 *      Author: Administrator
 */

#ifndef REGISTERS_H_
#define REGISTERS_H_
/***************************************************************************************/
/*	CPU registers					*/
cregister volatile unsigned int CSR;  /* Control Status Register  */
cregister volatile unsigned int IER;
cregister volatile unsigned int ISTP;
cregister volatile unsigned int IFR;
cregister volatile unsigned int AMR;
extern cregister volatile unsigned int CSR;  /* Control Status Register  */
extern cregister volatile unsigned int IER;
extern cregister volatile unsigned int ISTP;
extern cregister volatile unsigned int IFR;
extern cregister volatile unsigned int AMR;
/*********************************SOC******************************************/
#define SOC_EDMA30CC_0_REGS                 (0x01C00000)
#define SOC_EDMA30TC_0_REGS                 (0x01C08000)
#define SOC_EDMA30TC_1_REGS                 (0x01C08400)
#define SOC_PSC_0_REGS                      (0x01C10000)
#define SOC_PLLC_0_REGS                     (0x01C11000)
#define SOC_SYSCFG_0_REGS                   (0x01C14000)
#define SOC_TMR_0_REGS                      (0x01C20000)
#define SOC_TMR_1_REGS                      (0x01C21000)    //1做看门狗
#define SOC_I2C_0_REGS                      (0x01C22000)
#define SOC_SPI_0_REGS                      (0x01C41000)  //SPI0起始地址
#define SOC_MCBSP_0_CTRL_REGS               (0x01D10000)
#define SOC_MCBSP_0_FIFO_REGS               (0x01D10800)
#define SOC_MCBSP_0_DATA_REGS               (0x01F10000)
#define SOC_MCBSP_1_CTRL_REGS               (0x01D11000)
#define SOC_MCBSP_1_FIFO_REGS               (0x01D11800)
#define SOC_MCBSP_1_DATA_REGS               (0x01F11000)
#define SOC_UPP_REGS                        (0x01E16000)
#define SOC_PLLC_1_REGS                     (0X01E1A000)
#define SOC_GPIO_REGS                       (0x01E26000)
#define SOC_PSC_1_REGS                      (0x01E27000)
#define SOC_SYSCFG_1_REGS                   (0x01E2C000)
#define SOC_EDMA31CC_0_REGS                 (0x01E30000)
#define SOC_EDMA31TC_0_REGS                 (0x01E38000)
#define SOC_TMR_2_REGS                      (0x01F0C000)
#define SOC_TMR_3_REGS                      (0x01F0D000)
#define SOC_SPI_1_REGS                      (0x01F0E000)  //SPI1起始地址
#define SOC_DDR2_CTRL_REGS                  (0xB0000000)
#define SOC_DDR2_DATA_REGS                  (0xC0000000)
/*********************************SYSCFG0******************************************/
#define SYSCFG0_KICK0R		(0x38)
#define SYSCFG0_KICK1R		(0x3C)
#define SYSCFG0_PINMUX(n)	(0x120 + (n * 4))    //0~19
#define SYSCFG0_CFGCHIP0	(0x17C)
#define SYSCFG0_CFGCHIP1	(0x180)
#define SYSCFG0_CFGCHIP2	(0x184)
#define SYSCFG0_CFGCHIP3	(0x188)
#define SYSCFG0_CFGCHIP4	(0x18C)
/*************************PSC.REGISTER*****************************************/
/* NOTE
 * The actual number of MDCTL and MDSTAT register depend on number of
 * LPSC modules in a PSC. The number of MDCTL/MDSTAT registers defined
 * here would be a superset
 * e.g. PSC0 has 16 MDCTL/MDSTAT register, PSC1 has 32 MDCTL/MDSTAT
 * registers */
typedef enum {
    HW_PSC_CC0              = 0,
    HW_PSC_TC0              = 1,
    HW_PSC_TC1              = 2,
    HW_PSC_SPI0             = 4
} Psc0Peripheral;

typedef enum {
    HW_PSC_CC1          = 0,
    HW_PSC_GPIO         = 3,
    HW_PSC_DDR2_MDDR    = 6,
    HW_PSC_SPI1         = 10,
    HW_PSC_MCBSP0       = 14,
    HW_PSC_MCBSP1       = 15,
    HW_PSC_UPP          = 19,
    HW_PSC_TC2          = 21
} Psc1Peripheral;
#define PSC_POWERDOMAIN_ALWAYS_ON    0
#define PSC_PTCMD	(0x120)
#define PSC_PTSTAT	(0x128)
#define PSC_MDSTAT(n)	(0x800 + (n * 4))
#define PSC_MDCTL(n)	(0xA00 + (n * 4))
/******************************************************************************
**                      FIELD DEFINITION MACROS
******************************************************************************/
/* PTCMD */
#define PSC_PTCMD_GO1          (0x00000002u)
#define PSC_PTCMD_GO0          (0x00000001u)

/* PTSTAT */
#define PSC_PTSTAT_GOSTAT1     (0x00000002u)
#define PSC_PTSTAT_GOSTAT0     (0x00000001u)
/* MDSTAT */
#define PSC_MDSTAT_STATE       (0x0000003Fu)
/* MDCTL */
#define PSC_MDCTL_NEXT         (0x0000001Fu)
/*----NEXT Tokens----*/
#define PSC_MDCTL_NEXT_SWRSTDISABLE (0x00000000u)
#define PSC_MDCTL_NEXT_SYNCRST      (0x00000001u)
#define PSC_MDCTL_NEXT_DISABLE      (0x00000002u)
#define PSC_MDCTL_NEXT_ENABLE       (0x00000003u)
//*****************************************************************************
#define HWREG(x)                                                              \
        (*((volatile unsigned int *)(x)))
/***************************************************************************************/
//PLL registers
#define CORE_FREQ_300M   300
#define CORE_FREQ_150M   150
#define CORE_FREQ_100M   100
#define CORE_FREQ_50M    50
#define PLLCTL_0        (0x100)             //PLLC0 Control Register
#define PLLM_0          (0x110)             //PLLC0 PLL Multiplier Control Register
#define PREDIV_0        (0x114)             //PLLC0 Pre-Divider Control Register
#define PLLDIV1_0       (0x118)             //PLLC0 Divider 1 Register
#define PLLDIV2_0       (0x11C)             //PLLC0 Divider 2 Register
#define PLLDIV3_0       (0x120)             //PLLC0 Divider 3 Register
#define POSTDIV_0       (0x128)             //PLLC0 PLL Post-Divider Control Register
#define PLLCMD_0        (0x138)             //PLLC0 PLL Controller Command Register
#define PLLSTAT_0       (0x13C)             //PLLC0 PLL Controller Status Register
#define PLLDIV4_0       (0x160)             //PLLC0 Divider 4 Register
#define PLLDIV5_0       (0x164)             //PLLC0 Divider 5 Register
#define PLLDIV6_0       (0x168)             //PLLC0 Divider 6 Register
#define PLLDIV7_0       (0x16C)             //PLLC0 Divider 7 Register
#define DCHANGE_0       (0x144)             //PLLC0 PLLDIV Ratio Change Status Register
#define SYSTAT_0        (0x150)             //PLLC0 SYSCLK  Register

#define PLLCTL_1        (0x100)             //PLLC1 Control Register
#define PLLM_1	        (0x110)             //PLLC1 PLL Multiplier Control Register
#define PLLDIV1_1       (0x118)             //PLLC1 Divider 1 Register
#define PLLDIV2_1       (0x11C)             //PLLC1 Divider 2 Register
#define PLLDIV3_1       (0x120)             //PLLC1 Divider 3 Register
#define POSTDIV_1       (0x128)             //PLLC1 PLL Post-Divider Control Register
#define PLLCMD_1        (0x138)             //PLLC1 PLL Controller Command Register
#define PLLSTAT_1       (0x13C)             //PLLC1 PLL Controller Status Register
#define DCHANGE_1       (0x144)             //PLLC1 PLLDIV Ratio Change Status Register
#define SYSTAT_1        (0x150)             //PLLC1 SYSCLK  Register
/***************************************************************************************/
/*   GPIO registers              */
#define GPIO_REVID		(0x0)
#define GPIO_BINTEN		(0x8)
#define GPIO_DIR(n)		(0x10 + (0x28 * n))
#define GPIO_OUT_DATA(n)	(0x14 + (0x28 * n))
#define GPIO_SET_DATA(n)	(0x18 + (0x28 * n))     //就这个和下面这个寄存器用到了
#define GPIO_CLR_DATA(n)	(0x1C + (0x28 * n))    //用到了
#define GPIO_IN_DATA(n)		(0x20 + (0x28 * n))
#define GPIO_SET_RIS_TRIG(n)	(0x24 + (0x28 * n))
#define GPIO_CLR_RIS_TRIG(n)	(0x28 + (0x28 * n))
#define GPIO_SET_FAL_TRIG(n)	(0x2C + (0x28 * n))
#define GPIO_CLR_FAL_TRIG(n)	(0x30 + (0x28 * n))
#define GPIO_INTSTAT(n)		(0x34 + (0x28 * n))
#define GPIO_DIR_INPUT                1
#define GPIO_DIR_OUTPUT               0
#define GPIO_PIN_HIGH                 1
#define GPIO_PIN_LOW                  0
//GPIO PIN NUMBER
#define GP0_7             8
#define GP0_13            14
#define GP0_14            15
#define GP0_15            16
#define GP1_15            32
#define GP3_8             57
/***************************************************************************************/
typedef unsigned char Uint8;
typedef unsigned int Uint32;
typedef unsigned short Uint16;
/***************************************************************************************/
/* Interrupt Controller Registers */
#define INTC_BASE_ADDRESS	0x01800000
//每个类型的寄存器有三四个所以使用了define的函数形式使用x指定一组中具体哪个寄存器
#define INTC_EVTFLAGn(x)	*(volatile unsigned int*)(INTC_BASE_ADDRESS + x*0x04)
#define INTC_EVTSETn(x)		*(volatile unsigned int*)(INTC_BASE_ADDRESS + 0x20 + x*0x04)
#define INTC_EVTCLRn(x)		*(volatile unsigned int*)(INTC_BASE_ADDRESS + 0x40 + x*0x04)
#define INTC_EVTMASKn(x)	*(volatile unsigned int*)(INTC_BASE_ADDRESS + 0x80 + x*0x04)
#define INTC_MEVTFLAGn(x)	*(volatile unsigned int*)(INTC_BASE_ADDRESS + 0xA0 + x*0x04)
#define INTC_INTMUXn(x)		*(volatile unsigned int*)(INTC_BASE_ADDRESS + 0x100 + x*0x04)

//第一个参数是INT4-INT15，第二个参数是中断的编号，timer0的中断号是4。他们在下面有定义
inline INT_MUX(unsigned INTNUM, unsigned INTSEL)
{
	INTC_INTMUXn((unsigned int)(INTNUM/4)) &= ~(0xFF << (INTNUM%4)*8);
	INTC_INTMUXn((unsigned int)(INTNUM/4)) |=  (INTSEL << (INTNUM%4)*8);
}
/* INTNUM */
#define INT4	4
#define INT5	5
#define INT6	6
#define INT7	7
#define INT8	8
#define INT9	9
#define INT10	10
#define INT11	11
#define INT12	12
#define INT13	13
#define INT14	14
#define INT15	15
/* INTSEL */
#define TINTLO0		4
#define RINT0		87
#define XINT0       88
#define RINT1		89
#define XINT1       90
/***************************************************************************************/
/* EDMA3 registers */
#define PING 0
#define PONG 1
#define EDMA30CC_BASE_ADDR	0x01C00000
#define EDMA30CC_PID			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x0000)
#define EDMA30CC_CCCFG			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x0004)
#define EDMA30CC_QCHMAPn(x)		 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x0200+0x04*x)
#define EDMA30CC_DMAQNUMn(x)	 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x0240+0x04*x)
#define EDMA30CC_QMAQNUM		 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x0260)
#define EDMA30CC_ER			     *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1000)
#define EDMA30CC_ECR			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1008)
#define EDMA30CC_ESR			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1010)
#define EDMA30CC_CER			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1018)
#define EDMA30CC_EER			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1020)
#define EDMA30CC_EECR		     *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1028)
#define EDMA30CC_EESR		     *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1030)
#define EDMA30CC_IER			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1050)
#define EDMA30CC_IECR			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1058)
#define EDMA30CC_IESR			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1060)
#define EDMA30CC_IPR			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1068)
#define EDMA30CC_ICR			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1070)
#define EDMA30CC_IEVAL			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1078)

#define EDMA30CC_QER			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1080)
#define EDMA30CC_QEER			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1084)
#define EDMA30CC_QEECR			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1088)
#define EDMA30CC_QEESR			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x108C)
#define EDMA30CC_QSER			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1090)
#define EDMA30CC_QSECR			 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1094)

/************************************************************************************/
#define EDMA30CC_DRAEn(x)	     *(volatile unsigned int*)(0x02A00340   +    0x08*x)
#define EDMA30CC_QRAEn(x)	     *(volatile unsigned int*)(0x02A00380   +    0x04*x)
#define EDMA30CC_SR0_BASE_ADDR	0x01C01000
#define EDMA30CC_SR0_ER			 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1000)
#define EDMA30CC_SR0_ECR		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1008)
#define EDMA30CC_SR0_ESR		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1010)
#define EDMA30CC_SR0_CER		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1018)
#define EDMA30CC_SR0_EER		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1020)
#define EDMA30CC_SR0_EECR		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1028)
#define EDMA30CC_SR0_EESR		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1030)
#define EDMA30CC_SR0_IER		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1050)
#define EDMA30CC_SR0_IECR		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1058)
#define EDMA30CC_SR0_IESR		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1060)
#define EDMA30CC_SR0_IPR		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1068)
#define EDMA30CC_SR0_ICR		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1070)
#define EDMA30CC_SR0_IEVAL		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1078)

#define EDMA30CC_SR0_QER		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1080)
#define EDMA30CC_SR0_QEER		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1084)
#define EDMA30CC_SR0_QEECR		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x1088)
#define EDMA30CC_SR0_QEESR		 *(volatile unsigned int*)(EDMA30CC_SR0_BASE_ADDR+0x108C)
#define EDMA30CC_SR0_QSER		 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1090)
#define EDMA30CC_SR0_QSECR		 *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x1094)


/************************************************************************************/
#define EDMA30CC_EMCR		     *(volatile unsigned int*)(EDMA30CC_BASE_ADDR+0x0308)
/*|----------------------------------------------------------------|
 *|                Channel Options Parameter (OPT)                 |
 *|                  Channel Source Address (SRC)                  |
 *| Count for 2nd Dimension (BCNT)|Count for 1st Dimension (ACNT)  |
 *|               Channel Destination Address (DST)                |
 *|    Destination BCNT Index     |  Source BCNT Index (SRCBIDX)   |
 *|           (DSTBIDX)           |                                |
 *|    BCNT Reload (BCNTRLD)      |  Link Address (LINK)           |
 *|    Destination CCNT Index     |  Source CCNT Index (SRCCIDX)   |
 *|           (DSTCIDX)           |                                |
 *|           Reserved            |  Count for 3rd Dimension (CCNT)|
 *| ---------------------------------------------------------------|
 */
struct EDMA30CC_PaRAM
{
	unsigned int OPT;
	unsigned int SRC;
	unsigned int BCNT_ACNT;
	unsigned int DST;
	unsigned int DSTBIDX_SRCBIDX;
	unsigned int BCNTRLD_LINK;
	unsigned int DSTCIDX_SRCCIDX;
	unsigned int RSVD_CCNT;
};
#define EDMA30CC_PaRAMn(x)	(struct EDMA30CC_PaRAM*)(EDMA30CC_BASE_ADDR+0x4000+0x20*(x))

#define EDMA30TC0_BASE_ADDR	0x01C08000
#define EDMA30TC0_RDRATE	*(volatile unsigned int*)(EDMA30TC0_BASE_ADDR+0x0140)


/* some useful macro functions */
inline void EDMA_REGIN_ENA(unsigned int REGIN)
{
	EDMA30CC_DRAEn(REGIN)  = 0x00000000;
}
inline void EVENT_ENABLE(unsigned int EVTNUM)
{
	EDMA30CC_EESR |= (1 << EVTNUM);
}
inline void EVENT_DISABLE(unsigned int EVTNUM)
{
	EDMA30CC_DRAEn(0)  = 0xFFFFFFFF;
	EDMA30CC_EECR |= (1 << EVTNUM);
}
inline void EVENT_DIABLE_ALL(void)
{
	EDMA30CC_EECR = 0xFFFFFFFF;
	EDMA30CC_QEECR |= 0x0000000F;
}
inline void EVENT_CLEAR_ALL(void)
{
	EDMA30CC_ECR = 0xFFFFFFFF;
}
inline void EVENT_CLEAR(unsigned int EVTNUM)
{
	EDMA30CC_ECR |= (1 << EVTNUM);
}
inline void CHANNEL_QUEUE(unsigned int EVTNUM, unsigned char QNUM)
{
	EDMA30CC_DMAQNUMn((unsigned)(EVTNUM/8)) &= ~(  7 << (EVTNUM%8)*4);
	EDMA30CC_DMAQNUMn((unsigned)(EVTNUM/8)) |= (QNUM << (EVTNUM%8)*4);
}
inline void QDMA_REGIN_ENA(unsigned int REGIN)
{
	EDMA30CC_QRAEn(REGIN) = 0x00000000;
}
inline void QEVENT_ENABLE(unsigned int QEVTNUM)
{
	EDMA30CC_QEESR |= 1 << QEVTNUM;
}
inline void QEVENT_DISABLE(unsigned int QEVTNUM)
{
	EDMA30CC_QEECR |= 1 << QEVTNUM;
}
inline void QCHANNEL_QUEUE(unsigned int QEVTNUM, unsigned char QNUM)
{
	EDMA30CC_QMAQNUM &= ~(  7 << (QEVTNUM)*4);
	EDMA30CC_QMAQNUM |= (QNUM << (QEVTNUM)*4);
}
inline void QCHANNEL_MAP(unsigned int EVTNUM, unsigned int PaRAMSET, unsigned int TRWORD)
{
	EDMA30CC_QCHMAPn(EVTNUM) = 0;
	EDMA30CC_QCHMAPn(EVTNUM) = (PaRAMSET << 5) | ((TRWORD << 2) & 0x0000001f);
}
inline void DMA_INT_ENA(unsigned int INTNUM)
{
	EDMA30CC_IESR |= (1 << INTNUM);
}
/* OPT Channel Option Parameter */
#define OPT_PRIVID(x)	    (x << 24)
#define OPT_ITCCHEN_DIS	    (0 << 23)
#define OPT_ITCCHEN_EN	    (1 << 23)
#define OPT_TCCHEN_DIS	    (0 << 22)
#define OPT_TCCHEN_EN	    (1 << 22)
#define OPT_ITCINTEN_DIS	(0 << 21)
#define OPT_ITCINTEN_EN		(1 << 21)
#define OPT_TCINTEN_DIS		(0 << 20)
#define OPT_TCINTEN_EN		(1 << 20)
#define OPT_TCC(x)			(x << 12)
#define OPT_TCCMODE_NORMAL	(0 << 11)
#define OPT_TCCMODE_EARLY	(1 << 11)
#define OPT_TCCMODE_FWID(x)	(x << 8)
#define OPT_STATIC_NOT		(0 << 3)
#define OPT_STATIC_YES		(1 << 3)
#define OPT_SYNCDIM_A		(0 << 2)
#define OPT_SYNCDIM_AB		(1 << 2)
#define OPT_DAM_INCR		(0 << 1)
#define OPT_DAM_CONST		(1 << 1)
#define OPT_SAM_INCR		(0 << 0)
#define OPT_SAM_CONST		(1 << 0)
/*  */

/* EDMA3 Channel Synchronization Events */
/* 凡是使用EVTNUM的地方都可以使用这些宏 */
#define REVT0		2   //McBSP0
#define XEVT0		3
#define REVT1		4   //McBSP1
#define XEVT1		5
#define TEVTLO0		10	//timer0
#define TEVTHI0		11

#define QCH0		0
#define QCH1		1
#define QCH3		2
#define QCH4		3
#define QCH5		5
#define QCH6		6
#define QCH7		7
#define QCH8		8

#define QUEUE0		0
#define QUEUE1		1

#define REGIN0		0
#define REGIN1		1
/***************************************************************************************/
/* McBSP register */

struct McbspRegs {
    volatile unsigned int DRR;
    volatile unsigned int DXR;
    volatile unsigned int SPCR;
    volatile unsigned int RCR;
    volatile unsigned int XCR;
    volatile unsigned int SRGR;
    volatile unsigned int MCR;
    volatile unsigned int RCERE0;
    volatile unsigned int XCERE0;
    volatile unsigned int PCR;
    volatile unsigned int RCERE1;
    volatile unsigned int XCERE1;
    volatile unsigned int RCERE2;
    volatile unsigned int XCERE2;
    volatile unsigned int RCERE3;
    volatile unsigned int XCERE3;
};//不要更改
#define MCBSP_0_BASE_ADDR	(struct McbspRegs*)(SOC_MCBSP_0_CTRL_REGS)
#define MCBSP_1_BASE_ADDR	(struct McbspRegs*)(SOC_MCBSP_1_CTRL_REGS)
/*******************************TIMER registers*******************************************/
#define TMR_TIM12	(0x10)
#define TMR_TIM34	(0x14)
#define TMR_PRD12	(0x18)
#define TMR_PRD34	(0x1C)
#define TMR_TCR		(0x20)
#define TMR_TGCR	(0x24)
#define TMR_WDTCR	(0x28)
#define TMR_INTCTLSTAT	(0x44)
//TGCR 中一些有用的标志位宏
#define TIMMODE_64_GP	(0 << 2)
#define TIM34RS_NOT		(1 << 1)
#define TIM12RS_NOT		(1 << 0)
/* TCR中一些有用的宏定义 */
#define CLKSRC_12	(0 << 8)
#define ENAMODE_12	(2 << 6)

/*******************************SPI1 registers*******************************************/
#define SPI_SPIGCR0		*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x00)
#define SPI_SPIGCR1		*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x04)
#define SPI_SPIINT0		*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x08)
#define SPI_SPILVL		*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x0C)
#define SPI_SPIFLG		*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x10)
#define SPI_SPIPC(n)	*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x14 + (4 * n))
#define SPI_SPIDAT0		*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x38)
#define SPI_SPIDAT1		*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x3C)
#define SPI_SPIBUF		*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x40)
#define SPI_SPIEMU		*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x44)
#define SPI_SPIDELAY	*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x48)
#define SPI_SPIDEF		*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x4C)
#define SPI_SPIFMT(n)	*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x50 + (n * 4))
#define SPI_INTVEC1		*(volatile unsigned int*)(SOC_SPI_1_REGS + 0x64)

/*******************************SPI0 registers*******************************************/
#define SPI0_SPIGCR0		*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x00)
#define SPI0_SPIGCR1		*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x04)
#define SPI0_SPIINT0		*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x08)
#define SPI0_SPILVL		*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x0C)
#define SPI0_SPIFLG		*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x10)
#define SPI0_SPIPC(n)	*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x14 + (4 * n))
#define SPI0_SPIDAT0		*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x38)
#define SPI0_SPIDAT1		*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x3C)
#define SPI0_SPIBUF		*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x40)
#define SPI0_SPIEMU		*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x44)
#define SPI0_SPIDELAY	*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x48)
#define SPI0_SPIDEF		*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x4C)
#define SPI0_SPIFMT(n)	*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x50 + (n * 4))
#define SPI0_INTVEC1		*(volatile unsigned int*)(SOC_SPI_0_REGS + 0x64)


static Uint32 spidat1 = 0x10000000;
#define SPI_SPIBUF_RXDATA   (0x0000ffffu)
#endif /* REGISTERS_H_ */
