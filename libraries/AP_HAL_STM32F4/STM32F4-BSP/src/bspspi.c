#include "stm32f4bsp.h"
#include "bspspi.h"

bsp_spi_dev_t bsp_spi_map[BSP_SPI_DEV_NUM] = {
		{
				SPI2,
				RCC_APB1Periph_SPI2,
				GPIO_AF_SPI2,

				GPIOI,
				RCC_AHB1Periph_GPIOI,
				GPIO_Pin_1,
				GPIO_PinSource1,

				GPIOI,
				RCC_AHB1Periph_GPIOI,
				GPIO_Pin_2,
				GPIO_PinSource2,

				GPIOI,
				RCC_AHB1Periph_GPIOI,
				GPIO_Pin_3,
				GPIO_PinSource3,

				0,
				0
		}
};

bsp_spi_dev_t *bspSpiDeviceReq(uint8_t spi_num)
{
		if (spi_num >= BSP_SPI_DEV_NUM)
				return NULL;
		return &bsp_spi_map[spi_num];
}

void bspSpiInit(bsp_spi_dev_t *dev, uint16_t cpol, uint16_t cpha, uint16_t br_prescaler)
{
		SPI_InitTypeDef  SPI_InitStructure;
		GPIO_InitTypeDef  GPIO_InitStructure;

		//enable peripherial clock
		if (dev->spi_dev == SPI1)
				RCC_APB2PeriphClockCmd(dev->rcc_periph, ENABLE);
		else
				RCC_APB1PeriphClockCmd(dev->rcc_periph, ENABLE);

		RCC_AHB1PeriphClockCmd(dev->clk_port_rcc | dev->mosi_port_rcc | dev->miso_port_rcc, ENABLE);

		//GPIO config
		GPIO_PinAFConfig(dev->clk_pin_group, dev->clk_pin_source, dev->dev_af);
		GPIO_PinAFConfig(dev->mosi_pin_group, dev->mosi_pin_source, dev->dev_af);
		GPIO_PinAFConfig(dev->miso_pin_group, dev->miso_pin_source, dev->dev_af);

		GPIO_InitStructure.GPIO_Pin = dev->clk_pin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
		GPIO_Init(dev->clk_pin_group, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = dev->mosi_pin;
		GPIO_Init(dev->mosi_pin_group, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = dev->miso_pin;
		GPIO_Init(dev->miso_pin_group, &GPIO_InitStructure);

		//SPI config
		SPI_Cmd(dev->spi_dev, DISABLE);
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_CPOL = cpol;
		SPI_InitStructure.SPI_CPHA = cpha;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		SPI_InitStructure.SPI_BaudRatePrescaler = br_prescaler;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init(dev->spi_dev, &SPI_InitStructure);

		SPI_Cmd(dev->spi_dev, ENABLE);
		dev->initialized = 1;
}

void bspSpiSetMode(bsp_spi_dev_t *dev, uint16_t cpol, uint16_t cpha, uint16_t br_prescaler)
{
		uint16_t tmpreg = dev->spi_dev->CR1;

		if (IS_SPI_CPOL(cpol) && IS_SPI_CPHA(cpha) && IS_SPI_BAUDRATE_PRESCALER(br_prescaler))
		{
				tmpreg &= 0xff84;
				SPI_Cmd(dev->spi_dev, DISABLE);
				tmpreg |= cpol | cpha | br_prescaler;
				dev->spi_dev->CR1 = tmpreg;
				SPI_Cmd(dev->spi_dev, ENABLE);
		}
}

uint8_t bspSpiTransmitReceive(bsp_spi_dev_t *dev, uint8_t *pTxData, uint8_t *pRxData, uint16_t size, uint32_t timeout)
{
		uint16_t i;
		uint32_t exceed_time = systickMillis() + timeout;
		if (dev != NULL)
		{
				dev->busy = 1;
				for (i=0; i<size; i++)
				{
						while (SPI_I2S_GetFlagStatus(dev->spi_dev, SPI_I2S_FLAG_TXE) == RESET && systickMillis() < exceed_time);
						if (systickMillis() >= exceed_time)
								return 0;
						if (pTxData != NULL)
								SPI_I2S_SendData(dev->spi_dev, pTxData[i]);
						else 
								SPI_I2S_SendData(dev->spi_dev, 0xff);

						while (SPI_I2S_GetFlagStatus(dev->spi_dev, SPI_I2S_FLAG_RXNE) == RESET && systickMillis() < exceed_time);
						if (systickMillis() >= exceed_time)
								return 0;
						if (pRxData != NULL)
								pRxData[i] = SPI_I2S_ReceiveData(dev->spi_dev);
						else
								SPI_I2S_ReceiveData(dev->spi_dev);
				}
				dev->busy = 0;
				return 1;
		}
		return 0;
}
