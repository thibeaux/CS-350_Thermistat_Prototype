################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1657342753: ../Thermostat_project.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs1110/ccs/utils/sysconfig_1.11.0/sysconfig_cli.bat" -s "C:/ti/simplelink_cc32xx_sdk_5_30_00_08/.metadata/product.json" --script "C:/Users/Metro Aviation/workspace_v11/Thermostat_Project/Thermostat_project.syscfg" -o "syscfg" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_drivers_config.c: build-1657342753 ../Thermostat_project.syscfg
syscfg/ti_drivers_config.h: build-1657342753
syscfg/ti_utils_build_linker.cmd.genlibs: build-1657342753
syscfg/syscfg_c.rov.xs: build-1657342753
syscfg/ti_utils_runtime_model.gv: build-1657342753
syscfg/ti_utils_runtime_Makefile: build-1657342753
syscfg/: build-1657342753

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1110/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -O3 --include_path="C:/Users/Metro Aviation/workspace_v11/Thermostat_Project" --include_path="C:/Users/Metro Aviation/workspace_v11/Thermostat_Project/MCU+Image" --include_path="C:/ti/simplelink_cc32xx_sdk_5_30_00_08/source" --include_path="C:/ti/simplelink_cc32xx_sdk_5_30_00_08/kernel/nortos" --include_path="C:/ti/simplelink_cc32xx_sdk_5_30_00_08/kernel/nortos/posix" --include_path="C:/ti/ccs1110/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --define=DeviceFamily_CC3220 --define=NORTOS_SUPPORT -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/Metro Aviation/workspace_v11/Thermostat_Project/MCU+Image/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1445775743: ../image.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs1110/ccs/utils/sysconfig_1.11.0/sysconfig_cli.bat" -s "C:/ti/simplelink_cc32xx_sdk_5_30_00_08/.metadata/product.json" --script "C:/Users/Metro Aviation/workspace_v11/Thermostat_Project/image.syscfg" -o "syscfg" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_drivers_net_wifi_config.json: build-1445775743 ../image.syscfg
syscfg/: build-1445775743

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1110/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -O3 --include_path="C:/Users/Metro Aviation/workspace_v11/Thermostat_Project" --include_path="C:/Users/Metro Aviation/workspace_v11/Thermostat_Project/MCU+Image" --include_path="C:/ti/simplelink_cc32xx_sdk_5_30_00_08/source" --include_path="C:/ti/simplelink_cc32xx_sdk_5_30_00_08/kernel/nortos" --include_path="C:/ti/simplelink_cc32xx_sdk_5_30_00_08/kernel/nortos/posix" --include_path="C:/ti/ccs1110/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --define=DeviceFamily_CC3220 --define=NORTOS_SUPPORT -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/Metro Aviation/workspace_v11/Thermostat_Project/MCU+Image/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


