#include <stdint.h>

/* BCM2837 GPIO Register */

#define GPIO_BASE       0x3F200000UL

/* GPIO Function Select registers */
#define GPFSEL0         (*(volatile uint32_t *)(GPIO_BASE + 0x00))
#define GPFSEL1         (*(volatile uint32_t *)(GPIO_BASE + 0x04))
#define GPFSEL2         (*(volatile uint32_t *)(GPIO_BASE + 0x08))

/* GPIO Pin Output Set registers */
#define GPSET0          (*(volatile uint32_t *)(GPIO_BASE + 0x1C))

/* GPIO Pin Output Clear registers */
#define GPCLR0          (*(volatile uint32_t *)(GPIO_BASE + 0x28))

/* Configuration */

#define LED_GPIO_PIN    26


#define GPFSEL2_PIN26_MASK  (0x7U << 18)
#define GPFSEL2_PIN26_OUT   (0x1U << 18)

/* Dirty Delay */

static void delay(uint64_t count)
{
    volatile uint64_t i;
    for (i = 0; i < count; i++) {
        /* busy wait */
    }
}

/* Main */

void main(void)
{
    /* Configure GPIO 26 as output */
    uint32_t reg = GPFSEL2;
    reg &= ~GPFSEL2_PIN26_MASK;
    reg |=  GPFSEL2_PIN26_OUT;
    GPFSEL2 = reg;

    /* Blink forever */
    while (1) {
        GPSET0 = (1U << LED_GPIO_PIN);
        delay(5000000);

        GPCLR0 = (1U << LED_GPIO_PIN);
        delay(5000000);
    }
}
