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
	"${ProjDirPath}/../pin_mux.c"
	"${ProjDirPath}/../pin_mux.h"
	"${ProjDirPath}/../board.c"
	"${ProjDirPath}/../board.h"
	"${ProjDirPath}/../clock_config.c"
	"${ProjDirPath}/../clock_config.h"
	"${ProjDirPath}/../fsl_iomuxc.h"
	"${ProjDirPath}/../empty_rsc_table.c"
	"${ProjDirPath}/../uart_commands.c"
	"${ProjDirPath}/../uart_commands.h"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${ProjDirPath}/..
)

set(CMAKE_MODULE_PATH
	~/Downloads/sdk/devices/MIMX8ML8/drivers
	~/Downloads/sdk/devices/MIMX8ML8
	~/Downloads/sdk/devices/MIMX8ML8/utilities
	~/Downloads/sdk/components/uart
	~/Downloads/sdk/components/serial_manager
	~/Downloads/sdk/components/lists
	~/Downloads/sdk/CMSIS/Core/Include
)

# include modules
include(driver_clock_MIMX8ML8)

include(driver_common_MIMX8ML8)

include(driver_rdc_MIMX8ML8)

include(driver_audiomix_MIMX8ML8)

include(device_MIMX8ML8_CMSIS_MIMX8ML8)

include(utility_debug_console_MIMX8ML8)

include(component_iuart_adapter_MIMX8ML8)

include(component_serial_manager_MIMX8ML8)

include(component_lists_MIMX8ML8)

include(component_serial_manager_uart_MIMX8ML8)

include(driver_iuart_MIMX8ML8)

include(device_MIMX8ML8_startup_MIMX8ML8)

include(utility_assert_MIMX8ML8)

include(utilities_misc_utilities_MIMX8ML8)

include(CMSIS_Include_core_cm_MIMX8ML8)

include(device_MIMX8ML8_system_MIMX8ML8)


TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE m)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE c)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE gcc)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE nosys)

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY}
-Obinary ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME} ${EXECUTABLE_OUTPUT_PATH}/imx8mp_uart_server.bin)
