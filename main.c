#include <stdlib.h>
#include <string.h>

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

#include "i2c_task.h"
#include "uart_task.h"
#include "rpmsg_task.h"

#define master_task_PRIORITY (configMAX_PRIORITIES - 2)

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

// PRINTF("\r\nA ");
// for (int i = 0; i < 20; i++)
// {
// 	delay_ms(1000);
// 	PRINTF("%d ", i);
// }
// PRINTF("B\r\n");

	i2c_master_initialize();
	uart_initialize();
	rpmsg_initialize();

	PRINTF("\r\nCortex-M7 started.\r\n");

	if (xTaskCreate(master_task, "Master_task", configMINIMAL_STACK_SIZE + 124, NULL,
			master_task_PRIORITY, NULL) != pdPASS)
	{
		PRINTF("Failed to create master task.\r\n");
		vTaskSuspend(NULL);
	}

	if (xTaskCreate(i2c_master_task, "I2C_task", 128, NULL,
			master_task_PRIORITY, NULL) != pdPASS)
	{
		PRINTF("Failed to create I2C task.\r\n");
		vTaskSuspend(NULL);
	}

	if (xTaskCreate(uart_task, "UART_task", 128, NULL, master_task_PRIORITY, NULL) != pdPASS)
	{
		PRINTF("Failed to create UART task.\r\n");
		vTaskSuspend(NULL);
	}

	if (xTaskCreate(rpmsg_task, "RPMsg_task", 128, NULL, master_task_PRIORITY, NULL) != pdPASS)
	{
		PRINTF("Failed to create RPMsg task.\r\n");
		vTaskSuspend(NULL);
	}

	PRINTF("Start scheduler.\r\n");
	vTaskStartScheduler();
	PRINTF("\r\nError: exit from scheduler.\r\n");
	while (true) ;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	PRINTF("\r\nError: stack overflow\r\n");
}

static void master_task(void *pvParameters)
{
	PRINTF("Master task started.\r\n");

	while (true)
	{
// PRINTF("Main task tick.\r\n");
		vTaskDelay(pdMS_TO_TICKS(5000));
	}

	vTaskSuspend(NULL);
}


/*

	SDK w /opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ/
Wygeneruj i pobierz SDK na:
# https://mcuxpresso.nxp.com/en/select
# https://www.nxp.com account: jsjsjs000@gmail.com, wCCR72B2aV@zZ8S
# Processors > i.MX > 8M Plus Quad > MIMX8ML8xxxLZ
# SDK v2.11.1, Build MCUXpresso SDK
# Download SDK > Download SDK archive including documentation
cd ~/Downloads
sudo mkdir -p /opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ
sudo tar -xf SDK_2_11_1_MIMX8ML8xxxLZ.tar.gz -C /opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ
cd /opt/MCUXpresso_SDK_2_11_1_MIMX8ML8xxxLZ
	# download phytec examples
sudo git clone https://git.phytec.de/phytec-mcux-boards -b SDK_2.11.1-phy

	Build prepare:
sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi make cmake
export ARMGCC_DIR=/usr
echo $ARMGCC_DIR
echo export ARMGCC_DIR=/usr >> ~/.bashrc

	Build:
cd armgcc
	# Windows:
clean.bat
build_debug.bat
build_release.bat
	# Linux:
./clean.sh
./build_debug.sh
./build_release.sh
make # next builds
	# Visual Studio Code:
Ctrl+Shift+B - run Makefile

	Add user to dialout group - access to UART without root permission
sudo adduser $USER dialout
# logout

	Debug in UART1/2:
minicom -w -D /dev/ttyUSB0
minicom -w -D /dev/ttyUSB1
	Connect to Linux via ssh
ssh root@192.168.30.11

	Set TFTP server:
sudo nano /etc/xinetd.d/tftp
------------------------------------------------------------
	server_args = -s /home/p1887/Desktop/imx8mp_uart_server/armgcc/debug
------------------------------------------------------------
sudo service xinetd restart

	Install AutoKey
sudo apt install autokey-gtk
	Configure shortcut Ctrl+Alt+Q
		Add new: TFTP in Terminal
			Command: tftp 0x48000000 imx8mp_uart_server.bin; cp.b 0x48000000 0x7e0000 0x20000; bootaux 0x7e0000
			Hotkey: Ctrl+Alt+Q
			Window Filter: gnome-terminal-server.Gnome-terminal
		Add new: minicom in Terminal
			Command: minicom -w -D /dev/ttyUSB
			Hotkey: Ctrl+Alt+M
			Window Filter: gnome-terminal-server.Gnome-terminal

	Download .bin file from TFTP server and run firmware on M7 core in U-boot:
tftp 0x48000000 imx8mp_uart_server.bin; cp.b 0x48000000 0x7e0000 0x20000; bootaux 0x7e0000
	or
# Press Ctrl+Alt+Q in minicom in Terminal

*/
