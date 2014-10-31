#ifndef __AP_HAL_STM32F4_SPI_DEVICE_H__
#define __AP_HAL_STM32F4_SPI_DEVICE_H__

#include <AP_HAL.h>
#include "AP_HAL_STM32F4_Namespace.h"
#include "stm32f4bsp.h"

class AP_HAL_STM32F4_NS::STM32F4SPIDeviceDriver : public AP_HAL::SPIDeviceDriver 
{
public:
    STM32F4SPIDeviceDriver(
			bsp_spi_dev_t *p_spi_dev,
			uint8_t cs_pin_num,
            uint16_t cpol,
			uint16_t cpha,
			uint16_t baud_rate_prescaler
    ) :
		_p_spi_dev(p_spi_dev),
        _cs_pin_num(cs_pin_num),
        CPOL(cpol),
        CPHA(cpha),
		BaudRatePrescaler(baud_rate_prescaler)
    {}

    void init();
    AP_HAL::Semaphore* get_semaphore();

    void transaction(const uint8_t *tx, uint8_t *rx, uint16_t len);

    void cs_assert();
    void cs_release();
    uint8_t transfer(uint8_t data);
    void transfer(const uint8_t *data, uint16_t len);

private:
    static AP_HAL_STM32F4_NS::STM32F4Semaphore _semaphore;
	bsp_spi_dev_t *_p_spi_dev;
	uint8_t _cs_pin_num;
    uint16_t CPOL;
	uint16_t CPHA;
	uint16_t BaudRatePrescaler;
};



#endif // __AP_HAL_STM32F4_NS_SPI_DEVICES_H__
