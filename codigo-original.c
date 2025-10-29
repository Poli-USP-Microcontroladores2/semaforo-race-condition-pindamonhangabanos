#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* Ajuste de acordo com os pinos do FRDM-KL25Z */
#define RED_LED_NODE   DT_ALIAS(led0)
#define GREEN_LED_NODE DT_ALIAS(led1)

static const struct gpio_dt_spec red_led = GPIO_DT_SPEC_GET(RED_LED_NODE, gpios);
static const struct gpio_dt_spec green_led = GPIO_DT_SPEC_GET(GREEN_LED_NODE, gpios);

#define STACK_SIZE 512
#define PRIORITY 5
#define DELAY_MS 100

/* Variável global compartilhada */
volatile int shared_counter = 0;

/* Mutex para proteger (habilite para corrigir race condition) */
K_MUTEX_DEFINE(counter_mutex);

/* Se quiser demonstrar o erro, comente as duas linhas de lock/unlock! */

/* Thread A */
void thread_a(void)
{
    while (1) {
        /* 🔴 Race condition demonstrativa */
        // k_mutex_lock(&counter_mutex, K_FOREVER);

        int local = shared_counter;
        local++;
        k_msleep(10); // simula atraso
        shared_counter = local;

        // k_mutex_unlock(&counter_mutex);

        gpio_pin_set_dt(&red_led, 1);
        k_msleep(DELAY_MS);
        gpio_pin_set_dt(&red_led, 0);

        printk("Thread A: shared_counter = %d\n", shared_counter);
    }
}

/* Thread B */
void thread_b(void)
{
    while (1) {
        // k_mutex_lock(&counter_mutex, K_FOREVER);

        int local = shared_counter;
        local++;
        k_msleep(10);
        shared_counter = local;

        // k_mutex_unlock(&counter_mutex);

        gpio_pin_set_dt(&green_led, 1);
        k_msleep(DELAY_MS);
        gpio_pin_set_dt(&green_led, 0);

        printk("Thread B: shared_counter = %d\n", shared_counter);
    }
}

/* Criação das threads */
K_THREAD_DEFINE(thread_a_id, STACK_SIZE, thread_a, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(thread_b_id, STACK_SIZE, thread_b, NULL, NULL, NULL, PRIORITY, 0, 0);

void main(void)
{
    printk("=== Exemplo de Race Condition no FRDM-KL25Z (Zephyr 3.4.x) ===\n");

    if (!device_is_ready(red_led.port) || !device_is_ready(green_led.port)) {
        printk("Erro: LEDs não estão prontos\n");
        return;
    }

    gpio_pin_configure_dt(&red_led, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&green_led, GPIO_OUTPUT_INACTIVE);

    printk("Threads iniciadas...\n");
}
