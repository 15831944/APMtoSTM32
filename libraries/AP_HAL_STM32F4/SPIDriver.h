
#ifndef __AP_HAL_EMPTY_SPIDRIVER_H__
#define __AP_HAL_EMPTY_SPIDRIVER_H__

#include <AP_HAL_STM32F4.h>
#include "SPIDevice.h"
#include "Semaphores.h"

class AP_HAL_STM32F4_NS::STM32F4SPIDeviceManager : public AP_HAL::SPIDeviceManager {
public:
    STM32F4SPIDeviceManager()
	{}
    void init(void *);
    AP_HAL::SPIDeviceDriver* device(enum AP_HAL::SPIDevice);
private:
    STM32F4SPIDeviceDriver* _mpu6500;
};

#endif // __AP_HAL_EMPTY_SPIDRIVER_H__
