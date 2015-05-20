

#ifndef HARDWARE_H_
#define HARDWARE_H_
extern unsigned int flag;
int PSCModuleControl (unsigned int baseAdd, unsigned int moduleId,
                      unsigned int powerDomain, unsigned int flags);
void sw_wait(unsigned int time_delay);
void psc();
void pinmux();
void PLL0_init(unsigned int uiCoreFrequency);
void SPI0_init();
void GPIO_int_init(void);
void EDMA3_SETUP(unsigned int EVTNUM, unsigned char QNUM, unsigned int PaRAMSET);
void EDMA3_PaRAM_SETUP(unsigned int PaRAMSET, struct EDMA30CC_PaRAM *pEDMA);
void McBSP1_init();
void disable_int();
void enable_int();
void spiflash_cycle(Uint16 *buf,Uint16 len);
extern  unsigned int flag ;
#endif /* HARDWARE_H_ */
