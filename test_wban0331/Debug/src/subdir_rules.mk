################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/gpio_init.obj: ../src/gpio_init.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=c6748 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="src/gpio_init.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/pinmux.obj: ../src/pinmux.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=c6748 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="src/pinmux.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/pll_init.obj: ../src/pll_init.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=c6748 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="src/pll_init.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/psc.obj: ../src/psc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=c6748 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="src/psc.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/spi0_FPGA_16bit_fushu.obj: ../src/spi0_FPGA_16bit_fushu.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=c6748 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="src/spi0_FPGA_16bit_fushu.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/spi1_init_slave.obj: ../src/spi1_init_slave.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=c6748 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="src/spi1_init_slave.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


