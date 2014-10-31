#include "systick.h"
#include "stm32f4xx.h"

static volatile uint32_t ms_ever_elapsed = 0;
static void (*systick_user_callback)(void);

void systickConfig(void)
{
	if(SysTick_Config(SYSTICK_RELOAD_VAL))
	{
		while(1);
	}
}

uint32_t systickMillis(void)
{
	return ms_ever_elapsed;
}

uint32_t systickMicros(void)
{
	uint32_t ms;
	uint32_t cycle_cnt;
	do{
		ms = systickMillis();
		cycle_cnt = SysTick->VAL;
	}while(ms != systickMillis());

	return (ms*1000 + (SYSTICK_RELOAD_VAL-cycle_cnt)/SYSTICK_COUNT_PER_US);
}

void systickDelay_ms(uint32_t ms)
{
	uint32_t target_ms = systickMillis() + ms;
	while(systickMillis() < target_ms);
}

void systickDelay_us(uint32_t us)
{
	int32_t target_tick = SysTick->VAL - us*SYSTICK_COUNT_PER_US;
	if(target_tick < 0)
		target_tick = 0;
	for(;;)
	{
		if(SysTick->VAL <= target_tick)
			return;
	}
}

void systickAttachCallback(void (*callback)(void))
{
		systick_user_callback = callback;
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
		ms_ever_elapsed ++;
		if(systick_user_callback)
			systick_user_callback();
}
