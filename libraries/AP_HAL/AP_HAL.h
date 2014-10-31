
#ifndef __AP_HAL_H__
#define __AP_HAL_H__

#include <stdint.h>
#include <stdbool.h>

#include "AP_HAL_Namespace.h"
#include "AP_HAL_Boards.h"
#include "AP_HAL_Macros.h"

/* HAL Module Classes (all pure virtual) */
#include "Storage.h"
#include "UARTDriver.h"
#include "GPIO.h"
#include "RCInput.h"
#include "Scheduler.h"
#include "Semaphores.h"
#include "SPIDriver.h"
#include "Util.h"

#include "utility/Print.h"
#include "utility/Stream.h"
#include "utility/BetterStream.h"

/* HAL Class definition */
#include "HAL.h"

#endif // __AP_HAL_H__

