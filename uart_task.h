#include <fsl_device_registers.h>
#include <fsl_debug_console.h>

#define BOARD_UART          UART4
#define BOARD_UART_CLK_FREQ BOARD_DEBUG_UART_CLK_FREQ
#define BOARD_UART_IRQn     UART4_IRQn

extern void uart_initialize(void);
extern void uart_task(void *pvParameters);
