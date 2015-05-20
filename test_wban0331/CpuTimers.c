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
**����ԭ��:		void InitCpuTimers(void)
**��ڲ���:   	void
**��   �� 	ֵ��		void
**˵      	����		��ʼ����ʱ��Timer0��Timer1������Timer0��Ϊϵͳʱ�ӣ�
				Timer1��Ϊ��Ҫʱ�Ķ�ʱ��
************************************************************************/
void InitCpuTimers(void)
{
#ifdef DEBUG
	printf("\n��ʼ��ϵͳʱ��... @InitCpuTimers()\n");
#endif
	//Timer0�ṩȫ��ϵͳʱ�ӣ��ӳ���ʼ������һֱ����
	mac_CpuTimer0.RegsAddr = mac_Timer0Addr;
	mac_CpuTimer0.RegsAddr->TCR.bit.ENAMODE12 = 0;
	mac_CpuTimer0.RegsAddr->TGCR.bit.TIMMODE = 0;
	mac_CpuTimer0.RegsAddr->TGCR.bit.TIM12RS = 0;
	mac_CpuTimer0.RegsAddr->TGCR.bit.TIM34RS = 0;
	mac_CpuTimer0.RegsAddr->PRD.Val = 0xFFFFFFFFFFFFFFFF;
	mac_CpuTimer0.RegsAddr->TGCR.bit.TIM12RS = 1;
	mac_CpuTimer0.RegsAddr->TGCR.bit.TIM34RS = 1;
	mac_CpuTimer0.RegsAddr->TCR.bit.ENAMODE12 = 1;

	//Timer1�ṩ����Ҫʱ�ļ�����������ʱ��
	//mac_CpuTimer1.RegsAddr = mac_Timer1Addr;
	//mac_CpuTimer1.RegsAddr->TGCR.bit.TIMMODE = 0;
}

/**********************************************************************
**����ԭ��:		void ConfigCpuTimer(struct CPUTIMERS_VARS *Timer, float Freq, float Period)
**��ڲ���:   	struct CPUTIMERS_VARS *Timer, ��Ҫ�趨�Ķ�ʱ��Timer
   				float Freq, ��ǰϵͳʱ��Ƶ�ʣ���λMHz
   				float Period�� ��Ҫ�趨��ʱ�䣬��λus
**��   �� 	ֵ��		void
**˵      	����		��ָ���Ķ�ʱ���趨Ϊָ����ʱ��
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
**����ԭ��:		TICK TickGet(void)
**��ڲ���:   	void
**��   �� 	ֵ��		��ǰTimer0��ʱ��ֵ
**˵      	����		�õ���ǰ��ϵͳʱ��
***********************************************************************/
TICK TickGet(void){
	TICK currentTime;

	StopCpuTimer0();
	currentTime.Val = ReadCpuTimer0();
	StartCpuTimer0();

	return currentTime;
}



