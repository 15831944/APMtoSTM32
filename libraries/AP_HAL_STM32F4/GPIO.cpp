#include <AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_STM32F4

#include "GPIO.h"
#include "stm32f4bsp.h"

using namespace AP_HAL_STM32F4_NS;

STM32F4GPIO::STM32F4GPIO()
{}

void STM32F4GPIO::init()
{}

void STM32F4GPIO::pinMode(uint8_t pin, uint8_t output)
{
		bspPinMode(pin, output);
}

int8_t STM32F4GPIO::analogPinToDigitalPin(uint8_t pin)
{
		return pin;
}

uint8_t STM32F4GPIO::read(uint8_t pin) 
{
		if(pin >= BSP_GPIO_PIN_NUM)
				return 0xff;
		return GPIO_ReadInputDataBit(bsp_pin_map[pin].gpio_group, bsp_pin_map[pin].pin);
}

void STM32F4GPIO::write(uint8_t pin, uint8_t value)
{
		if(pin >= BSP_GPIO_PIN_NUM)
				return;
		if(value == 0)
				GPIO_ResetBits(bsp_pin_map[pin].gpio_group, bsp_pin_map[pin].pin);
		else
				GPIO_SetBits(bsp_pin_map[pin].gpio_group, bsp_pin_map[pin].pin);
}

void STM32F4GPIO::toggle(uint8_t pin)
{
		if(pin >= BSP_GPIO_PIN_NUM)
				return;
		GPIO_ToggleBits(bsp_pin_map[pin].gpio_group, bsp_pin_map[pin].pin);
}

/* Alternative interface: */
AP_HAL::DigitalSource* STM32F4GPIO::channel(uint16_t n) {
		if(n >= BSP_GPIO_PIN_NUM)
				return NULL;
		return new STM32F4DigitalSource(n);
}

/* Interrupt interface: */
bool STM32F4GPIO::attach_interrupt(uint8_t interrupt_num, AP_HAL::Proc p, uint8_t mode) 
{
		EXTITrigger_TypeDef edge;
		switch(mode)
		{
				case 0:
						edge = EXTI_Trigger_Falling;
						break;
				case 1:
						edge = EXTI_Trigger_Rising;
						break;
				case 2:
						edge = EXTI_Trigger_Rising_Falling;
						break;
				default:
						return false;
		}

		bspITAttach(interrupt_num, edge, p);
		return true;
}

void STM32F4DigitalSource::mode(uint8_t output)
{
		bspPinMode(_pin_num, output);
}

uint8_t STM32F4DigitalSource::read() {
		return GPIO_ReadInputDataBit(bsp_pin_map[_pin_num].gpio_group, bsp_pin_map[_pin_num].pin);
}

void STM32F4DigitalSource::write(uint8_t value) {
		if(value == 0)
				GPIO_ResetBits(bsp_pin_map[_pin_num].gpio_group, bsp_pin_map[_pin_num].pin);
		else
				GPIO_SetBits(bsp_pin_map[_pin_num].gpio_group, bsp_pin_map[_pin_num].pin);
}

void STM32F4DigitalSource::toggle() {
		GPIO_ToggleBits(bsp_pin_map[_pin_num].gpio_group, bsp_pin_map[_pin_num].pin);
}

#endif
