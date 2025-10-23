#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>    // Para rand() e srand()
#include <zephyr/sys/printk.h>

LOG_MODULE_REGISTER(race_example, LOG_LEVEL_INF);

// LEDs: KL25Z - led0 = vermelho, led2 = azul
#define LED_RED_NODE   DT_ALIAS(led0)
#define LED_BLUE_NODE  DT_ALIAS(led2)

static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);

// Função que alterna aleatoriamente um LED
void toggle_led_aleatorio(void)
{
    int escolha = rand() % 2;

    if (escolha == 0) {
        gpio_pin_toggle_dt(&led_red);
        LOG_INF("Toggle no LED VERMELHO (thread: %p)", k_current_get());
    } else {
        gpio_pin_toggle_dt(&led_blue);
        LOG_INF("Toggle no LED AZUL (thread: %p)", k_current_get());
    }
}

// Thread A
void thread_a(void *p1, void *p2, void *p3)
{
    while (1) {
        toggle_led_aleatorio();
        k_msleep(150);
    }
}

// Thread B
void thread_b(void *p1, void *p2, void *p3)
{
    while (1) {
        toggle_led_aleatorio();
        k_msleep(120);
    }
}

// Define as threads
K_THREAD_DEFINE(thread_a_id, 512, thread_a, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(thread_b_id, 512, thread_b, NULL, NULL, NULL, 5, 0, 0);

void main(void)
{
    // Inicializa gerador rand com base no uptime (sem seed, pode repetir no reboot)
    srand((unsigned int)k_uptime_get());

    if (!device_is_ready(led_red.port) || !device_is_ready(led_blue.port)) {
        LOG_ERR("GPIOs dos LEDs não estão prontos");
        return;
    }

    gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_INACTIVE);

    LOG_INF("Iniciando teste de Race Condition com LEDs");
}
