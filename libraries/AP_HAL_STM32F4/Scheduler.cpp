// Scheduler.cpp
//
// Flymaple Scheduler.
// We use systick interrupt for the 1kHz ordinary timers.
// We use a slightly higher priority HardwareTimer 2 for the failsafe callbacks
// so a hung timer wont prevent the failsafe timer interrupt running
//
// Use of noInterrupts()/interrupts() on FLymaple ARM processor.
// Please see the notes in FlymaplePortingNotes.txt in this directory for 
// information about disabling interrupts on Flymaple

#include <AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_STM32F4

#include "Scheduler.h"
#include "stm32f4bsp.h"

// Flymaple: Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects (eg SerialUSB) that need libmaple may fail.
__attribute__((constructor)) void premain() {
		Stm32f4BspInit();
}

using namespace AP_HAL_STM32F4_NS;

extern const AP_HAL::HAL& hal;

AP_HAL::Proc STM32F4Scheduler::_failsafe = NULL;
volatile bool     STM32F4Scheduler::_timer_suspended = false;
volatile bool     STM32F4Scheduler::_timer_event_missed = false;
volatile bool     STM32F4Scheduler::_in_timer_proc = false;
AP_HAL::MemberProc STM32F4Scheduler::_timer_proc[STM32F4_SCHEDULER_MAX_TIMER_PROCS
] = {NULL};
uint8_t           STM32F4Scheduler::_num_timer_procs = 0;

STM32F4Scheduler::STM32F4Scheduler() :
		_delay_cb(NULL),
		_min_delay_cb_ms(65535),
		_initialized(false)
{}

void STM32F4Scheduler::init(void* machtnichts)
{
		// 1kHz interrupts from systick for normal timers
		systickAttachCallback(_timer_procs_timer_event);

		systickConfig();
}

// This function may calls the _delay_cb to use up time
void STM32F4Scheduler::delay(uint16_t ms)
{    
		uint32_t start = systickMicros();

		while (ms > 0) {
				while ((systickMicros() - start) >= 1000) {
						ms--;
						if (ms == 0) break;
						start += 1000;
				}
				if (_min_delay_cb_ms <= ms) {
						if (_delay_cb) {
								_delay_cb();
						}
				}
		}
}

uint32_t STM32F4Scheduler::millis() {
		return systickMillis();
}

uint32_t STM32F4Scheduler::micros() {
		return systickMicros();
}

void STM32F4Scheduler::delay_microseconds(uint16_t us)
{ 
		systickDelay_us(us);
}

void STM32F4Scheduler::register_delay_callback(AP_HAL::Proc proc, uint16_t min_time_ms)
{
		_delay_cb = proc;
		_min_delay_cb_ms = min_time_ms;
}

void STM32F4Scheduler::register_timer_process(AP_HAL::MemberProc proc)
{
		int i;
		for(i = 0; i<_num_timer_procs; i++)
		{
				if(_timer_proc[i] == proc)
				{
						return;
				}
		}

		if(_num_timer_procs < STM32F4_SCHEDULER_MAX_TIMER_PROCS)
		{
				_timer_proc[_num_timer_procs] = proc;

				BSP_ENTER_CRITICAL();
				_num_timer_procs ++;
				BSP_EXIT_CRITICAL();
		}
}

void STM32F4Scheduler::register_io_process(AP_HAL::MemberProc proc)
{
}

void STM32F4Scheduler::register_timer_failsafe(AP_HAL::Proc failsafe, uint32_t period_us)
{
		/* XXX Assert period_us == 1000 */
		_failsafe = failsafe;
}

void STM32F4Scheduler::suspend_timer_procs()
{
		_timer_suspended = true;
}

void STM32F4Scheduler::resume_timer_procs()
{
		_timer_suspended = false;
		if (_timer_event_missed == true) {
				_run_timer_procs(false);
				_timer_event_missed = false;
		}
}

bool STM32F4Scheduler::in_timerprocess() {
		return _in_timer_proc;
}

void STM32F4Scheduler::_timer_procs_timer_event() {
		_run_timer_procs(true);
}

void STM32F4Scheduler::_run_timer_procs(bool called_from_isr) 
{
		if (_in_timer_proc) 
		{
				// the timer calls took longer than the period of the
				// timer. This is bad, and may indicate a serious
				// driver failure. We can't just call the drivers
				// again, as we could run out of stack. So we only
				// call the _failsafe call. It's job is to detect if
				// the drivers or the main loop are indeed dead and to
				// activate whatever failsafe it thinks may help if
				// need be.  We assume the failsafe code can't
				// block. If it does then we will recurse and die when
				// we run out of stack
				if (_failsafe != NULL) 
				{
						_failsafe();
				}
				return;
		}

		_in_timer_proc = true;

		if (!_timer_suspended) 
		{
				// now call the timer based drivers
				for (int i = 0; i < _num_timer_procs; i++) 
				{
						if (_timer_proc[i] != NULL) 
						{
								_timer_proc[i]();
						}
				}
		} 
		else if (called_from_isr) 
		{
				_timer_event_missed = true;
		}

		// and the failsafe, if one is setup
		if (_failsafe != NULL) 
		{
				_failsafe();
		}

		_in_timer_proc = false;
}

bool STM32F4Scheduler::system_initializing() {
		return !_initialized;
}

void STM32F4Scheduler::system_initialized()
{
		//    if (_initialized) {
		//        panic(PSTR("PANIC: scheduler::system_initialized called"
		//                   "more than once"));
		//    }
		_initialized = true;
}

void STM32F4Scheduler::panic(const char *errormsg) {
		/* Suspend timer processes. We still want the timer event to go off
		 * to run the _failsafe code, however. */
		// REVISIT: not tested on STM32F4
		_timer_suspended = true;
		//    hal.console->println_P(errormsg);
		for(;;);
}

void STM32F4Scheduler::reboot(bool hold_in_bootloader) {
		//   hal.uartA->println_P(PSTR("GOING DOWN FOR A REBOOT\r\n"));
		hal.scheduler->delay(100);
		Stm32f4BspReboot();
}

#endif
