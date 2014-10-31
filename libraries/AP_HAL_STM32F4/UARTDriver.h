
#ifndef __AP_HAL_STM32F4_UART_DRIVER_H__
#define __AP_HAL_STM32F4_UART_DRIVER_H__

#include <AP_HAL_Boards.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_STM32F4

#include <stdint.h>
#include <stdarg.h>

#include <AP_HAL.h>
#include "AP_HAL_STM32F4_Namespace.h"
#include "usart.h"

/**
 * STM32F4UARTDriver is an implementation of UARTDriver for the STM32F4.
 * It will be a thin wrapper on FastSerial.
 */

class AP_HAL_STM32F4_NS::STM32F4UARTDriver : public AP_HAL::UARTDriver {
public:
    STM32F4UARTDriver(uint8_t port_num);

    /* Implementations of UARTDriver virtual methods */
    void begin(uint32_t b); 
    void begin(uint32_t b, uint16_t rxS, uint16_t txS);
    void end();
    void flush();
    bool is_initialized() { return initialized; }

	void set_blocking_writes(bool blocking) {
	}

    bool tx_pending() {
        return false;
    }

    /* Implementations of Stream virtual methods */
    int16_t available();
    int16_t txspace();
    int16_t read();

    /* Implementations of Print virtual methods */
    size_t write(uint8_t c);
    size_t write(const uint8_t *buffer, size_t size);

private:
	bool	initialized;
	usart_dev_t *p_com;
	uint8_t *p_tx_buff;
	uint16_t tx_buff_size;
	uint8_t *p_rx_buff;
	uint16_t rx_buff_size;
};

#endif
#endif // __AP_HAL_STM32F4_UART_DRIVER_H__

