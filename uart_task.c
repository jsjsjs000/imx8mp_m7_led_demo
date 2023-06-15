/* Freescale includes. */
#include <fsl_device_registers.h>
#include <fsl_debug_console.h>
#include <fsl_uart_freertos.h>
#include <fsl_uart.h>

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

#include "uart_commands.h"
#include "uart_task.h"

static uint8_t background_buffer[128];
static int input_line_index = 0;
static char input_line[512] = { 0 };
static char result[512] = { 0 };

static uart_rtos_handle_t handle;
static struct _uart_handle t_handle;

static uart_rtos_config_t uart_config =
{
	.baudrate    = 115200,
	.parity      = kUART_ParityDisabled,
	.stopbits    = kUART_OneStopBit,
	.buffer      = background_buffer,
	.buffer_size = sizeof(background_buffer),
};

void uart_initialize(void)
{
  NVIC_SetPriority(BOARD_UART_IRQn, 3);
}

void uart_task(void *pvParameters)
{
	PRINTF("UART task started.\r\n");

	uart_config.srcclk = BOARD_UART_CLK_FREQ;
	uart_config.base   = BOARD_UART;

	if (kStatus_Success != UART_RTOS_Init(&handle, &t_handle, &uart_config))
	{
		PRINTF("\r\nError: UART init error!\r\n");
		vTaskSuspend(NULL);
	}

	int error = kStatus_Success;
	size_t receive_count = 0;
	uint8_t recv_buffer;

	do
	{
		error = UART_RTOS_Receive(&handle, &recv_buffer, sizeof(recv_buffer), &receive_count);
		if (error == kStatus_UART_RxHardwareOverrun)
		{
			PRINTF("\r\nError: hardware buffer overrun!\r\n");
			vTaskSuspend(NULL);
		}
		if (error == kStatus_UART_RxRingBufferOverrun)
		{
			PRINTF("\r\nError: ring buffer overrun\r\n");
			vTaskSuspend(NULL);
		}
		if (receive_count > 0)
		{
			//	/// test echo
			// if (recv_buffer < 32 || recv_buffer > 127)
			// 	PRINTF("%d ", recv_buffer);
			// else
			// 	PRINTF("'%c' ", recv_buffer);

			if (recv_buffer == '\r' || recv_buffer == '\n')
			{
				input_line[input_line_index] = 0;
				// PRINTF("line: %s\r\n", input_line); /// test input line

				if (input_line_index > 0)
				{
					parse_line(input_line, result);
					int len = strlen(result);
					result[len++] = '\r';
					result[len++] = '\n';
					UART_RTOS_Send(&handle, (uint8_t*)result, len);
				}

				input_line_index = 0;
			}
			else if (input_line_index < sizeof(input_line))
				input_line[input_line_index++] = recv_buffer;
			else
			{
				input_line_index = 0;
				input_line[0] = 0;
			}
		}
	}
	while (true);

	UART_RTOS_Deinit(&handle);
	vTaskSuspend(NULL);
}
