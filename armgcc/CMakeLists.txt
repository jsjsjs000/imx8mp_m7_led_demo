# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 3.10.0)

# THE VERSION NUMBER
SET (Tutorial_VERSION_MAJOR 1)
SET (Tutorial_VERSION_MINOR 0)

# ENABLE ASM
ENABLE_LANGUAGE(ASM)

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)

SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

# CURRENT DIRECTORY
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})
SET(LIBRARY_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})

project(imx8mp_uart_server)

set(MCUX_SDK_PROJECT_NAME imx8mp_uart_server.elf)

include(${ProjDirPath}/flags.cmake)
include(${ProjDirPath}/config.cmake)

add_executable(${MCUX_SDK_PROJECT_NAME}
	"${ProjDirPath}/../main.c"
	"${ProjDirPath}/../FreeRTOSConfig.h"
	"${ProjDirPath}/../board_cfg/pin_mux.c"
	"${ProjDirPath}/../board_cfg/pin_mux.h"
	"${ProjDirPath}/../board_cfg/board.c"
	"${ProjDirPath}/../board_cfg/board.h"
	"${ProjDirPath}/../board_cfg/clock_config.c"
	"${ProjDirPath}/../board_cfg/clock_config.h"
	"${ProjDirPath}/../board_cfg/fsl_iomuxc.h"
	"${ProjDirPath}/../board_cfg/empty_rsc_table.c"
	"${ProjDirPath}/../rpmsg_config.h"
	"${ProjDirPath}/../rpmsg/rsc_table.c"
	"${ProjDirPath}/../rpmsg/rsc_table.h"
	"${ProjDirPath}/../rpmsg/remoteproc.h"
	"${ProjDirPath}/../common.c"
	"${ProjDirPath}/../common.h"
	"${ProjDirPath}/../wdog.c"
	"${ProjDirPath}/../wdog.h"
	"${ProjDirPath}/../i2c_task.c"
	"${ProjDirPath}/../i2c_task.h"
	"${ProjDirPath}/../uart_commands.c"
	"${ProjDirPath}/../uart_commands.h"
	"${ProjDirPath}/../uart_task.c"
	"${ProjDirPath}/../uart_task.h"
	"${ProjDirPath}/../rpmsg_task.c"
	"${ProjDirPath}/../rpmsg_task.h"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${ProjDirPath}/..
)

set(CMAKE_MODULE_PATH
	/opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ/middleware/multicore
	/opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ/rtos/freertos/freertos_kernel
	/opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ/devices/MIMX8ML8/drivers
	/opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ/devices/MIMX8ML8
	/opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ/devices/MIMX8ML8/utilities
	/opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ/drivers
	/opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ/components/uart
	/opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ/components/serial_manager
	/opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ/components/lists
	/opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ/CMSIS/Core/Include
)

# include modules
include(driver_clock_MIMX8ML8)
include(driver_common_MIMX8ML8)
include(driver_audiomix_MIMX8ML8)
include(driver_wdog01_MIMX8ML8)
include(device_MIMX8ML8_CMSIS_MIMX8ML8)
include(utility_debug_console_MIMX8ML8)
include(component_iuart_adapter_MIMX8ML8)
include(component_serial_manager_MIMX8ML8)
include(component_lists_MIMX8ML8)
include(component_serial_manager_uart_MIMX8ML8)
include(driver_igpio_MIMX8ML8)
include(driver_iuart_MIMX8ML8)
include(driver_ii2c_MIMX8ML8)
include(device_MIMX8ML8_startup_MIMX8ML8)
include(utility_assert_MIMX8ML8)
include(CMSIS_Include_core_cm_MIMX8ML8)
include(utilities_misc_utilities_MIMX8ML8)
include(device_MIMX8ML8_system_MIMX8ML8)

# FreeRTOS modules
include(middleware_freertos-kernel_MIMX8ML8)
include(middleware_freertos-kernel_extension_MIMX8ML8)
include(middleware_freertos-kernel_heap_4_MIMX8ML8)
include(driver_iuart_freertos_MIMX8ML8)
include(driver_ii2c_freertos_MIMX8ML8)

# RPMsg modules
include(middleware_multicore_rpmsg_lite_MIMX8ML8)
include(middleware_multicore_rpmsg_lite_imx8mp_m7_freertos_MIMX8ML8)
include(middleware_multicore_rpmsg_lite_freertos_MIMX8ML8)
include(driver_mu_MIMX8ML8)
include(driver_rdc_MIMX8ML8)

# check armgcc/config.cmake

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)
target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE m)
target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE c)
target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE gcc)
target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE nosys)
TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY}
-Obinary ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME} ${EXECUTABLE_OUTPUT_PATH}/imx8mp_uart_server.bin)
