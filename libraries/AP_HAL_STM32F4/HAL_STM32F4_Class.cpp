#include <AP_HAL.h>
#if CONFIG_HAL_BOARD == HAL_BOARD_STM32F4

#include "HAL_STM32F4_Class.h"
#include "AP_HAL_STM32F4_Private.h"

using namespace AP_HAL_STM32F4_NS;

static STM32F4UARTDriver uartDriverConsole(0);
static STM32F4UARTDriver uartDriverGPS(1);
static STM32F4SPIDeviceManager spiDeviceManager;
static STM32F4Storage storageDriver;
static STM32F4GPIO gpioDriver;
static STM32F4RCInput rcinDriver;
static STM32F4Scheduler schedulerInstance;
static STM32F4Util utilInstance;

HAL_STM32F4::HAL_STM32F4() :
    AP_HAL::HAL(
		&uartDriverGPS,
		&spiDeviceManager,
		&storageDriver,
		&uartDriverConsole,
		&gpioDriver,
		&rcinDriver,
		&schedulerInstance,
		&utilInstance
	)
{}

void HAL_STM32F4::init(int argc,char* const argv[]) const {
    /* initialize all drivers and private members here.
     * up to the programmer to do this in the correct order.
     * Scheduler should likely come first. */
    scheduler->init(NULL);
	console->begin(115200);
	spi->init(NULL);
	storage->init(NULL);
	rcin->init(NULL);
}

const HAL_STM32F4 AP_HAL_STM32F4;

#endif
