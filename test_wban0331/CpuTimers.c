/*********************************************************************
 *
 *                  CpuTimers C File(TMS320C6748)
 *
 *********************************************************************
 * FileName:        CpuTimers.c
 * Company:         Xidian University
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *Yueyang Song       2013.10.04
 *Yueyang Song		 2014.11.22
 ********************************************************************/
#include "CpuTimers.h"
#include "Wban.h"

struct CPUTIMERS_VARS mac_CpuTimer0;
struct CPUTIMERS_VARS mac_CpuTimer1;

/**********************************************************************
**函数原型:		void InitCpuTimers(void)
**入口参数:   	void
**返   回 	值：		void
**说      	明：		初始化定时器Timer0和Timer1，其中Timer0作为系统时钟，
				Timer1作为需要时的定时器
************************************************************************/
void InitCpuTimers(void)
{
#ifdef DEBUG
	printf("\n初始化系统时钟... @InitCpuTimers()\n");
#endif
	//Timer0提供全局系统时钟，从程序开始到结束一直存在
	mac_CpuTimer0.RegsAddr = mac_Timer0Addr;
	mac_CpuTimer0.RegsAddr->TCR.bit.ENAMODE12 = 0;
	mac_CpuTimer0.RegsAddr->TGCR.bit.TIMMODE = 0;
	mac_CpuTimer0.RegsAddr->TGCR.bit.TIM12RS = 0;
	mac_CpuTimer0.RegsAddr->TGCR.bit.TIM34RS = 0;
	mac_CpuTimer0.RegsAddr->PRD.Val = 0xFFFFFFFFFFFFFFFF;
	mac_CpuTimer0.RegsAddr->TGCR.bit.TIM12RS = 1;
	mac_CpuTimer0.RegsAddr->TGCR.bit.TIM34RS = 1;
	mac_CpuTimer0.RegsAddr->TCR.bit.ENAMODE12 = 1;

	//Timer1提供有需要时的计数器，作定时用
	//mac_CpuTimer1.RegsAddr = mac_Timer1Addr;
	//mac_CpuTimer1.RegsAddr->TGCR.bit.TIMMODE = 0;
}

/**********************************************************************
**函数原型:		void ConfigCpuTimer(struct CPUTIMERS_VARS *Timer, float Freq, float Period)
**入口参数:   	struct CPUTIMERS_VARS *Timer, 需要设定的定时器Timer
   				float Freq, 当前系统时钟频率，单位MHz
   				float Period， 需要设定的时间，单位us
**返   回 	值：		void
**说      	明：		将指定的定时器设定为指定的时间
************************************************************************/
void ConfigCpuTimer(struct CPUTIMERS_VARS *Timer, float Freq, float Period){
	uint64_t temp;
	Timer->CPUFreqInMHz = Freq;
	Timer->PeridInUSec = Period;
	temp = (uint64_t)(Freq * Period);

	Timer->RegsAddr->TCR.bit.ENAMODE12 = 0;
	Timer->RegsAddr->TGCR.bit.TIM12RS = 0;
	Timer->RegsAddr->TGCR.bit.TIM34RS = 0;
	Timer->RegsAddr->PRD.Val = temp;
	Timer->RegsAddr->TGCR.bit.TIM12RS = 1;
	Timer->RegsAddr->TGCR.bit.TIM34RS = 1;
}

/**********************************************************************
**函数原型:		TICK TickGet(void)
**入口参数:   	void
**返   回 	值：		当前Timer0的时间值
**说      	明：		得到当前的系统时间
***********************************************************************/
TICK TickGet(void){
	TICK currentTime;

	StopCpuTimer0();
	currentTime.Val = ReadCpuTimer0();
	StartCpuTimer0();

	return currentTime;
}



