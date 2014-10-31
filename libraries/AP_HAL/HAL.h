
#ifndef __AP_HAL_HAL_H__
#define __AP_HAL_HAL_H__

#include "AP_HAL_Namespace.h"

#include "../AP_HAL/GPIO.h"
#include "../AP_HAL/Scheduler.h"
#include "../AP_HAL/RCInput.h"
#include "../AP_HAL/Storage.h"
#include "../AP_HAL/UARTDriver.h"
#include "../AP_HAL/SPIDriver.h"

class AP_HAL::HAL {
public:
    HAL(AP_HAL::UARTDriver* _uartB,
		AP_HAL::SPIDeviceManager*	_spi,
		AP_HAL::Storage*	_storage,
		AP_HAL::UARTDriver* _console,
		AP_HAL::GPIO*       _gpio,
		AP_HAL::RCInput*	_rcin,
        AP_HAL::Scheduler*  _scheduler,
		AP_HAL::Util*		_util)
        :
		uartB(_uartB),
		spi(_spi),
		storage(_storage),
		console(_console),
        gpio(_gpio),
		rcin(_rcin),
        scheduler(_scheduler),
		util(_util)
    {}

    virtual void init(int argc, char * const argv[]) const = 0;

	AP_HAL::UARTDriver* uartB;
	AP_HAL::SPIDeviceManager* 	spi;
	AP_HAL::Storage*	storage;
	AP_HAL::UARTDriver* console;
    AP_HAL::GPIO*       gpio;
	AP_HAL::RCInput*	rcin;
    AP_HAL::Scheduler*  scheduler;
	AP_HAL::Util* 		util;
};

#endif // __AP_HAL_HAL_H__

