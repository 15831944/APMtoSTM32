#include "usart.h"
#include "stm32f4bsp.h"
#include <stdio.h>

usart_dev_t bsp_usart_map[BSP_USART_NUM] = {
		{
				USART6,
				GPIO_AF_USART6,
				RCC_APB2Periph_USART6,
				USART6_IRQn,
				GPIOG,
				RCC_AHB1Periph_GPIOG,
				GPIO_Pin_14,
				GPIO_PinSource14,
				GPIOG,
				RCC_AHB1Periph_GPIOG,
				GPIO_Pin_9,
				GPIO_PinSource9,
				{NULL, 0, 0, 0, 0},
				{NULL, 0, 0, 0, 0},
				0
		},
		{
				USART2,
				GPIO_AF_USART2,
				RCC_APB1Periph_USART2,
				USART2_IRQn,
				GPIOD,
				RCC_AHB1Periph_GPIOD,
				GPIO_Pin_5,
				GPIO_PinSource5,
				GPIOD,
				RCC_AHB1Periph_GPIOD,
				GPIO_Pin_6,
				GPIO_PinSource6,
				{NULL, 0, 0, 0, 0},
				{NULL, 0, 0, 0, 0},
				0
		},
		{
				USART3,
				GPIO_AF_USART3,
				RCC_APB1Periph_USART3,
				USART3_IRQn,
				GPIOC,
				RCC_AHB1Periph_GPIOC,
				GPIO_Pin_10,
				GPIO_PinSource10,
				GPIOC,
				RCC_AHB1Periph_GPIOC,
				GPIO_Pin_11,
				GPIO_PinSource11,
				{NULL, 0, 0, 0, 0},
				{NULL, 0, 0, 0, 0},
				0
		}
};

uint8_t bspUsartInUse(uint8_t usart_num)
{
		if (usart_num >= BSP_USART_NUM)
				return 0xff;
		return bsp_usart_map[usart_num].in_use;
}

usart_dev_t *bspUsartDeviceReq(uint8_t usart_num)
{
		if (!bspUsartInUse(usart_num))
		{
				bsp_usart_map[usart_num].in_use = 1;
				return &bsp_usart_map[usart_num];
		}
		else
				return NULL;
}

void bspUsartBegin(usart_dev_t *dev, uint32_t baudrate)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

//		printf("initializing...\n");
		//enable peripherial clock
		if(dev->USARTx == USART1 || dev->USARTx == USART6)
				RCC_APB2PeriphClockCmd(dev->dev_rcc, ENABLE);
		else
				RCC_APB1PeriphClockCmd(dev->dev_rcc, ENABLE);

		RCC_AHB1PeriphClockCmd(dev->tx_port_rcc | dev->rx_port_rcc, ENABLE);

		//map pins to dev
		GPIO_PinAFConfig(dev->tx_pin_group, dev->tx_pin_source, dev->dev_af);
		GPIO_PinAFConfig(dev->rx_pin_group, dev->rx_pin_source, dev->dev_af);

		//configure pins
		GPIO_InitStructure.GPIO_Pin = dev->tx_pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(dev->tx_pin_group, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = dev->rx_pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init(dev->rx_pin_group, &GPIO_InitStructure);

		//configure usart
		USART_DeInit(dev->USARTx);
		USART_InitStructure.USART_BaudRate = baudrate;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
		USART_Init(dev->USARTx, &USART_InitStructure);

		USART_Cmd(dev->USARTx, ENABLE);

		//miscellaneous irq preemptionPrio 3
		//serial com 2
		//sensor 1
		//reverse 0
		NVIC_InitStructure.NVIC_IRQChannel = dev->dev_irq_num;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_ITConfig(dev->USARTx, USART_IT_RXNE, ENABLE);
}

void bspUsartEnd(usart_dev_t *dev)
{
		while ((dev->USARTx->CR1 & USART_CR1_UE) && !(dev->USARTx->SR & USART_FLAG_TC));
		USART_Cmd(dev->USARTx, DISABLE);

		rb_reset(&(dev->rx_buff));
		rb_reset(&(dev->tx_buff));
}

