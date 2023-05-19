#include <stdlib.h>
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "uart_commands.h"

void main(void)
{
	BOARD_InitMemory();
	BOARD_RdcInit();
	BOARD_InitBootPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();

	PRINTF("\r\nCortex-M7 started.\r\n");

	int input_line_index = 0;
	char input_line[1024] = { 0 };
	while (true)
	{
		char c = GETCHAR();
		if (c == '\r' || c == '\n')
		{
			input_line[input_line_index] = 0;
			//PRINTF("line: %s\r\n", input_line);

			char result[1024] = { 0 };
			parse_line(input_line, result);
			PRINTF("%s\r\n", result);

			input_line_index = 0;
		}
		else if (input_line_index < sizeof(input_line))
			input_line[input_line_index++] = c;
		else
		{
			input_line_index = 0;
			input_line[0] = 0;
		}

		// PUTCHAR(c);
		// if (c == '\r')
		// 	PUTCHAR('\n');
	}
}

/*

	SDK w ~/Downloads/sdk/
Wygeneruj i pobierz SDK na:
# https://mcuxpresso.nxp.com/en/select
# https://www.nxp.com account: jsjsjs000@gmail.com, wCCR72B2aV@zZ8S
# Processors > i.MX > 8M Plus Quad > MIMX8ML8xxxLZ
# SDK v2.11.1, Build MCUXpresso SDK
# Download SDK > Download SDK archive including documentation
cd ~/Downloads
mkdir -p sdk
cd sdk
tar -xf SDK_2_11_1_MIMX8ML8xxxLZ.tar.gz
git clone https://git.phytec.de/phytec-mcux-boards --branch SDK_2.11.1-phy

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
	# Visual Studio Code:
Ctrl+Shift+B - run Makefile

	Debug in UART1/2:
sudo minicom -w -D /dev/ttyUSB0
sudo minicom -w -D /dev/ttyUSB1

	Set TFTP server:
sudo nano /etc/xinetd.d/tftp
------------------------------------------------------------
	server_args = -s /home/p1887/Pulpit/imx8mp_uart_server/armgcc/debug
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
			Command: sudo minicom -w -D /dev/ttyUSB
			Hotkey: Ctrl+Alt+M
			Window Filter: gnome-terminal-server.Gnome-terminal

	Download .bin file from TFTP server and run firmware on M7 core in U-boot:
tftp 0x48000000 imx8mp_uart_server.bin; cp.b 0x48000000 0x7e0000 0x20000; bootaux 0x7e0000
	or
# Press Ctrl+Alt+Q in minicom in Terminal

*/
