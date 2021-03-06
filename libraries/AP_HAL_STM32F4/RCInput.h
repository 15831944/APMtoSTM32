#ifndef __AP_HAL_STM32F4_RCINPUT_H__
#define __AP_HAL_STM32F4_RCINPUT_H__

#include <AP_HAL_STM32F4.h>

class AP_HAL_STM32F4_NS::STM32F4RCInput : public AP_HAL::RCInput {
public:
    STM32F4RCInput();
    void init(void* machtnichts);
    bool new_input();
    uint8_t num_channels();
    uint16_t read(uint8_t ch);
    uint8_t read(uint16_t* periods, uint8_t len);

    bool set_overrides(int16_t *overrides, uint8_t len);
    bool set_override(uint8_t channel, int16_t override);
    void clear_overrides();
};

#endif // __AP_HAL_STM32F4_RCINPUT_H__
