#include <stdlib.h>
#include <string.h>

/* Freescale includes. */
#include <fsl_device_registers.h>
#include <fsl_debug_console.h>
#include <fsl_gpio.h>
#include <fsl_uart_freertos.h>
#include <fsl_uart.h>
#include <fsl_wdog.h>

/* Board config */
#include "board_cfg/board.h"
#include "board_cfg/clock_config.h"
#include "board_cfg/pin_mux.h"

#include "wdog.h"

static wdog_config_t wdog_config;

void wdog_reset_reason(void)
{
	uint16_t reset_flag = WDOG_GetStatusFlags(WDOG_BASE);

	switch (reset_flag & (kWDOG_PowerOnResetFlag | kWDOG_TimeoutResetFlag | kWDOG_SoftwareResetFlag))
	{
		case kWDOG_PowerOnResetFlag:
			PRINTF("Last reset: Power On Reset!\r\n");
			break;
		case kWDOG_TimeoutResetFlag:
			PRINTF("Last reset: Time Out Reset!\r\n");
			break;
		case kWDOG_SoftwareResetFlag:
			PRINTF("Last reset: Software Reset!\r\n");
			break;
		default:
			PRINTF("Last reset: Er>ror status!\r\n");
			break;
	}
}

void wdog_init(void)
{
	/*
		* wdogConfig->enableWdog = true;
		* wdogConfig->workMode.enableWait = true;
		* wdogConfig->workMode.enableStop = false;
		* wdogConfig->workMode.enableDebug = false;
		* wdogConfig->enableInterrupt = false;
		* wdogConfig->enablePowerdown = false;
		* wdogConfig->resetExtension = false;
		* wdogConfig->timeoutValue = 0xFFU;
		* wdogConfig->interruptTimeValue = 0x04u;
		*/
	WDOG_GetDefaultConfig(&wdog_config);
	wdog_config.timeoutValue = 0xFU; /* Timeout value is (0xF + 1)/2 = 8 sec. */

		/// refresh function using interrupt
	// 	wdog_config.enableInterrupt    = true;
	// 	wdog_config.interruptTimeValue = 0x4U; /* Interrupt occurred (0x4)/2 = 2 sec before WDOG timeout. */

	WDOG_Init(WDOG_BASE, &wdog_config);
}

void reset(void)
{
	WDOG_TriggerSystemSoftwareReset(WDOG_BASE);
}

void WDOG3_IRQHandler(void)
{
	PRINTF("\r\nError: Before watchdog reset callback.\r\n");
	WDOG_ClearInterruptStatus(WDOG_BASE, kWDOG_InterruptFlag);
	/* User code. User can do urgent case before timeout reset.
		* IE. user can backup the ram data or ram log to flash.
		* the period is set by config.interruptTimeValue, user need to
		* check the period between interrupt and timeout. */
}
