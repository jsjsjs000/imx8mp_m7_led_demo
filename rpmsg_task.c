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

#include "common.h"
#include "rpmsg_task.h"
#include "i2c_task.h"

#define RPMSG_LITE_LINK_ID             (RL_PLATFORM_IMX8MP_M7_USER_LINK_ID)
#define RPMSG_LITE_SHMEM_BASE0         (VDEV0_VRING_BASE)
#define RPMSG_LITE_SHMEM_BASE1         (VDEV1_VRING_BASE)
#define RPMSG_LITE_NS_ANNOUNCE_STRING0 "rpmsg-openamp-channel-0"
#define RPMSG_LITE_NS_ANNOUNCE_STRING1 "rpmsg-openamp-channel-1"

#define APP_DEBUG_UART_BAUDRATE (115200U) /* Debug console baud rate. */
#define APP_TASK_STACK_SIZE (256U)
#define LOCAL_EPT_ADDR (30U)
#define APP_RPMSG_READY_EVENT_DATA (1U)

volatile rpmsg_message_t rpmsg_message;

volatile uint32_t clock_correction = 0;

static struct rpmsg_lite_instance *volatile rpmsg_instance = NULL;

static struct rpmsg_lite_endpoint *volatile rpmsg_endpoint = NULL;
static volatile rpmsg_queue_handle rpmsg_queue = NULL;

static void app_nameservice_isr_cb(uint32_t new_ept, const char *new_ept_name, uint32_t flags, void *user_data);

void rpmsg_initialize(void)
{
	copyResourceTable();
}

void rpmsg_task(void *pvParameters)
{
	PRINTF("RPMsg task started.\r\n");
	PRINTF("RPMSG Share Base Addr is 0x%x\r\n", RPMSG_LITE_SHMEM_BASE0);

	rpmsg_instance = rpmsg_lite_remote_init((void*)RPMSG_LITE_SHMEM_BASE0, RPMSG_LITE_LINK_ID, RL_NO_FLAGS);
	while (0 == rpmsg_lite_is_link_up(rpmsg_instance))
	{
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	PRINTF("RPMSG Link is up!\r\n");

	rpmsg_queue  = rpmsg_queue_create(rpmsg_instance);
	rpmsg_endpoint    = rpmsg_lite_create_ept(rpmsg_instance, LOCAL_EPT_ADDR, rpmsg_queue_rx_cb, rpmsg_queue);
	volatile rpmsg_ns_handle ns_handle = rpmsg_ns_bind(rpmsg_instance, app_nameservice_isr_cb, NULL);

		/// Delay to avoid NS announce message not being captured by master side.
	// SDK_DelayAtLeastUs(1000000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
	vTaskDelay(pdMS_TO_TICKS(1000));

	rpmsg_ns_announce(rpmsg_instance, rpmsg_endpoint, RPMSG_LITE_NS_ANNOUNCE_STRING0, (uint32_t)RL_NS_CREATE);
	PRINTF("RPMSG Nameservice announce sent.\r\n");

	/* Wait Hello handshake message from Remote Core. */
	volatile uint32_t remote_addr;
	char helloMsg[15];
	memset(helloMsg, 0, sizeof(helloMsg));
	rpmsg_queue_recv(rpmsg_instance, rpmsg_queue, (uint32_t*)&remote_addr, helloMsg, sizeof(helloMsg), NULL, RL_BLOCK);
	PRINTF("Received: '%s'\r\n", helloMsg);

	while (true)
	{
		rpmsg_queue_recv(rpmsg_instance, rpmsg_queue, (uint32_t*)&remote_addr, (char*)&rpmsg_message, sizeof(rpmsg_message_t),
				NULL, RL_BLOCK);
		uint32_t ms = xTaskGetTickCount() * 1000 / configTICK_RATE_HZ;
		clock_correction = ms - rpmsg_message.time_milliseconds;
		// PRINTF("Received: %d\r\n", rpmsg_message.time_milliseconds);

		if (rpmsg_message.led_r != LED_COMMAND_UNDEFINED)
		{
			led_r = rpmsg_message.led_r;
			led_g = rpmsg_message.led_g;
			led_b = rpmsg_message.led_b;
		}
		if (rpmsg_message.led_mode != LED_COMMAND_UNDEFINED)
		{
			led_mode = rpmsg_message.led_mode;
		}

		rpmsg_message.led_r = led_r;
		rpmsg_message.led_g = led_g;
		rpmsg_message.led_b = led_b;
		rpmsg_message.led_mode = led_mode;
		rpmsg_lite_send(rpmsg_instance, rpmsg_endpoint, remote_addr, (char*)&rpmsg_message, sizeof(rpmsg_message), RL_BLOCK);
	}

	rpmsg_lite_destroy_ept(rpmsg_instance, rpmsg_endpoint);
	rpmsg_endpoint = NULL;
	rpmsg_queue_destroy(rpmsg_instance, rpmsg_queue);
	rpmsg_queue = NULL;
	rpmsg_ns_unbind(rpmsg_instance, ns_handle);
	rpmsg_lite_deinit(rpmsg_instance);
	rpmsg_instance = NULL;

	vTaskSuspend(NULL);
}

static void app_nameservice_isr_cb(uint32_t new_ept, const char *new_ept_name, uint32_t flags, void *user_data)
{
}
