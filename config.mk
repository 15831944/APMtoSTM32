#config.mk START
# Select maple_RET6 for Flymaple
BOARD = dtfc_IGT6
# HAL_BOARD determines default HAL target.
HAL_BOARD ?= HAL_BOARD_STM32F4
# The communication port used to communicate with the Flymaple
PORT = /dev/ttyUSB0
# You must provide the path to the libmaple library directory:
STM32F4BSP_PATH = /host/ex_yangsheng/sandbox/ardu_port/libraries/AP_HAL_STM32F4/STM32F4-BSP
