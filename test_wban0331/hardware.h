/*
 * hardware.h
 *
 *  Created on: 2014-2-26
 *      Author: Administrator
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_
#include "registers.h"

int PSCModuleControl (unsigned int baseAdd, unsigned int moduleId,
                      unsigned int powerDomain, unsigned int flags);//psc里的函数
void sw_wait(unsigned int time_delay);//pll里的函数
void psc();
void pinmux();
void PLL0_init(unsigned int uiCoreFrequency);
void SPI0_init();
void SPI1_init();
void GPIO_init(void);
void GPIODirModeSet(unsigned int pinNumber,unsigned int pinDir);//GPI0里的函数
void GPIOPinWrite(unsigned int pinNumber, unsigned int bitValue);//GPI0里的函数
void EDMA3_SETUP(unsigned int EVTNUM, unsigned char QNUM, unsigned int PaRAMSET);
void EDMA3_PaRAM_SETUP(unsigned int PaRAMSET, struct EDMA30CC_PaRAM *pEDMA);
//void McBSP1_init();//EDMA、McBSP可能这些接口还没有用到
void spiflash_cycletx(Uint8 *buf,int len);//SPI1里的函数
void spiflash_cyclerx(Uint8 *buf);//SPI1里的函数
void spi0flash_cycletx(Uint16 *buf,Uint16 len);//SPI0里的函数
void spi0flash_cyclerx(Uint16 *buf);
#endif /* HARDWARE_H_ */