void bspUsartFlush(usart_dev_t *dev)
{
		rb_reset(&(dev->rx_buff));
}

uint32_t bspUsartAvailable(usart_dev_t *dev)
{
		return rb_full_count(&(dev->rx_buff));
}

uint8_t bspUsartRead(usart_dev_t *dev)
{
		while(!bspUsartAvailable(dev));//blocking
		return rb_remove(&(dev->rx_buff));
}

uint32_t bspUsartWrite(usart_dev_t *dev, const uint8_t *pData, uint32_t len)
{
		uint32_t txed = 0;
		bspDisableIRQChannel(dev->dev_irq_num);
		while ((txed < len) && rb_insert(&(dev->tx_buff), pData[txed]))
		{
				txed ++;
		}
		bspEnableIRQChannel(dev->dev_irq_num);
		USART_ITConfig(dev->USARTx, USART_IT_TXE, ENABLE);
		return txed;
}

void USART2_IRQHandler(void)
{
		char byteRec;
		NVIC_ClearPendingIRQ(USART2_IRQn);
		if(RESET != USART_GetFlagStatus(USART2, USART_FLAG_ORE))
		{
				USART_ClearFlag(USART2, USART_FLAG_ORE);
				USART_ReceiveData(USART2);
		}
		if(RESET != USART_GetITStatus(USART2, USART_IT_RXNE))
		{
				byteRec = USART_ReceiveData(USART2);
				rb_insert(&(bsp_usart_map[1].rx_buff), byteRec);
				USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		}
		if(RESET != USART_GetITStatus(USART2, USART_IT_TXE))
		{
				if(!rb_is_empty(&(bsp_usart_map[1].tx_buff)))
				{
						USART_SendData(USART2, rb_remove(&(bsp_usart_map[1].tx_buff)));
				}
				else
				{
						USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
				}
				USART_ClearITPendingBit(USART2, USART_IT_TXE);
		}
}

void USART6_IRQHandler(void)
{
		char byteRec;
		NVIC_ClearPendingIRQ(USART6_IRQn);
		if(RESET != USART_GetFlagStatus(USART6, USART_FLAG_ORE))
		{
				USART_ClearFlag(USART6, USART_FLAG_ORE);
				USART_ReceiveData(USART6);
		}
		if(RESET != USART_GetITStatus(USART6, USART_IT_RXNE))
		{
				byteRec = USART_ReceiveData(USART6);
				rb_insert(&(bsp_usart_map[0].rx_buff), byteRec);
				USART_ClearITPendingBit(USART6, USART_IT_RXNE);
		}
		if(RESET != USART_GetITStatus(USART6, USART_IT_TXE))
		{
				if(!rb_is_empty(&(bsp_usart_map[0].tx_buff)))
				{
						USART_SendData(USART6, rb_remove(&(bsp_usart_map[0].tx_buff)));
				}
				else
				{
						USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
				}
				USART_ClearITPendingBit(USART6, USART_IT_TXE);
		}
}

void USART3_IRQHandler(void)
{
		char byteRec;
		NVIC_ClearPendingIRQ(USART3_IRQn);
		if(RESET != USART_GetFlagStatus(USART3, USART_FLAG_ORE))
		{
				USART_ClearFlag(USART3, USART_FLAG_ORE);
				USART_ReceiveData(USART3);
		}
		if(RESET != USART_GetITStatus(USART3, USART_IT_RXNE))
		{
				byteRec = USART_ReceiveData(USART3);
				rb_insert(&(bsp_usart_map[2].rx_buff), byteRec);
				USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		}
		if(RESET != USART_GetITStatus(USART3, USART_IT_TXE))
		{
				if(!rb_is_empty(&(bsp_usart_map[2].tx_buff)))
				{
						USART_SendData(USART3, rb_remove(&(bsp_usart_map[2].tx_buff)));
				}
				else
				{
						USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
				}
				USART_ClearITPendingBit(USART3, USART_IT_TXE);
		}
}
