/* Includes ------------------------------------------------------------------*/
//#include "HAL_STM32F4_Class.h"
#include "stm32f4xx.h"
#include "stm32f4bsp.h"
//#include "EEPROM.h"
#include <stdio.h>
#include <stdlib.h>

uint32_t delay_cnt = 200;
//STM32F4GPIO& hal = g_STM32F4GPIO;
//const HAL_STM32F4& hal = AP_HAL_STM32F4;
uint8_t rx_buf[64];
uint8_t tx_buf[64];
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
void ChangeDelay(void);
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
		usart_dev_t *p_u_dev = bspUsartDeviceReq(0);
		bsp_spi_dev_t *p_s_dev = bspSpiDeviceReq(0);
		uint8_t data[2] = {0x62, '2'};
		uint8_t mpu_pid = 0x62;
		uint8_t mpu_tx_data = 0x75 | 0x80;
		/*!< At this stage the microcontroller clock setting is already configured, 
		  this is done through SystemInit() function which is called from startup
		  file (startup_stm32f4xx.s) before to branch to application main.
		  To reconfigure the default setting of SystemInit() function, refer to
		  system_stm32f4xx.c file
		 */
//		hal.init();
		Stm32f4BspInit();
		systickConfig();
		bspRcinBegin();	
//		eeprom.init();
//		loc_io.pinMode(0,1);
//		g_STM32F4GPIO.pinMode(0,1);
		//hal.pinMode(0,1);
		//hal.init();
//		hal.p_gpio_driver->pinMode(0,1);
//		hal.p_gpio_driver->pinMode(1,1);
//		hal.p_gpio_driver->pinMode(2,1);
//		hal.p_gpio_driver->pinMode(3,1);
		bspPinMode(0,1);
		bspPinMode(1,1);
		bspPinMode(6,1);

		bspWritePin(6,1);

		BSP_ENTER_CRITICAL();
		systickDelay_ms(1);
		BSP_EXIT_CRITICAL();

		if (p_s_dev != NULL)
				bspSpiInit(p_s_dev, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_BaudRatePrescaler_32);

		if (p_u_dev != NULL)
		{
				rb_init(&(p_u_dev->rx_buff), 64, rx_buf); 
				rb_init(&(p_u_dev->tx_buff), 64, tx_buf); 
				bspUsartBegin(p_u_dev, 115200);
		}
	

//		if(p_s_dev != NULL)
//		{
//				bspWritePin(6, 0);
//				bspSpiTransmitReceive(p_s_dev, &mpu_tx_data, NULL, 1, 5);
//				bspSpiTransmitReceive(p_s_dev, NULL, &mpu_pid, 1, 5);
//		}

//		if(p_u_dev != NULL)
//		{
//				bspUsartWrite(p_u_dev, &mpu_pid, 1); 
//		}
		
		while (1)
		{
				if(p_s_dev != NULL)
				{
						bspWritePin(6,0);
						bspSpiTransmitReceive(p_s_dev, &mpu_tx_data, NULL, 1, 5);
						bspSpiTransmitReceive(p_s_dev, NULL, &mpu_pid, 1, 5);
						bspWritePin(6,1);
				}
				if(p_u_dev != NULL)
				{
						while (bspUsartAvailable(p_u_dev) > 0)
						{
								data[0] = bspUsartRead(p_u_dev);
								bspUsartWrite(p_u_dev, data, 1); 
						}
						bspUsartWrite(p_u_dev, &mpu_pid, 1);
				}

				if(0 != bspRcinDataReady())
						bspWritePin(1,0);
				bspWritePin(0,0);
				/* Insert delay */
				systickDelay_ms(50);

				bspWritePin(0,1);
				bspWritePin(1,1);
				/* Insert delay */
				systickDelay_ms(950);
				//ChangeDelay();
		}
}

/**
 * @brief  Delay Function.
 * @param  nCount:specifies the Delay time length.
 * @retval None
 */
void Delay(__IO uint32_t nCount)
{
		while(nCount--)
		{
		}
}

void ChangeDelay(void)
{
		delay_cnt += 200;
		if(delay_cnt >= 3000)
				delay_cnt = 200;
}


#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
		/* User can add his own implementation to report the file name and line number,
ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

		/* Infinite loop */
		while (1)
		{
		}
}
#endif

/**
 * @}
 */ 

/**
 * @}
 */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
