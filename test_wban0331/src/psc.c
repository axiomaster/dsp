/*
 * psc.c
 *
 *  Created on: 2014-4-21
 *      Author: zhangxiaoqian
 */
/**************************************************************************
  API FUNCTION DEFINITIONS
***************************************************************************/

/**
 *
 *  \brief   This function sets the requested module in the required state
 *
 * \param     baseAdd         PSC0 or PSC1 ,PSC_POWERDOMAIN_ALWAYS_ON
 * \param     moduleId        The module number of the module to be commanded.
 * \param     powerDomain     PD0 or PD1
 * \param     flags           This contains the flags that is a logical OR of
 *                            the commands that can be given to a module.
 *
 * \return                    0 in case of successful transition, -1 otherwise.
 *
 */
/**
* PD0Ҳ�ɳ�Ϊ��Always ON domain����оƬ�ϵ������һֱ����ON״̬������ͨ����̽���ת��ΪOFF״̬
*��ģ�鴦��Disable��SwRstDisable״̬ʱ��ģ���ʱ�ӱ��ر�/����
*Ҫ���ģ���ܶ��ⲿ����������Ӧ�����뽫��ģ���������ó�Enable״̬
*/
#include "../registers.h"
#include "../hardware.h"
void psc()
{
    PSCModuleControl(SOC_PSC_0_REGS, 0, 0, PSC_MDCTL_NEXT_ENABLE );  //EDMA3_0_CC0
    PSCModuleControl(SOC_PSC_0_REGS, 1, 0, PSC_MDCTL_NEXT_ENABLE );  //EDMA3_0_TC0
    PSCModuleControl(SOC_PSC_0_REGS, 2, 0, PSC_MDCTL_NEXT_ENABLE );  //EDMA3_0_TC1
    PSCModuleControl(SOC_PSC_0_REGS, 3, 0, PSC_MDCTL_NEXT_ENABLE );  //EMIFA�ϵ�
    PSCModuleControl(SOC_PSC_0_REGS, 4, 0, PSC_MDCTL_NEXT_ENABLE );  //SPI0,��flash����дʱʹ��
    //PSCModuleControl(SOC_PSC_1_REGS, 0, 0, PSC_MDCTL_NEXT_ENABLE );  //EDMA3_1_CC1,����
    PSCModuleControl(SOC_PSC_1_REGS, 3, 0, PSC_MDCTL_NEXT_ENABLE );  //GPIO
    PSCModuleControl(SOC_PSC_1_REGS, 10, 0, PSC_MDCTL_NEXT_ENABLE );  //SPI1,��FPGA
    PSCModuleControl(SOC_PSC_1_REGS, 14, 0, PSC_MDCTL_NEXT_ENABLE ); //McBSP0
    //PSCModuleControl(SOC_PSC_1_REGS, 15, 0, PSC_MDCTL_NEXT_ENABLE ); //McBSP1
    //PSCModuleControl(SOC_PSC_1_REGS, 21, 0, PSC_MDCTL_NEXT_ENABLE ); //EDMA3_1_TC1,����
}
int PSCModuleControl (unsigned int baseAdd, unsigned int moduleId,
                         unsigned int powerDomain, unsigned int flags)
{
    volatile unsigned int timeout = 0xFFFFFF;
    int    retVal = 0;
    unsigned int    status = 0;

    HWREG(baseAdd +  PSC_MDCTL(moduleId)) = (flags & PSC_MDCTL_NEXT);

    if (powerDomain == 0)
    {
        HWREG(baseAdd + PSC_PTCMD) = PSC_PTCMD_GO0;
    }
    else
    {
        HWREG(baseAdd + PSC_PTCMD) = PSC_PTCMD_GO1;
    }

    if (powerDomain == 0)
    {
        do {
            status = HWREG(baseAdd + PSC_PTSTAT) & PSC_PTSTAT_GOSTAT0;
        } while (status && timeout--);
    }
    else
    {
        do {
            status = HWREG(baseAdd + PSC_PTSTAT) & PSC_PTSTAT_GOSTAT1;
        } while (status && timeout--);
    }

    if (timeout != 0)
    {
        timeout = 0xFFFFFF;
        status = flags & PSC_MDCTL_NEXT;
        do {
            timeout--;
        } while(timeout &&
                (HWREG(baseAdd + PSC_MDSTAT(moduleId)) & PSC_MDSTAT_STATE) != status);
    }

    if (timeout == 0)
    {
        retVal = -1;
    }

    return retVal;
}




