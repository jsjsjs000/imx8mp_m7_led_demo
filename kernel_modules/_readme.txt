~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx_rpmsg_connector.c
~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx_rpmsg_connector.ko

# copy kernel module to this repository
cp ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx_rpmsg_connector.c ~/Desktop/imx8mp_uart_server/kernel_modules/


# add new module to kernel configuration
~/linux-imx-phytec-v5.10.72_2.2.0-phy9/.config
CONFIG_REMOTEPROC=y
CONFIG_IMX_REMOTEPROC=y
CONFIG_IMX_DSP_REMOTEPROC=y
CONFIG_IMX_RPMSG_CONNECTOR=m

~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/Makefile
obj-$(CONFIG_IMX_RPMSG_CONNECTOR)	+= imx_rpmsg_connector.o

~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/modules.order
drivers/rpmsg/imx_rpmsg_connector.ko

~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/Kconfig
config IMX_RPMSG_CONNECTOR
	tristate "IMX RPMSG connector driver -- loadable modules only"
	default m
	depends on HAVE_IMX_RPMSG && m
	help
	  One 32bit unsigned int data transactions demoe between the A core
	  and the remote M core on the iMX AMP platforms.

	  Only the module mode is supported here, the demo would be kicked off
	  immediately when this module is insmoded.

# compile kernel
source /opt/ampliphy-vendor-xwayland/BSP-Yocto-NXP-i.MX8MP-PD22.1.1/environment-setup-cortexa53-crypto-phytec-linux
cd ~/linux-imx-phytec-v5.10.72_2.2.0-phy9
make -j16

# upload to i.MX8
scp ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx_rpmsg_connector.ko root@192.168.30.11:/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/rpmsg/

# run on i.MX8 Linux
insmod /lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/rpmsg/imx_rpmsg_connector.ko
# or
depmod   # recreate modules dependency list
modprobe imx_rpmsg_connector

# autorun module at boot
echo "imx_rpmsg_connector" >> /etc/modules-load.d/modules.conf
cat /etc/modules-load.d/modules.conf
