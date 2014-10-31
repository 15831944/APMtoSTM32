#include <AP_HAL.h>
#if CONFIG_HAL_BOARD == HAL_BOARD_STM32F4

#include <AP_HAL.h>
#include "SPIDevice.h"
#include "Semaphores.h"

using namespace AP_HAL_STM32F4_NS;

extern const AP_HAL::HAL& hal;

STM32F4Semaphore STM32F4SPIDeviceDriver::_semaphore;

void STM32F4SPIDeviceDriver::init() {
		bspPinMode(_cs_pin_num, 1);
		bspWritePin(_cs_pin_num, 1);

		if (_p_spi_dev != NULL)
		{
				if (_p_spi_dev->initialized == 0)
						bspSpiInit(_p_spi_dev, CPOL, CPHA, BaudRatePrescaler);
		}
		else
		{
				/* complain */
				hal.scheduler->panic(PSTR("PANIC: spi_device empty!"));
		}
}

AP_HAL::Semaphore* STM32F4SPIDeviceDriver::get_semaphore() {
		return &_semaphore;
}

void STM32F4SPIDeviceDriver::cs_assert() 
{
		bspSpiSetMode(_p_spi_dev, CPOL, CPHA, BaudRatePrescaler);
		bspWritePin(_cs_pin_num, 0);
}

void STM32F4SPIDeviceDriver::cs_release() 
{
		bspWritePin(_cs_pin_num, 1);
}

uint8_t STM32F4SPIDeviceDriver::transfer(uint8_t data) 
{
		uint8_t rx_data = 0;
		uint8_t tx_data = 0;
		if (_p_spi_dev->busy) {
				hal.scheduler->panic(PSTR("PANIC: SPI transfer collision"));
		}
		if (0 == bspSpiTransmitReceive(_p_spi_dev, &tx_data, &rx_data, 1,5))
		{
				hal.scheduler->panic(PSTR("PANIC: SPI write collision"));
				return 0;
		}
		return rx_data;
}

void STM32F4SPIDeviceDriver::transfer(const uint8_t *data, uint16_t len)
{
		if (_p_spi_dev->busy) {
				hal.scheduler->panic(PSTR("PANIC: SPI write collision"));
		}
		if (0 == bspSpiTransmitReceive(_p_spi_dev, data, NULL, len, 10))
		{
				hal.scheduler->panic(PSTR("PANIC: SPI transfer error"));
		}
}

void STM32F4SPIDeviceDriver::transaction(const uint8_t *tx, uint8_t *rx, uint16_t len) 
{
		cs_assert();
		bspSpiTransmitReceive(_p_spi_dev, tx, rx, len, 10);
		cs_release();
}

#endif
