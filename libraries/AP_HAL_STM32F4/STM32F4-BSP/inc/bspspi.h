#ifndef _BSPSPI_H_
#define _BSPSPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"

#define BSP_SPI_DEV_NUM 1

struct bsp_spi_dev_s {
		SPI_TypeDef* spi_dev;
		uint32_t rcc_periph;
		uint8_t dev_af;

		GPIO_TypeDef* clk_pin_group;
		uint32_t clk_port_rcc;
		uint16_t clk_pin;
		uint8_t clk_pin_source;

		GPIO_TypeDef* mosi_pin_group;
		uint32_t mosi_port_rcc;
		uint16_t mosi_pin;
		uint8_t mosi_pin_source;

		GPIO_TypeDef* miso_pin_group;
		uint32_t miso_port_rcc;
		uint16_t miso_pin;
		uint8_t miso_pin_source;

		uint8_t initialized;
		volatile uint8_t busy;
};

typedef struct bsp_spi_dev_s bsp_spi_dev_t;

bsp_spi_dev_t *bspSpiDeviceReq(uint8_t spi_num);
void bspSpiInit(bsp_spi_dev_t *dev, uint16_t cpol, uint16_t cpha, uint16_t br_prescaler);  
void bspSpiSetMode(bsp_spi_dev_t *dev, uint16_t cpol, uint16_t cpha, uint16_t br_prescaler);  
uint8_t bspSpiTransmitReceive(bsp_spi_dev_t *dev, uint8_t *pTxData, uint8_t *pRxData, uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif

