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
                      unsigned int powerDomain, unsigned int flags);//psc��ĺ���
void sw_wait(unsigned int time_delay);//pll��ĺ���
void psc();
void pinmux();
void PLL0_init(unsigned int uiCoreFrequency);
void SPI0_init();
void SPI1_init();
void GPIO_init(void);
void GPIODirModeSet(unsigned int pinNumber,unsigned int pinDir);//GPI0��ĺ���
void GPIOPinWrite(unsigned int pinNumber, unsigned int bitValue);//GPI0��ĺ���
void EDMA3_SETUP(unsigned int EVTNUM, unsigned char QNUM, unsigned int PaRAMSET);
void EDMA3_PaRAM_SETUP(unsigned int PaRAMSET, struct EDMA30CC_PaRAM *pEDMA);
//void McBSP1_init();//EDMA��McBSP������Щ�ӿڻ�û���õ�
void spiflash_cycletx(Uint8 *buf,int len);//SPI1��ĺ���
void spiflash_cyclerx(Uint8 *buf);//SPI1��ĺ���
void spi0flash_cycletx(Uint16 *buf,Uint16 len);//SPI0��ĺ���
void spi0flash_cyclerx(Uint16 *buf);
#endif /* HARDWARE_H_ */
