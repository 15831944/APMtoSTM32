# board_flymaple.mk
#
# Build ArduPlane for Flymaple http://www.open-drone.org/flymaple

ifeq ($(wildcard $(STM32F4BSP_PATH)/inc),)
$(error ERROR: failed to find libmaple - please see libraries/AP_HAL_FLYMAPLE/FlymaplePortingNotes.txt)
endif

TOOLCHAIN = ARM

#include $(MK_DIR)/find_arduino.mk
include $(MK_DIR)/find_tools.mk

# include dir
#
BSPINCLUDES = -I$(STM32F4BSP_PATH)/inc \
			  -I$(STM32F4BSP_PATH)/Libraries/CMSIS/Include \
			  -I$(STM32F4BSP_PATH)/Libraries/CMSIS/ST/STM32F4xx/Include \
			  -I$(STM32F4BSP_PATH)/Libraries/STM32F4xx_StdPeriph_Driver/inc 

# flags about dependancy
#
DEPFLAGS        =   -MD -MT $@

#
# Tool options
#

ARCHFLAGS        = -mcpu=cortex-m4 \
					  -mthumb \
					  -mfpu=fpv4-sp-d16 \
					  -mfloat-abi=hard 

OPTFLAGS = -O0 \
		   -g \
		   -ggdb3 \
		   -ffunction-sections \
		   -fdata-sections

DEFINES         =  	$(EXTRAFLAGS) \
					-DCONFIG_HAL_BOARD=$(HAL_BOARD) \
					-DSTM32F4xx \
					-DUSE_STDPERIPH_DRIVER \
					-DARM_MATH_CM4 \
					-D__FPU_PRESENT=1 \
					-D__FPU_USED=1

WARNFLAGS       =  -Wall -Wextra

# flags we pass to the C compiler
#
CFLAGS = $(ARCHFLAGS) \
		$(WARNFLAGS) \
		$(BSPINCLUDES) \
		$(DEFINES) \
		$(OPTFLAGS) \
		$(DEPFLAGS) \
		-flto \
		-Wstrict-prototypes \
		-std=gnu89 \
		-fverbose-asm

# flags we pass to the C++ compiler		
#
CXXFLAGS = $(ARCHFLAGS) \
		   $(BSPINCLUDES) \
		   $(OPTFLAGS) \
		   $(DEFINES) \
		   $(WARNFLAGS) \
			   -fno-rtti \
			   -fno-exceptions \
			   -fverbose-asm \
			   -fpermissive \
			   -std=gnu++98


# flags we pass to the assembler
#
ASFLAGS = $(ARCHFLAGS) \
		  $(DEFINES) \
		  $(BSPINCLUDES) \
		  -g -ggdb3 \
		  -DUSES_CXX \
		  -x assembler-with-cpp

# flags pass to the linker
#
LDFLAGS = -Wl,--gc-sections,-T$(STM32F4BSP_PATH)/stm32_flash.ld \
		  $(ARCHFLAGS)

# under certain situations with certain avr-gcc versions the --relax flag causes
# a bug. Give the user a way to disable this flag per-sketch.
# I know this is a rotten hack but we're really close to sunset on AVR.
EXCLUDE_RELAX := $(wildcard $(SRCROOT)/norelax.inoflag)
ifeq ($(EXCLUDE_RELAX),)
#  LDFLAGS      +=   -Wl,--relax
endif

LIBS = -lm

ifeq ($(VERBOSE),)
v = @
else
v =
endif

COREOBJS = $(STM32F4BSP_PATH)/Object/*.o 

# Library object files
LIBOBJS			:=	$(SKETCHLIBOBJS) $(COREOBJS)

################################################################################
# Built products
#

# The ELF file
SKETCHELF		=	$(BUILDROOT)/$(SKETCH).elf

# HEX file
SKETCHHEX		=	$(BUILDROOT)/$(SKETCH).hex

# BIN file
SKETCHBIN		=	$(BUILDROOT)/$(SKETCH).bin

# EEP file
SKETCHEEP		=	$(BUILDROOT)/$(SKETCH).eep

# Map file
SKETCHMAP		=	$(BUILDROOT)/$(SKETCH).map

# All of the objects that may be built
ALLOBJS			=	$(SKETCHOBJS) $(LIBOBJS)

# All of the dependency files that may be generated
ALLDEPS			=	$(ALLOBJS:%.o=%.d)

################################################################################
# Targets
#

all: $(SKETCHELF) $(SKETCHEEP) $(SKETCHHEX) $(SKETCHBIN)

################################################################################
# Rules
#

# fetch dependency info from a previous build if any of it exists
-include $(ALLDEPS)

# Link the final object
$(SKETCHELF):	$(SKETCHOBJS) $(LIBOBJS)
	$(RULEHDR)
	$(v)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

# Create the hex file
$(SKETCHHEX):	$(SKETCHELF)
	$(RULEHDR)
	$(v)$(OBJCOPY) -O ihex -R .eeprom $< $@

# Create the bin file
$(SKETCHBIN):	$(SKETCHELF)
	$(RULEHDR)
	$(v)$(OBJCOPY) -v -O binary $< $@

# Create the eep file
$(SKETCHEEP):	$(SKETCHELF)
	$(RULEHDR)
	$(v)$(OBJCOPY) -O ihex -j.eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $< $@

#
# Build sketch objects
#
SKETCH_INCLUDES	=	$(SKETCHLIBINCLUDES) $(ARDUINOLIBINCLUDES) $(BSPINCLUDES)

$(BUILDROOT)/%.o: $(BUILDROOT)/%.cpp
	$(RULEHDR)
	$(v)$(CXX) $(CXXFLAGS) -c -o $@ $< -I$(SRCROOT) $(SKETCH_INCLUDES)

$(BUILDROOT)/%.o: $(SRCROOT)/%.cpp
	$(RULEHDR)
	$(v)$(CXX) $(CXXFLAGS) -c -o $@ $< $(SKETCH_INCLUDES)

$(BUILDROOT)/%.o: $(SRCROOT)/%.c
	$(RULEHDR)
	$(v)$(CC) $(CFLAGS) -c -o $@ $< $(SKETCH_INCLUDES)

$(BUILDROOT)/%.o: $(SRCROOT)/%.S
	$(RULEHDR)
	$(v)$(AS) $(ASFLAGS) -c -o $@ $< $(SKETCH_INCLUDES)

#
# Build library objects from sources in the sketchbook
#
SLIB_INCLUDES	=	-I$(dir $<)/utility $(SKETCHLIBINCLUDES) $(ARDUINOLIBINCLUDES) $(COREINCLUDES)

$(BUILDROOT)/libraries/%.o: $(SKETCHBOOK)/libraries/%.cpp
	$(RULEHDR)
	$(v)$(CXX) $(CXXFLAGS) -c -o $@ $< $(SLIB_INCLUDES)
	$(warning .o from .cpp)

$(BUILDROOT)/libraries/%.o: $(SKETCHBOOK)/libraries/%.c
	$(RULEHDR)
	$(v)$(CC) $(CFLAGS) -c -o $@ $< $(SLIB_INCLUDES)
	$(warning .o from .c)

$(BUILDROOT)/libraries/%.o: $(SKETCHBOOK)/libraries/%.S
	$(RULEHDR)
	$(v)$(AS) $(ASFLAGS) -c -o $@ $< $(SLIB_INCLUDES)
	$(warning .o from .s)


