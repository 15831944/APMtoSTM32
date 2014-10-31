#include "bsprcin.h"
#include "stm32f4bsp.h"

uint8_t uart4_raw_cache[50];
uint8_t uart4_mid_cache[50];

volatile uint32_t rc_data_ready = 0;

void bspRcinBegin(void) 
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		USART_DeInit(UART4);
		USART_InitStructure.USART_BaudRate = 100000;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_2;
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
		USART_Init(UART4, &USART_InitStructure);

		USART_Cmd(UART4, ENABLE);

		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
}

void bspRcinEnd(void)
{
		USART_Cmd(UART4, DISABLE);
}

/*
function : figure out how many channel data is ready
param : none
retval: 
   */
uint8_t bspRcinDataReady(void)
{
		uint8_t i;
		uint8_t valid_ch_num = 0;
		uint32_t dr = rc_data_ready;

		for (i=0; i<BSP_MAX_CH_NUM; i++)
		{
				valid_ch_num += (dr & 1);
				dr = dr>>1;
		}
		return valid_ch_num;
}

/*
   @function : 
   @param : specific channel to parse
   @retval : channel data, 0xffff means channel data invalid
   */
uint16_t bspParseSbusData(rc_channel_t ch)
{
		uint16_t rc_data = 0xffff;
		if (CHx_VALID(ch))
		{
				switch(ch)
				{
						case RC_CH_1:
								rc_data = (uart4_mid_cache[2] & 0x0007)<<8 | (uart4_mid_cache[1] & 0x00ff);
								break;
						case RC_CH_2:
								rc_data = (uart4_mid_cache[3] & 0x003f)<<5 | (uart4_mid_cache[2] & 0x00f8)>>3;
								break;
						case RC_CH_3:
								rc_data = (uart4_mid_cache[5] & 0x0001)<<10 | (uart4_mid_cache[4] & 0x00ff)<<2 | (uart4_mid_cache[3] & 0x00c0)>>6;
								break;
						case RC_CH_4:
								rc_data = (uart4_mid_cache[6] & 0x000f)<<7 | (uart4_mid_cache[5] & 0x00fe)>>1;
								break;
						case RC_CH_5:
								rc_data = (uart4_mid_cache[7] & 0x007f)<<4 | (uart4_mid_cache[6] & 0x00f0)>>4;
								break;
						case RC_CH_6:
								rc_data = (uart4_mid_cache[9] & 0x0003)<<9 | (uart4_mid_cache[8] & 0x00ff)<<1 | (uart4_mid_cache[7] & 0x0080)>>7;
								break;
						case RC_CH_7:
								rc_data = (uart4_mid_cache[10] & 0x001f)<<6 | (uart4_mid_cache[9] & 0x00fc)>>2;
								break;
						case RC_CH_8:
								rc_data = (uart4_mid_cache[11] & 0x00ff)<<3 | (uart4_mid_cache[10] & 0x00e0)>>5;
								break;
						case RC_CH_9:
								rc_data = (uart4_mid_cache[13] & 0x0007)<<8 | (uart4_mid_cache[12] & 0x00ff);
								break;
						case RC_CH_10:
								rc_data = (uart4_mid_cache[14] & 0x003f)<<5 | (uart4_mid_cache[13] & 0x00f8)>>3;
								break;
						default:
								break;
				}
				SET_CHx_INVALID(ch);
		}
		return rc_data;
}

/**

 * @brief This function handles UART4 global interrupt.

 */

void UART4_IRQHandler(void)
{
		static uint8_t byteCNT = 0;
		static uint32_t lastTime = 0;
		uint32_t curTime;
		uint32_t interval = 0;

		NVIC_ClearPendingIRQ(UART4_IRQn);

		if(lastTime == 0)
		{
				curTime = systickMillis();
				lastTime = curTime;
		}
		else
		{
				curTime = systickMillis();
				interval = curTime - lastTime;
				lastTime = curTime;

				if(interval >= 3)
				{
						if(byteCNT == 25 && uart4_raw_cache[0] == 0x0f && uart4_raw_cache[24] == 0x00)
						{
								rc_data_ready = 0;
								memcpy(uart4_mid_cache, uart4_raw_cache, byteCNT);
								SET_ALL_CH_VALID();
						}
						byteCNT = 0;
				}
		}

		if(RESET != USART_GetFlagStatus(UART4, USART_FLAG_ORE))
		{
				USART_ClearFlag(UART4, USART_FLAG_ORE);
				uart4_raw_cache[byteCNT++] = USART_ReceiveData(UART4);
		}

		if(RESET != USART_GetITStatus(UART4, USART_IT_RXNE))
		{
				uart4_raw_cache[byteCNT++] = USART_ReceiveData(UART4);
		}
}
