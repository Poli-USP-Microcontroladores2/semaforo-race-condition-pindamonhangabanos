#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define THREAD_PRIORITY 5

/* Device-tree LED specs using DT_ALIAS */
static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

/* Shared global variable intentionally unprotected to create a race condition */
static volatile uint8_t shared_pattern = 0;

/* Helper that applies LED outputs reading the shared variable repeatedly.
 * Intentionally reads the shared variable separately for each LED with small delays
 * between writes — this increases the chance of interleaving and visible color mixing.
 */
static void apply_leds_with_race(void)
{
    uint8_t val;

    val = (shared_pattern & 0x01) ? 1U : 0U; /* red */
    gpio_pin_set_dt(&led_red, val);
    k_busy_wait(3000);

    val = (shared_pattern & 0x02) ? 1U : 0U; /* green */
    gpio_pin_set_dt(&led_green, val);
    k_busy_wait(3000);

    val = (shared_pattern & 0x04) ? 1U : 0U; /* blue */
    gpio_pin_set_dt(&led_blue, val);
    k_busy_wait(3000);
}

/* Thread A: toggles RED on/off repeatedly by writing shared_pattern = 0x01 or 0x00 */
void thread_a(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    for (int i = 0; i < 40; i++) {
        shared_pattern = 0x01; /* request RED */
        k_busy_wait(2000);
        apply_leds_with_race();

        k_msleep(80);

        shared_pattern = 0x00; /* all off */
        k_busy_wait(2000);
        apply_leds_with_race();

        k_msleep(120);
    }

    printk("Thread A finished\n");
}

/* Thread B: cycles between GREEN and BLUE by writing shared_pattern = 0x02 or 0x04 */
void thread_b(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    for (int i = 0; i < 40; i++) {
        shared_pattern = 0x02; /* request GREEN */
        k_busy_wait(1500);
        apply_leds_with_race();

        k_msleep(100);

        shared_pattern = 0x04; /* request BLUE */
        k_busy_wait(1500);
        apply_leds_with_race();

        k_msleep(140);
    }

    printk("Thread B finished\n");
}

/* Thread stacks and thread control blocks */
K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);
static struct k_thread thread_a_data;
static struct k_thread thread_b_data;

void main(void)
{
    int ret;

    printk("Starting race-condition LED demo\n");

    /* Configure LEDs from device-tree */
    ret = gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_LOW);
    if (ret) {
        printk("Failed to configure red LED\n");
        return;
    }
    ret = gpio_pin_configure_dt(&led_green, GPIO_OUTPUT_LOW);
    if (ret) {
        printk("Failed to configure green LED\n");
        return;
    }
    ret = gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_LOW);
    if (ret) {
        printk("Failed to configure blue LED\n");
        return;
    }

    /* Create both threads (they start immediately) */
    k_thread_create(&thread_a_data, stack_a, STACK_SIZE,
                    thread_a, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&thread_b_data, stack_b, STACK_SIZE,
                    thread_b, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    /* Wait for both threads to finish */
    k_thread_join(&thread_a_data, K_FOREVER);
    k_thread_join(&thread_b_data, K_FOREVER);

    /* Indicate end by turning BLUE LED on steady2 */
    shared_pattern = 0x00;
    gpio_pin_set_dt(&led_red, 0);
    gpio_pin_set_dt(&led_green, 0);
    gpio_pin_set_dt(&led_blue, 1);

    printk("Demo finished — blue LED steady\n");
}
