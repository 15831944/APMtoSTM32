TARGET:=IO_Toggle
TOOLCHAIN_PATH:=/host/ex_yangsheng/arm-linux/build-tools/sat/bin
TOOLCHAIN_PREFIX:=arm-none-eabi
OPTLVL:=0 # Optimization level, can be [0, 1, 2, 3, s].

PROJECT_NAME:=$(notdir $(lastword $(CURDIR)))
STMLIB:=$(CURDIR)/Libraries
STD_PERIPH:=$(STMLIB)/STM32F4xx_StdPeriph_Driver
STARTUP:=$(CURDIR)/Startup
LINKER_SCRIPT=$(CURDIR)/stm32_flash.ld
OBJDIR := $(CURDIR)/Object

INCLUDE=-I$(CURDIR)/inc
INCLUDE+=-I$(STMLIB)/CMSIS/Include
INCLUDE+=-I$(STMLIB)/CMSIS/ST/STM32F4xx/Include
INCLUDE+=-I$(STD_PERIPH)/inc
INCLUDE += -I$(CURDIR)/AP_HAL_STM32F4
#INCLUDE += -I$(STARTUP)

# vpath is used so object files are written to the current directory instead
# of the same directory as their source files
vpath %.c $(CURDIR)/src $(STD_PERIPH)/src $(STARTUP)
vpath %.cpp $(CURDIR)/src
vpath %.cpp $(CURDIR)/AP_HAL_STM32F4
vpath %.S $(STARTUP)
vpath %.s $(STARTUP)

ASRC=startup_stm32f4xx.s

# Project Source Files
CSRC=stm32f4xx_it.c \
	 system_stm32f4xx.c \
	 systick.c \
	 stm32f4bsp.c \
	 bspgpio.c \
	 usart.c \
		syscalls.c \
	 bsprcin.c \
	 bspspi.c

# Standard Peripheral Source Files
CSRC+=stm32f4xx_rcc.c \
		misc.c \
		stm32f4xx_gpio.c \
		stm32f4xx_exti.c \
		stm32f4xx_flash.c \
		stm32f4xx_syscfg.c \
		stm32f4xx_iwdg.c \
		stm32f4xx_spi.c \
		stm32f4xx_usart.c

CXXSRC = LibraryHacks.cpp
CXXSRC += main.cpp

ARCHFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard 

WARNFLAGS = -Wall -Wextra

OPTFLAGS = -O$(OPTLVL) -g \
		   -ggdb3 \
		   -ffunction-sections \
		   -fdata-sections

CDEFS=-DSTM32F4XX
CDEFS+=-DUSE_STDPERIPH_DRIVER
CDEFS += -DARM_MATH_CM4
CDEFS += -D__FPU_PRESENT=1
CDEFS += -D__FPU_USED=1

CFLAGS=$(ARCHFLAGS) \
	   $(INCLUDE) \
	   $(OPTFLAGS) \
	   $(CDEFS) \
	   $(WARNFLAGS) \
	   -Wstrict-prototypes \
	   -flto \
	   -std=gnu89 \
	   -fverbose-asm

CXXFLAGS = $(ARCHFLAGS) \
		   $(INCLUDE) \
		   $(OPTFLAGS) \
		   $(WARNFLAGS) \
		   $(CDEFS) \
		   -fpermissive \
		   -fno-rtti \
		   -fno-exceptions \
		   -fverbose-asm \
		   -std=gnu++98

ASFLAGS = $(ARCHFLAGS) \
		  $(CDEFS) \
		  $(INCLUDE) \
		  -g -ggdb3 \
		  -DUSES_CXX \
		  -x assembler-with-cpp

LDLIBS=

# -Wl,--gc-sections remove unused code
LDFLAGS=$(ARCHFLAGS) \
		-g \
        -Wl,--gc-sections,-T$(LINKER_SCRIPT)

#####
#####

OBJ := $(CSRC:%.c=%.o) $(ASRC:%.s=%.o) $(CXXSRC:%.cpp=%.o)
OBJ := $(addprefix $(OBJDIR)/,$(OBJ))
DEPS = $(OBJS:.o=.d)		

CC=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-gcc
CXX = $(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-g++
LD=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-g++
OBJCOPY=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-objcopy
AS=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-gcc
AR=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-ar
GDB=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-gdb

-include $(DEPS)

#all:
#	@echo $(CFLAGS)
#		@echo $(CXXFLAGS)
#		@echo $(ASFLAGS)
#		@echo $(LDFLAGS)

all: $(OBJ) 
	$(LD) -o $(TARGET).elf $(LDFLAGS) $(OBJ)	$(LDLIBS)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

lib: $(OBJ)
	@rm $(OBJDIR)/main.o

$(OBJDIR)/%.o : %.c
	$(warning building .o from .c)
	 $(CC) -MMD -c $(CFLAGS) -o $@ $<

$(OBJDIR)/%.o : %.cpp
	$(warning building .o from .cpp)
	$(CXX) -MMD $(CXXFLAGS) -o $@ -c $< 

$(OBJDIR)/%.o : %.s
	 $(AS) -c $(ASFLAGS) -o $@ $<

.PHONY: clean

clean:
	-rm -f $(OBJ)
	-rm -f $(OBJDIR)/*.d
	-rm -f $(TARGET).elf
	-rm -f $(TARGET).hex
	-rm -f $(TARGET).bin
