#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/virtio.h>
#include <linux/rpmsg.h>

#define MSG		"hello world!"    // $$ line 26

static struct task_struct *kthread;
static struct rpmsg_device *rpdev_;

static int kthread_loop(void *data)
{
	int err;
	unsigned int rpmsg_pingpong;

	usleep_range(6 * 1000 * 1000, 6 * 1000 * 1000 + 1);

	dev_info(&rpdev_->dev, "new channel_: 0x%x -> 0x%x!\n", rpdev_->src, rpdev_->dst);

	/*	send a message to our remote processor, and tell remote
			processor about this channel */
	err = rpmsg_send(rpdev_->ept, MSG, strlen(MSG));
	if (err)
	{
		dev_err(&rpdev_->dev, "rpmsg_send failed: %d\n", err);
		return err;
	}

	rpmsg_pingpong = 0;
	err = rpmsg_sendto(rpdev_->ept, (void *)(&rpmsg_pingpong), 4, rpdev_->dst);
	if (err)
	{
		dev_err(&rpdev_->dev, "rpmsg_send failed: %d\n", err);
		return err;
	}

	return 0;
}

static int rpmsg_pingpong_cb(struct rpmsg_device *rpdev, void *data, int len,
		void *priv, u32 src)
{
	int err;
	unsigned int rpmsg_pingpong;

	/* reply */
	rpmsg_pingpong = *(unsigned int *)data;
	pr_info("get_ %d (src: 0x%x)\n", rpmsg_pingpong, src);

	/* pingpongs should not live forever */
	if (rpmsg_pingpong > 100) {
		dev_info(&rpdev->dev, "goodbye!\n");
		return 0;
	}
	rpmsg_pingpong++;
	err = rpmsg_sendto(rpdev->ept, (void *)(&rpmsg_pingpong), 4, src);

	if (err)
		dev_err(&rpdev->dev, "rpmsg_send failed: %d\n", err);

	return err;
}

static int rpmsg_pingpong_probe(struct rpmsg_device *rpdev)
{
	rpdev_ = rpdev;

	kthread = kthread_create(kthread_loop, NULL, "mykthread");
	wake_up_process(kthread);

	return 0;
}

static void rpmsg_pingpong_remove(struct rpmsg_device *rpdev)
{
	dev_info(&rpdev->dev, "rpmsg pingpong driver is removed\n");
	kthread_stop(kthread);
}

static struct rpmsg_device_id rpmsg_driver_pingpong_id_table[] =
{
	{ .name	= "rpmsg-openamp-demo-channel" },
	{ .name	= "rpmsg-openamp-demo-channel-1" },
	{ },
};

static struct rpmsg_driver rpmsg_pingpong_driver =
{
	.drv.name	= KBUILD_MODNAME,
	.drv.owner	= THIS_MODULE,
	.id_table	= rpmsg_driver_pingpong_id_table,
	.probe		= rpmsg_pingpong_probe,
	.callback	= rpmsg_pingpong_cb,
	.remove		= rpmsg_pingpong_remove,
};

static int __init init(void)
{
	return register_rpmsg_driver(&rpmsg_pingpong_driver);
}

static void __exit fini(void)
{
	unregister_rpmsg_driver(&rpmsg_pingpong_driver);
}

module_init(init);
module_exit(fini);

MODULE_AUTHOR("PCO jarsulk");
MODULE_DESCRIPTION("iMX virtio remote processor messaging connector driver");
MODULE_LICENSE("GPL v2");

/*

# compile and upload to i.MX8:
make -j16 && scp ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/rpmsg/imx_rpmsg_connector.ko root@192.168.30.11:/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/rpmsg/ && ssh root@192.168.30.11 'reboot'

*/
