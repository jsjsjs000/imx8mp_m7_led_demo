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

extern void i2c_master_initialize(void);
extern void i2c_master_task(void *pvParameters);
