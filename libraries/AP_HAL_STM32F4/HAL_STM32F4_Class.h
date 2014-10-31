#ifndef __AP_HAL_STM32F4_CLASS_H__
#define __AP_HAL_STM32F4_CLASS_H__

#include <AP_HAL.h>

#include "AP_HAL_STM32F4_Namespace.h"

class HAL_STM32F4 : public AP_HAL::HAL {
public:
    HAL_STM32F4();
    void init(int argc, char * const * argv) const;
};

extern const HAL_STM32F4 AP_HAL_STM32F4;

#endif // __AP_HAL_STM32F4_CLASS_H__

