/*********************************************************************
 *
 *                  WbanPHY Header File
 *
 *********************************************************************
 * FileName:        WbanPHY.h
 * Company:         Xidian University
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Yueyang Song       2013.10.02
 *Yueyang Song 		 2013.11.18		v1.2
 *Yueyang Song		 2014.11.22		v2.0
 ********************************************************************/
#ifndef _WbanPHY
#define _WbanPHY
#include "Wban.h"
#include "WbanTasks.h"


#define DATA_RATE_121_4 0
#define DATA_RATE_242_9 1
#define DATA_RATE_485_7 2
#define DATA_RATE_971_4 3

extern BYTE mac_RxBuffer[];

WBAN_PRIMITIVE PHYTasks(WBAN_PRIMITIVE inputPrimitive);
WBAN_PRIMITIVE dataIndicationProcess();
BYTE CCA();
BYTE Scan(BYTE scanType, BYTE channelFreBand, BYTE ChannelNum);
BYTE getMACFullAddr(BYTE n);
BOOL spiflash_cycletest(void);
#endif
