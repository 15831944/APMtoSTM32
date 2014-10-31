#include <AP_HAL.h>
#if CONFIG_HAL_BOARD == HAL_BOARD_STM32F4

#include <AP_HAL.h>
#include "SPIDriver.h"
#include "SPIDevice.h"

using namespace AP_HAL_STM32F4_NS;

extern const AP_HAL::HAL& hal;

void STM32F4SPIDeviceManager::init(void* machtnichts) {
		bsp_spi_dev_t *p_spi_dev = bspSpiDeviceReq(0);
	_mpu6500 = new STM32F4SPIDeviceDriver(p_spi_dev, 6, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_BaudRatePrescaler_32);
	_mpu6500->init();
}

AP_HAL::SPIDeviceDriver* STM32F4SPIDeviceManager::device(enum AP_HAL::SPIDevice d) 
{
    switch (d) {
        case AP_HAL::SPIDevice_MPU6500:
            return _mpu6500;
        default:
            return NULL;
    };
}

#endif
