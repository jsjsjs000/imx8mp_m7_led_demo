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

/* RPMsg */
#include <rpmsg_lite.h>
#include <rpmsg_queue.h>
#include <rpmsg_ns.h>
#include "rpmsg/rsc_table.h"




#define RPMSG_LITE_LINK_ID            (RL_PLATFORM_IMX8MP_M7_USER_LINK_ID)
#define RPMSG_LITE_SHMEM_BASE         (VDEV0_VRING_BASE)
#define RPMSG_LITE_NS_ANNOUNCE_STRING "rpmsg-openamp-demo-channel"
// #define RPMSG_LITE_MASTER_IS_LINUX

#define APP_DEBUG_UART_BAUDRATE (115200U) /* Debug console baud rate. */
#define APP_TASK_STACK_SIZE (256U)
#define LOCAL_EPT_ADDR (30U)
#define APP_RPMSG_READY_EVENT_DATA (1U)

typedef struct the_message
{
    uint32_t DATA;
} THE_MESSAGE, *THE_MESSAGE_PTR;

static volatile THE_MESSAGE msg = {0};
static char helloMsg[13];

static TaskHandle_t app_task_handle = NULL;

static struct rpmsg_lite_instance *volatile my_rpmsg = NULL;

static struct rpmsg_lite_endpoint *volatile my_ept = NULL;
static volatile rpmsg_queue_handle my_queue        = NULL;

static void app_nameservice_isr_cb(uint32_t new_ept, const char *new_ept_name, uint32_t flags, void *user_data);

void app_destroy_task(void)
{
	if (app_task_handle)
	{
		vTaskDelete(app_task_handle);
		app_task_handle = NULL;
	}

	if (my_ept)
	{
		rpmsg_lite_destroy_ept(my_rpmsg, my_ept);
		my_ept = NULL;
	}

	if (my_queue)
	{
		rpmsg_queue_destroy(my_rpmsg, my_queue);
		my_queue = NULL;
	}

	if (my_rpmsg)
	{
		rpmsg_lite_deinit(my_rpmsg);
		my_rpmsg = NULL;
	}
}



void rpmsg_initialize(void)
{
	copyResourceTable();
}

void rpmsg_task(void *pvParameters)
{
	PRINTF("RPMsg task started.\r\n");

	volatile uint32_t remote_addr;
	volatile rpmsg_ns_handle ns_handle;

	PRINTF("RPMSG Share Base Addr is 0x%x\r\n", RPMSG_LITE_SHMEM_BASE);

	my_rpmsg = rpmsg_lite_remote_init((void *)RPMSG_LITE_SHMEM_BASE, RPMSG_LITE_LINK_ID, RL_NO_FLAGS);

	while (0 == rpmsg_lite_is_link_up(my_rpmsg))
	{
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	PRINTF("Link is up!\r\n");

	my_queue  = rpmsg_queue_create(my_rpmsg);
	my_ept    = rpmsg_lite_create_ept(my_rpmsg, LOCAL_EPT_ADDR, rpmsg_queue_rx_cb, my_queue);
	ns_handle = rpmsg_ns_bind(my_rpmsg, app_nameservice_isr_cb, ((void *)0));
	/* Introduce some delay to avoid NS announce message not being captured by the master side.
			This could happen when the remote side execution is too fast and the NS announce message is triggered
			before the nameservice_isr_cb is registered on the master side. */

	// SDK_DelayAtLeastUs(1000000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
	vTaskDelay(pdMS_TO_TICKS(1000));

	(void)rpmsg_ns_announce(my_rpmsg, my_ept, RPMSG_LITE_NS_ANNOUNCE_STRING, (uint32_t)RL_NS_CREATE);
	PRINTF("RPMSG Nameservice announce sent.\r\n");

// #ifdef RPMSG_LITE_MASTER_IS_LINUX
	/* Wait Hello handshake message from Remote Core. */
	(void)rpmsg_queue_recv(my_rpmsg, my_queue, (uint32_t *)&remote_addr, helloMsg, sizeof(helloMsg), ((void *)0),
													RL_BLOCK);
// #endif /* RPMSG_LITE_MASTER_IS_LINUX */

	while (msg.DATA <= 100U)
	{
		PRINTF("Waiting for ping...\r\n");
		(void)rpmsg_queue_recv(my_rpmsg, my_queue, (uint32_t *)&remote_addr, (char *)&msg, sizeof(THE_MESSAGE),
														((void *)0), RL_BLOCK);
		msg.DATA++;
		PRINTF("Sending pong...\r\n");
		(void)rpmsg_lite_send(my_rpmsg, my_ept, remote_addr, (char *)&msg, sizeof(THE_MESSAGE), RL_BLOCK);
	}

	PRINTF("Ping pong done, deinitializing...\r\n");

	(void)rpmsg_lite_destroy_ept(my_rpmsg, my_ept);
	my_ept = ((void *)0);
	(void)rpmsg_queue_destroy(my_rpmsg, my_queue);
	my_queue = ((void *)0);
	(void)rpmsg_ns_unbind(my_rpmsg, ns_handle);
	(void)rpmsg_lite_deinit(my_rpmsg);
	my_rpmsg = ((void *)0);
	msg.DATA = 0U;

	PRINTF("Looping forever...\r\n");
	app_destroy_task();

	while (true)
	{
// PRINTF("RPMsg task tick.\r\n");
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

	vTaskSuspend(NULL);
}

static void app_nameservice_isr_cb(uint32_t new_ept, const char *new_ept_name, uint32_t flags, void *user_data)
{
}
