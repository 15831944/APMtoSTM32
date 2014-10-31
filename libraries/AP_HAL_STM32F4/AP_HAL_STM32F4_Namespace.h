#ifndef __AP_HAL_STM32F4_NAMESPACE_H__
#define __AP_HAL_STM32F4_NAMESPACE_H__

/* While not strictly required, names inside the STM32F4 namespace are prefixed
 * AP_HAL_STM32F4_NS for clarity. (Some of our users aren't familiar with all of the
 * C++ namespace rules.)
 */

namespace AP_HAL_STM32F4_NS {
    class STM32F4UARTDriver;
    class STM32F4I2CDriver;
    class STM32F4SPIDeviceManager;
    class STM32F4SPIDeviceDriver;
    class STM32F4AnalogSource;
    class STM32F4AnalogIn;
    class STM32F4Storage;
    class STM32F4GPIO;
    class STM32F4DigitalSource;
    class STM32F4RCInput;
    class STM32F4RCOutput;
    class STM32F4Semaphore;
    class STM32F4Scheduler;
    class STM32F4Util;
}

#endif // __AP_HAL_STM32F4_NAMESPACE_H__

