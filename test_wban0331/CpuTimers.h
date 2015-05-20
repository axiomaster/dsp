/*********************************************************************
 *
 *                  CpuTimers Header File(TMS320C6748)
 *
 *********************************************************************
 * FileName:        CpuTimers.h
 * Company:         Xidian University
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Yueyang Song       2013.10.04
 *Yueyang Song		 2014.11.22
 ********************************************************************/
#ifndef _CpuTimers
#define _CpuTimers
#include "Wban.h"

//TMS320C6748定时器的各个寄存器定义
typedef union _TCR_REG{
	DWORD Val;
	struct _bits_TCR_REG{
		DWORD TSTAT12			:1;
		DWORD INVOUTP12			:1;
		DWORD INVINP12			:1;
		DWORD CP12				:1;
		DWORD PWID12			:2;
		DWORD ENAMODE12			:2;
		DWORD CLKSRC12			:1;
		DWORD TIEN12			:1;
		DWORD READRSTMODE12		:1;
		DWORD CAPMODE12			:1;
		DWORD CAPEVTMODE12		:2;
		DWORD 					:8;
		DWORD ENAMODE34			:2;
		DWORD 					:2;
		DWORD READRSTMODE34		:1;
		DWORD 					:5;
	}bit;
}TCR_REG;

typedef union _TGCR_REG{
	DWORD Val;
	struct _bits_TGCR_REG{
		DWORD TIM12RS			:1;
		DWORD TIM34RS			:1;
		DWORD TIMMODE			:2;
		DWORD PLUSEN			:1;
		DWORD fill1				:3;
		DWORD PSC34				:4;
		DWORD TDDR34			:4;
		DWORD 					:16;
	}bit;
}TGCR_REG;

typedef union _TIM_REG{
	uint64_t Val;
	struct __TIM_REG{
		DWORD TIM12;
		DWORD TIM34;
	}dword;
}TIM_REG;

typedef union _PRD_REG{
	uint64_t Val;
	struct __PRD_REG{
		DWORD PRD12;
		DWORD PRD34;
	}dword;
}PRD_REG;

struct CPUTIMERS_REGS{
	DWORD REVID;
	DWORD EMUMGT;
	DWORD GPINTGPEN;
	DWORD GPDATGPDIR;
	TIM_REG TIM;
	PRD_REG PRD;
	TCR_REG TCR;
	TGCR_REG TGCR;
	DWORD WDTCR;
	DWORD fill[2];
	DWORD REL12;
	DWORD REL34;
};

struct CPUTIMERS_VARS{
	volatile struct CPUTIMERS_REGS *RegsAddr;	//寄存器地址
	float CPUFreqInMHz;
	float PeridInUSec;
};

typedef union _TICK{
	uint64_t Val;
	struct _TICK_bytes{
		BYTE b0;
		BYTE b1;
		BYTE b2;
		BYTE b3;
		BYTE b4;
		BYTE b5;
		BYTE b6;
		BYTE b7;
	}byte;
}TICK;

extern struct CPUTIMERS_VARS mac_CpuTimer0;
extern struct CPUTIMERS_VARS mac_CpuTimer1;
//参照手册得到的定时器的地址
#define mac_Timer0Addr (volatile struct CPUTIMERS_REGS*)0x01c20000
#define mac_Timer1Addr (volatile struct CPUTIMERS_REGS*)0x01c21000

void InitCpuTimers(void);
void ConfigCpuTimer(struct CPUTIMERS_VARS *Timer, float Freq, float Period);
TICK TickGet(void);

/*************************************************************************/
//定时器0的基本操作
// 开始定时器
#define StartCpuTimer0()  	mac_CpuTimer0.RegsAddr->TCR.bit.ENAMODE12 = 1
// 暂停定时器
#define StopCpuTimer0()   	mac_CpuTimer0.RegsAddr->TCR.bit.ENAMODE12 = 0
// 读取定时器当前的时间值
#define ReadCpuTimer0() 	mac_CpuTimer0.RegsAddr->TIM.Val

//定时器1的基本操作
// 开始定时器:
#define StartCpuTimer1()  		mac_CpuTimer1.RegsAddr->TCR.bit.ENAMODE12 = 1; mac_CpuTimer1.RegsAddr->TCR.bit.ENAMODE12 = 0; mac_CpuTimer1.RegsAddr->TCR.bit.ENAMODE12 = 1
// 暂停定时器
#define StopCpuTimer1()  		mac_CpuTimer1.RegsAddr->TCR.bit.ENAMODE12 = 0
// 读取定时器当前的时间值
#define ReadCpuTimer1() 		mac_CpuTimer1.RegsAddr->TIM.Val
// 读取PRD当前的值
#define ReadCpuTimer1Period() 	mac_CpuTimer1.RegsAddr->PRD.Val
/**********************************************************************/
//得到两个时间点之间的时间值
#define TickGetDiff(a,b) (b.Val - a.Val)

#endif
