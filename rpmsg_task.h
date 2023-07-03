typedef struct
{
	int32_t time_milliseconds;
	uint8_t led_r;
	uint8_t led_g;
	uint8_t led_b;
	uint8_t led_mode;
} rpmsg_message_t;

extern volatile rpmsg_message_t rpmsg_message;
extern volatile uint32_t clock_correction;

extern void rpmsg_initialize(void);
extern void rpmsg_task(void *pvParameters);
