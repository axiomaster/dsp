################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
gpio_init.obj: ../gpio_init.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="gpio_init.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

pinmux.obj: ../pinmux.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="pinmux.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

pll_init.obj: ../pll_init.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="pll_init.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

psc.obj: ../psc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="psc.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

vec.obj: ../vec.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="vec.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


