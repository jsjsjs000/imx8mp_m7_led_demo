#define I2C_MASTER_BASE I2C2
#define I2C_MASTER_IRQN I2C2_IRQn
#define I2C_MASTER_CLK_FREQ                                                                 \
		(CLOCK_GetPllFreq(kCLOCK_SystemPll1Ctrl) / (CLOCK_GetRootPreDivider(kCLOCK_RootI2c2)) / \
		(CLOCK_GetRootPostDivider(kCLOCK_RootI2c2)) / 5)                           /* SYSTEM PLL1 DIV5 */

#define I2C_MASTER ((I2C_Type *)I2C_MASTER_BASE)

#define I2C_MASTER_SLAVE_ADDR_7BIT (0x62U) // LED driver
#define I2C_BAUDRATE               (100000)   /* 100K */
#define I2C_DATA_LENGTH            (256)      /* MAX is 256 */

#define master_task_PRIORITY (configMAX_PRIORITIES - 2)

#define LED_COMMAND_UNDEFINED  255

enum led_mode_t { LED_MODE_AUTO, LED_MODE_MANUAL };

extern volatile uint8_t led_r;
extern volatile uint8_t led_g;
extern volatile uint8_t led_b;
extern volatile enum led_mode_t led_mode;

extern void i2c_master_initialize(void);
extern void i2c_master_task(void *pvParameters);
extern uint8_t led_command_to_led_i2c(uint8_t a);
extern uint8_t led_i2c_to_led_command(uint8_t a);
