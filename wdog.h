#define WDOG_BASE       WDOG3
#define WDOG_IRQHandler WDOG3_IRQHandler

extern void wdog_reset_reason(void);
extern void wdog_init(void);
extern void reset(void);
