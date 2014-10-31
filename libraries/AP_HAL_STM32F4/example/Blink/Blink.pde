#include <AP_Common.h>
#include <AP_Math.h>
#include <AP_Progmem.h>
#include <AP_Param.h>

#include <AP_HAL.h>
#include <AP_HAL_STM32F4.h>

const AP_HAL::HAL& hal = AP_HAL_BOARD_DRIVER;

AP_HAL::DigitalSource *a_led;

void loop (void) {
    hal.scheduler->delay(1000);
    hal.gpio->write(0, 1);

    a_led->write(1);

    hal.scheduler->delay(1000);
    hal.gpio->write(0, 0);

    a_led->write(0);
}

void setup (void) {
    hal.gpio->pinMode(0, GPIO_OUTPUT);
    hal.gpio->write(0, 0);

    a_led = hal.gpio->channel(1);

    a_led->mode(GPIO_OUTPUT);

    a_led->write(0);
}

AP_HAL_MAIN();

