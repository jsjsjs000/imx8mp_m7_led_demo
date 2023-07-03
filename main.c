#include <stdlib.h>
#include <string.h>

/* Freescale includes. */
#include <fsl_device_registers.h>
#include <fsl_debug_console.h>
#include <fsl_gpio.h>
#include <fsl_uart_freertos.h>
#include <fsl_uart.h>
#include <fsl_wdog.h>

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>

/* Board config */
#include "board_cfg/board.h"
#include "board_cfg/clock_config.h"
#include "board_cfg/pin_mux.h"

#include "wdog.h"
#include "i2c_task.h"
#include "uart_task.h"
#include "rpmsg_task.h"

#define TASK_STACK_SIZE 1024

#define GPIO_LED_PORT  GPIO5
#define GPIO_LED_PIN   5U

#define master_task_PRIORITY (configMAX_PRIORITIES - 2)

static void gpio_init(void);
static void master_task(void *pvParameters);

void delay_ms(int ms)
{
	volatile int d = 0;
	for (int j = 0; j < ms * 75 * 1000; j++)
		d++;
}

void main(void)
{
	BOARD_InitMemory();
	BOARD_RdcInit();
	BOARD_InitBootPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();

/// wait few seconds for A53 boot
// #define WAIT_FOR_A53
#ifdef WAIT_FOR_A53
	PRINTF("\r\nStarting... ");
	for (int i = 0; i < 20; i++)
	{
		delay_ms(1000);
		PRINTF("%d ", i);
	}
	PRINTF("Started\r\n");
#endif

	PRINTF("\r\nCortex-M7 started.\r\n");

	wdog_reset_reason();
	wdog_init();
	gpio_init();
	i2c_master_initialize();
	uart_initialize();
	rpmsg_initialize();

	if (xTaskCreate(master_task, "Master_task", configMINIMAL_STACK_SIZE + TASK_STACK_SIZE, NULL,
			master_task_PRIORITY, NULL) != pdPASS)
	{
		PRINTF("Failed to create master task.\r\n");
		vTaskSuspend(NULL);
	}

	if (xTaskCreate(i2c_master_task, "I2C_task", TASK_STACK_SIZE, NULL,
			master_task_PRIORITY, NULL) != pdPASS)
	{
		PRINTF("Failed to create I2C task.\r\n");
		vTaskSuspend(NULL);
	}

	if (xTaskCreate(uart_task, "UART_task", TASK_STACK_SIZE, NULL, master_task_PRIORITY, NULL) != pdPASS)
	{
		PRINTF("Failed to create UART task.\r\n");
		vTaskSuspend(NULL);
	}

	if (xTaskCreate(rpmsg_task, "RPMsg_task", TASK_STACK_SIZE, NULL, master_task_PRIORITY, NULL) != pdPASS)
	{
		PRINTF("Failed to create RPMsg task.\r\n");
		vTaskSuspend(NULL);
	}

	PRINTF("Start scheduler.\r\n");
	vTaskStartScheduler();
	PRINTF("\r\nError: exit from scheduler.\r\n");
	while (true) ;
}

/// FreeRTOSConfig.h:
/// configCHECK_FOR_STACK_OVERFLOW 1
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	PRINTF("\r\nError: stack overflow\r\n");
}

static void gpio_init(void)
{
	gpio_pin_config_t led_config = { kGPIO_DigitalOutput, 0, kGPIO_IntLowLevel };
	GPIO_PinInit(GPIO_LED_PORT, GPIO_LED_PIN, &led_config);
}

static void master_task(void *pvParameters)
{
	PRINTF("Master task started.\r\n");

// int last_s = 0, s = 0;
	uint32_t ms, cycle;
	uint8_t led_on;
	while (true)
	{
		ms = xTaskGetTickCount() * 1000 / configTICK_RATE_HZ;
		ms -= clock_correction;
// s = ms / 1000;
		cycle = (ms / 100) % 20;
		led_on = cycle == 0 || cycle == 2;
		GPIO_PinWrite(GPIO_LED_PORT, GPIO_LED_PIN, led_on);

// if (s > last_s)
//  PRINTF("ms %d, clock_correction %d, ms-clock_correction %d \r\n", ms, clock_correction, ms - clock_correction);
// last_s = s;
		vTaskDelay(pdMS_TO_TICKS(10));

		// GPIO_PinWrite(GPIO_LED_PORT, GPIO_LED_PIN, 0);
		// vTaskDelay(pdMS_TO_TICKS(500));
		// GPIO_PinWrite(GPIO_LED_PORT, GPIO_LED_PIN, 1);
		// vTaskDelay(pdMS_TO_TICKS(500));

		WDOG_Refresh(WDOG_BASE);
	}

	vTaskSuspend(NULL);
}
