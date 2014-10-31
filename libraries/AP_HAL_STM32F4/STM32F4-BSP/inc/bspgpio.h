#ifndef _BSPGPIO_H_
#define _BSPGPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f4xx.h"

#define BSP_GPIO_PIN_NUM 7
#ifndef NULL
#define NULL 0
#endif

struct pin_info_s
{
	GPIO_TypeDef * gpio_group;
	uint16_t pin;
	uint32_t gpio_rcc;
	uint8_t exti_port_source;
	uint8_t exti_pin_source;
	uint32_t exti_line;
	IRQn_Type irq_channel;
};

typedef struct pin_info_s pin_info_t;

extern const pin_info_t bsp_pin_map[BSP_GPIO_PIN_NUM];

void bspITAttach(uint8_t pin_num, EXTITrigger_TypeDef edge_type, void (* exti_callback)(void));
void bspITDetach(uint8_t pin_num);
void bspPinMode(uint8_t pin_num, uint8_t mode);
void bspWritePin(uint8_t pin_num, uint8_t val);

#ifdef __cplusplus
}
#endif

#endif

