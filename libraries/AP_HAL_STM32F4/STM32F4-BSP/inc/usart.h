#ifndef _USART_H_
#define _USART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "ring_buffer.h"

#define BSP_USART_NUM 3
#ifndef NULL
#define NULL 0
#endif

struct usart_dev_s {
	USART_TypeDef *USARTx;
	uint8_t dev_af;
	uint32_t dev_rcc;

	IRQn_Type dev_irq_num;

	GPIO_TypeDef *tx_pin_group;
	uint32_t tx_port_rcc;
	uint16_t tx_pin;
	uint8_t tx_pin_source;

	GPIO_TypeDef *rx_pin_group;
	uint32_t rx_port_rcc;
	uint16_t rx_pin;
	uint8_t rx_pin_source;

	ring_buffer rx_buff;
	ring_buffer tx_buff;

	volatile uint8_t in_use;
};

typedef struct usart_dev_s usart_dev_t;

uint8_t bspUsartInUse(uint8_t usart_num);
usart_dev_t *bspUsartDeviceReq(uint8_t usart_num);
void bspUsartBegin(usart_dev_t *dev, uint32_t baudrate);
void bspUsartEnd(usart_dev_t *dev);
void bspUsartFlush(usart_dev_t *dev);
uint32_t bspUsartAvailable(usart_dev_t *dev);
uint8_t bspUsartRead(usart_dev_t *dev);
uint32_t bspUsartWrite(usart_dev_t *dev, const uint8_t *pData, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif

