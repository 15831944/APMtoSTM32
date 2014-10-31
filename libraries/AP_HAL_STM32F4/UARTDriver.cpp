#include <AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_STM32F4

#include <stdlib.h>
#include <stdarg.h>

#include <AP_HAL.h>
#include <AP_Math.h>
#include "UARTDriver.h"
#include "stm32f4bsp.h"

using namespace AP_HAL_STM32F4_NS;

STM32F4UARTDriver::STM32F4UARTDriver(uint8_t port_num):p_tx_buff(NULL),tx_buff_size(128),p_rx_buff(NULL),rx_buff_size(128)
{
		p_com = bspUsartDeviceReq(port_num);
}

void STM32F4UARTDriver::begin(uint32_t baud)
{
		if (p_com != NULL)
		{
				bspDisableIRQChannel(p_com->dev_irq_num);
				bspUsartBegin(p_com, baud);
				if (p_tx_buff == NULL)
						p_tx_buff = (uint8_t *)malloc(tx_buff_size);
				if (p_rx_buff == NULL)
						p_rx_buff = (uint8_t *)malloc(rx_buff_size);

				if (p_tx_buff != NULL)
						rb_init(&(p_com->tx_buff),tx_buff_size, p_tx_buff);
				if (p_rx_buff != NULL)
						rb_init(&(p_com->rx_buff),rx_buff_size, p_rx_buff);
				bspEnableIRQChannel(p_com->dev_irq_num);
		}
}

void STM32F4UARTDriver::begin(uint32_t baud, uint16_t rxSpace, uint16_t txSpace) 
{
		uint8_t *oldRxBuf = NULL;
		uint8_t *oldTxBuf = NULL;

		if (p_com != NULL)
		{
				if (txSpace > tx_buff_size)
				{
						oldTxBuf = p_tx_buff;
						p_tx_buff = (uint8_t *)malloc(txSpace);
						tx_buff_size = txSpace;
				}

				if (rxSpace > rx_buff_size)
				{
						oldRxBuf = p_rx_buff;
						p_rx_buff = (uint8_t *)malloc(rxSpace);
						rx_buff_size = rxSpace;
				}

				bspDisableIRQChannel(p_com->dev_irq_num);
				bspUsartBegin(p_com, baud);
				if (p_tx_buff != NULL)
						rb_init(&(p_com->tx_buff), tx_buff_size, p_tx_buff);
				if (p_rx_buff != NULL)
						rb_init(&(p_com->rx_buff), rx_buff_size, p_rx_buff);
				bspEnableIRQChannel(p_com->dev_irq_num);

				if (oldTxBuf != NULL)
						free(oldTxBuf);
				if (oldRxBuf != NULL)
						free(oldRxBuf);
		}
}

void STM32F4UARTDriver::end() {
		if (p_com != NULL)
				bspUsartEnd(p_com);
}


int16_t STM32F4UARTDriver::available(void) {
		if (p_com != NULL)
				return (int16_t)bspUsartAvailable(p_com);
		return 0;
}

int16_t STM32F4UARTDriver::txspace(void) {
		if (p_com != NULL)
				return p_com->tx_buff.size - rb_full_count(&(p_com->tx_buff));
		return 0;
}

int16_t STM32F4UARTDriver::read(void) {
		if (p_com != NULL)
				return bspUsartRead(p_com);
		return -1;
}

void STM32F4UARTDriver::flush(void) 
{
		if (p_com != NULL)
				bspUsartFlush(p_com);
}

size_t STM32F4UARTDriver::write(uint8_t c) 
{
		uint8_t ch = c;
		if (p_com != NULL)
		{
				bspUsartWrite(p_com, &ch, 1);
				return 1;
		}
		return 0;
}

/*
   write size bytes to the write buffer
 */
size_t STM32F4UARTDriver::write(const uint8_t *buffer, size_t size)
{
		if (p_com != NULL)
				return bspUsartWrite(p_com, buffer, size);
		return 0;
}

#endif
