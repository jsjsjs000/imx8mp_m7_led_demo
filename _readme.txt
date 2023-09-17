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

# optional build only to assembler code:
# flags.cmake:
# SET(CMAKE_C_FLAGS_DEBUG " \
# 	-fverbose-asm \
# 	-S \

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
	server_args = -s /home/p2119/jarsulk-pco/programs/imx8mp/m7/imx8mp_m7_led_demo/armgcc/debug/
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

	__NOP(); // No Operation
	__WFI(); // Wait For Interrupt
	__WFE(); // Wait For Event
	__SEV(); // Send Event
