#include "bspgpio.h"
#include "stm32f4bsp.h"

static void (* usr_exti_callback[BSP_GPIO_PIN_NUM])(void) = {NULL};

const pin_info_t bsp_pin_map[BSP_GPIO_PIN_NUM]={
		/* DTFC leds */
		{GPIOE, GPIO_Pin_0, RCC_AHB1Periph_GPIOE, EXTI_PortSourceGPIOE, EXTI_PinSource0, EXTI_Line0, EXTI0_IRQn}, //LED
		{GPIOE, GPIO_Pin_1, RCC_AHB1Periph_GPIOE, EXTI_PortSourceGPIOE, EXTI_PinSource1, EXTI_Line1, EXTI1_IRQn}, //LED
		/* discovery leds */
		{GPIOD, GPIO_Pin_12, RCC_AHB1Periph_GPIOD, EXTI_PortSourceGPIOD, EXTI_PinSource12, EXTI_Line12, EXTI15_10_IRQn},
		{GPIOD, GPIO_Pin_13, RCC_AHB1Periph_GPIOD, EXTI_PortSourceGPIOD, EXTI_PinSource13, EXTI_Line13, EXTI15_10_IRQn},
		{GPIOD, GPIO_Pin_14, RCC_AHB1Periph_GPIOD, EXTI_PortSourceGPIOD, EXTI_PinSource14, EXTI_Line14, EXTI15_10_IRQn},
		{GPIOD, GPIO_Pin_15, RCC_AHB1Periph_GPIOD, EXTI_PortSourceGPIOD, EXTI_PinSource15, EXTI_Line15, EXTI15_10_IRQn},
		/* DTFC spi cs pins */
		{GPIOI, GPIO_Pin_0, RCC_AHB1Periph_GPIOI, EXTI_PortSourceGPIOI, EXTI_PinSource0, EXTI_Line0, EXTI0_IRQn}
};

void bspITAttach(uint8_t pin_num, EXTITrigger_TypeDef edge_type, void (* exti_callback)(void))
{
		//assume that the pin has already been configured as input
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		if(pin_num >= BSP_GPIO_PIN_NUM)	//should not exceed MAX pin number
				return;

		BSP_ENTER_CRITICAL();
		usr_exti_callback[pin_num] = exti_callback;
		BSP_EXIT_CRITICAL();

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		SYSCFG_EXTILineConfig(bsp_pin_map[pin_num].exti_port_source, bsp_pin_map[pin_num].exti_pin_source);

		EXTI_InitStructure.EXTI_Line = bsp_pin_map[pin_num].exti_line;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = edge_type;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		NVIC_InitStructure.NVIC_IRQChannel = bsp_pin_map[pin_num].irq_channel;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}

void bspITDetach(uint8_t pin_num)
{
		if(pin_num >= BSP_GPIO_PIN_NUM)
				return;

		BSP_ENTER_CRITICAL();
		usr_exti_callback[pin_num] = NULL;
		BSP_EXIT_CRITICAL();
}

void bspPinMode(uint8_t pin_num, uint8_t mode)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		if(pin_num >= BSP_GPIO_PIN_NUM)
				return;

		if(mode == 0)
		{
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		}
		else
		{
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		}

		RCC_AHB1PeriphClockCmd(bsp_pin_map[pin_num].gpio_rcc, ENABLE);
		GPIO_InitStructure.GPIO_Pin = bsp_pin_map[pin_num].pin;
		GPIO_Init(bsp_pin_map[pin_num].gpio_group, &GPIO_InitStructure);
}

void bspWritePin(uint8_t pin_num, uint8_t val)
{
		if (pin_num >= BSP_GPIO_PIN_NUM)
				return;
		if (val != 0)
				GPIO_SetBits(bsp_pin_map[pin_num].gpio_group, bsp_pin_map[pin_num].pin);
		else
				GPIO_ResetBits(bsp_pin_map[pin_num].gpio_group, bsp_pin_map[pin_num].pin);
}

void EXTI15_10_IRQHandler(void)
{
		if(EXTI_GetITStatus(EXTI_Line12) != RESET)
		{
				if(usr_exti_callback[0])
						usr_exti_callback[0]();
				EXTI_ClearITPendingBit(EXTI_Line12);
		}

		if(EXTI_GetITStatus(EXTI_Line13) != RESET)
		{
				if(usr_exti_callback[1])
						usr_exti_callback[1]();
				EXTI_ClearITPendingBit(EXTI_Line13);
		}

		if(EXTI_GetITStatus(EXTI_Line14) != RESET)
		{
				if(usr_exti_callback[2])
						usr_exti_callback[2]();
				EXTI_ClearITPendingBit(EXTI_Line14);
		}

		if(EXTI_GetITStatus(EXTI_Line15) != RESET)
		{
				if(usr_exti_callback[3])
						usr_exti_callback[3]();
				EXTI_ClearITPendingBit(EXTI_Line15);
		}
}

void EXTI0_IRQHandler(void)
{
		if(EXTI_GetITStatus(EXTI_Line0) != RESET)
		{
				if(usr_exti_callback[4])
						usr_exti_callback[4]();
				EXTI_ClearITPendingBit(EXTI_Line0);
		}
}
