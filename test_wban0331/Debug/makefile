################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/ti/ccsv5/tools/compiler/c6000_7.4.4

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./src/spi1_init_slave.obj" \
"./src/spi0_FPGA_16bit_fushu.obj" \
"./src/psc.obj" \
"./src/pll_init.obj" \
"./src/pinmux.obj" \
"./src/gpio_init.obj" \
"./vec.obj" \
"./spread.obj" \
"./scrambler.obj" \
"./main.obj" \
"./interleave.obj" \
"./dpsk.obj" \
"./complex.obj" \
"./WbanTasks.obj" \
"./WbanSecurity.obj" \
"./WbanPHY.obj" \
"./WbanMAC.obj" \
"./PHYtx.obj" \
"./PHYrx.obj" \
"./PHYHeadGen.obj" \
"./PHYHeadCRC.obj" \
"./CpuTimers.obj" \
"./BCH.obj" \
"./AES_Encrypt.obj" \
"./AES_Decrypt.obj" \
"./AES_CBC.obj" \
"../C6748.cmd" \
"../rts6740.lib" \
-l"libc.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include src/subdir_vars.mk
-include subdir_rules.mk
-include src/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: test_wban0331.out

# Tool invocations
test_wban0331.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C6000 Linker'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --define=c6748 --display_error_number --diag_warning=225 --diag_wrap=off -z --stack_size=0x800 -m"test_wban0331.map" --heap_size=0x800 -i"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/lib" -i"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="test_wban0331_linkInfo.xml" --rom_model -o "test_wban0331.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C6000_EXECUTABLE_OUTPUTS__QUOTED) "test_wban0331.out"
	-$(RM) "AES_CBC.pp" "AES_Decrypt.pp" "AES_Encrypt.pp" "BCH.pp" "CpuTimers.pp" "PHYHeadCRC.pp" "PHYHeadGen.pp" "PHYrx.pp" "PHYtx.pp" "WbanMAC.pp" "WbanPHY.pp" "WbanSecurity.pp" "WbanTasks.pp" "complex.pp" "dpsk.pp" "interleave.pp" "main.pp" "scrambler.pp" "spread.pp" "src\gpio_init.pp" "src\pinmux.pp" "src\pll_init.pp" "src\psc.pp" "src\spi0_FPGA_16bit_fushu.pp" "src\spi1_init_slave.pp" 
	-$(RM) "AES_CBC.obj" "AES_Decrypt.obj" "AES_Encrypt.obj" "BCH.obj" "CpuTimers.obj" "PHYHeadCRC.obj" "PHYHeadGen.obj" "PHYrx.obj" "PHYtx.obj" "WbanMAC.obj" "WbanPHY.obj" "WbanSecurity.obj" "WbanTasks.obj" "complex.obj" "dpsk.obj" "interleave.obj" "main.obj" "scrambler.obj" "spread.obj" "vec.obj" "src\gpio_init.obj" "src\pinmux.obj" "src\pll_init.obj" "src\psc.obj" "src\spi0_FPGA_16bit_fushu.obj" "src\spi1_init_slave.obj" 
	-$(RM) "vec.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

