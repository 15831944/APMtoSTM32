#include "stm32f4bsp.h"
#include "stm32f4xx.h"
#include <stdio.h>

static volatile uint32_t tempISER[8]={0};

void Stm32f4BspInit(void)
{
		if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
				RCC_ClearFlag();
}

void SET_INTERRUPT_MASK(void)
{
		uint8_t i;
		for(i=0; i<8; i++)
		{
				tempISER[i] = NVIC->ISER[i];
				NVIC->ICER[i] = 0xffffffff;
		}
}

void CLEAR_INTERRUPT_MASK(void)
{
		uint8_t i;
		for(i=0; i<8; i++)
		{
				NVIC->ISER[i] = tempISER[i];
		}
}

void bspEnableIRQChannel(uint8_t nvic_irq_channel)
{
    NVIC->ISER[nvic_irq_channel >> 0x05] =
      (uint32_t)0x01 << (nvic_irq_channel & (uint8_t)0x1F);
}

void bspDisableIRQChannel(uint8_t nvic_irq_channel)
{
    NVIC->ICER[nvic_irq_channel >> 0x05] =
      (uint32_t)0x01 << (nvic_irq_channel & (uint8_t)0x1F);
}

void Stm32f4BspReboot(void)
{
		IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
		IWDG_SetPrescaler(IWDG_Prescaler_32);
		IWDG_SetReload(1000);
		IWDG_ReloadCounter();
		IWDG_Enable();
}

//re-write fputc function
/*
int fputc(int ch, FILE *f)
{
		return ITM_SendChar(ch);
}
*/

