#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/printk.h>

#define STACK_SIZE 512
#define PRIORITY 5

/* Nome do dispositivo de LED (verifique no device tree da KL25Z) */
#define LED_PORT DT_LABEL(DT_NODELABEL(gpioe))
#define RED_PIN 29   /* PTE29 - LED vermelho */
#define GREEN_PIN 31 /* PTE31 - LED verde */

static const struct device *gpio_dev;

/* Variável compartilhada entre as threads */
volatile int shared_counter = 0;

/* Delay entre alterações */
#define DELAY_MS 100

/* Thread 1 - incrementa o contador e acende LED vermelho */
void thread_a(void)
{
    while (1) {
        int local = shared_counter;
        local++;
        /* Simula tempo de processamento antes de escrever de volta */
        k_msleep(10);
        shared_counter = local;

        gpio_pin_set(gpio_dev, RED_PIN, 1);
        k_msleep(DELAY_MS);
        gpio_pin_set(gpio_dev, RED_PIN, 0);

        printk("Thread A: shared_counter = %d\n", shared_counter);
    }
}

/* Thread 2 - também incrementa o contador e acende LED verde */
void thread_b(void)
{
    while (1) {
        int local = shared_counter;
        local++;
        /* Simula tempo de processamento antes de escrever de volta */
        k_msleep(10);
        shared_counter = local;

        gpio_pin_set(gpio_dev, GREEN_PIN, 1);
        k_msleep(DELAY_MS);
        gpio_pin_set(gpio_dev, GREEN_PIN, 0);

        printk("Thread B: shared_counter = %d\n", shared_counter);
    }
}

/* Criação das threads */
K_THREAD_DEFINE(thread_a_id, STACK_SIZE, thread_a, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(thread_b_id, STACK_SIZE, thread_b, NULL, NULL, NULL, PRIORITY, 0, 0);

void main(void)
{
    printk("Iniciando exemplo de Race Condition no KL25Z com Zephyr!\n");

    gpio_dev = device_get_binding(LED_PORT);
    if (!gpio_dev) {
        printk("Erro ao acessar GPIOs do LED!\n");
        return;
    }

    gpio_pin_configure(gpio_dev, RED_PIN, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure(gpio_dev, GREEN_PIN, GPIO_OUTPUT_INACTIVE);

    printk("Threads iniciadas...\n");
}

