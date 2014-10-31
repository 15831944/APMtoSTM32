#include <AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_STM32F4

// Flymaple RCInput
// PPM input from a single pin

#include "RCInput.h"
#include "bsprcin.h"

using namespace AP_HAL_STM32F4_NS;

STM32F4RCInput::STM32F4RCInput()
{}

void STM32F4RCInput::init(void* machtnichts)
{
	bspRcinBegin();
}

bool STM32F4RCInput::new_input() 
{
		if (bspRcinDataReady() != 0)
				return true;
		return false;
}

uint8_t STM32F4RCInput::num_channels() 
{
    return 18;
}

/* constrain captured pulse to be between min and max pulsewidth. */
static inline uint16_t constrain_pulse(uint16_t p) {
    return 0;
}

uint16_t STM32F4RCInput::read(uint8_t ch) 
{
		return bspParseSbusData(ch);
}

uint8_t STM32F4RCInput::read(uint16_t* periods, uint8_t len) 
{
		uint8_t valid_cnt = 0;
		uint8_t i;
		uint16_t ch_data;
		for (i=0; i<len; i++)
		{
			ch_data = bspParseSbusData(i);
			if (ch_data != 0xffff) //valid
			{
					valid_cnt++;
					periods[i] = ch_data;
			}
		}
		return valid_cnt;
}

bool STM32F4RCInput::set_overrides(int16_t *overrides, uint8_t len) 
{
		return true;
}

bool STM32F4RCInput::set_override(uint8_t channel, int16_t override) 
{
	return true;
}

void STM32F4RCInput::clear_overrides()
{
}

#endif
