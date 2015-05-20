/*********************************************************************
 *
 *                  Wban Header File
 *
 *********************************************************************
 * FileName:        Wban.h
 * Company:         Xidian University
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Yueyang Song       2013.10.14
 *Yueyang Song		 2014.11.22
 ********************************************************************/
#ifndef _Wban
#define _Wban

#define DEBUG
#define DEBUG1
#define I_AM_HUB
#define mac_CLK_Freq 300	//MHz，系统时钟频率
#define MAC_FULL_ADDRESS {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}	//11:22:33:44:55:66
#define WBAN_BAND_2400M_2483M
#include <stdio.h>


#ifndef _stdint
#define _stdint
#include <stdint.h>
#define BYTE	uint8_t
#define WORD	uint16_t
#define DWORD	uint32_t
#define BOOL	uint8_t
#define TRUE	1
#define FALSE 	0
#endif

#define mac_FRAME_TRANSACTION_TIME 2300	//us   这个应该动态调整
/*********************************************************************
*EUI-48长地址和简明地址的定义
*以及简明地址的取值含义
**********************************************************************/
typedef struct _FULL_ADDR{//EUI-48,完整的MAC地址
	BYTE v[6];
}FULL_ADDR;

typedef struct _ABBR_ADDR{//abbreviated address,简明地址
	BYTE v;
}ABBR_ADDR;

#endif
