#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SYSTICK_COUNT_PER_US 168
#define SYSTICK_RELOAD_VAL 168000

void systickConfig(void);
/* 
   @function return millisecond elapsed from start
   */
uint32_t systickMillis(void);

uint32_t systickMicros(void);

void systickDelay_ms(uint32_t ms);

void systickDelay_us(uint32_t us);

void systickAttachCallback(void (*callback)(void));

#ifdef __cplusplus
}
#endif

#endif
