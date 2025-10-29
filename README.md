Integrantes:
- Guilherme da Silva Fernandes
- Bruno Mora
- Tiago Hayashi

# Parte do Bruno:

Atividade: Resolução de Race Condition com Semáforo

   - Uso compartilhado de uma variável global

## Planejamento de Testes
| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | Existem duas threads com a mesma prioridade que incrementam em uma variável global. A thread deve receber a variável global "shared_counter"| Thread recebe a variável -> Incrementa na variável -> Apenas vai salvar na variável o valor incrementado pela última thread executada | A variável global imprimida deve ser igual em ambas as threads caso ocorra racing condition |
| 2 | Existem duas threads com a mesma prioridade que incrementam em uma variável global. A thread deve receber a variável global "shared_counter"| Thread recebe a variável -> Incrementa na variável -> Apenas vai salvar na variável o valor incrementado pela última thread executada | Se a thread retornar o dobro do valor do print anterior, não está acontecendo racing condition, porém, os dados não estão sendo obtidos levando em consideração o incremento de cada thread |
| 3 | Existem duas threads com a mesma prioridade que incrementam em uma variável global. A thread deve receber a variável global "shared_counter"| Thread recebe a variável -> Incrementa na variável -> Apenas vai salvar na variável o valor incrementado pela última thread executada | Se a thread retornar o valor anterior +1 em relação a outra thread, não acontece racing condition e obtém-se os dados corretos do incremento de cada thread |

## O que estava errado antes?
  Duas threads compartilham uma variável global de contagem e cada thread incrementa nessa variável. O problema acontece na aquisição da variável por ambas as threads, que apenas obtém o valor original sem considerar o incremento da outra thread por estarem compartilhando recursos e terem o mesmo nível de prioridade.
## O que mudou com a correção?
  Foi aplicado um MUTEX quando a aquisição da variável global é feita, assim, o mesmo não pode ser interrompido quando está em execução. Dessa forma, o valor utilizado pelas threads é incrementado corretamente e salvo na variavel global para que a próxima thread consiga incrementar coretamente.
## O comportamento agora está estável?
  Agora, a função não é interrompida no meio e o comportamento do LED no microcontrolador está estável, já que a variável salva não muda ao retornar para a thread após a interrupção.

## Evidências

Teste antes de colocar o MUTEX

Casos de Teste: [1] -> Ocorreu como esperado, portanto, está acontecendo racing condition [2] e [3] -> Segundo as condições, está ocorrendo racing condition pois elas não são cumpridas.

<img width="391" height="172" alt="image" src="https://github.com/user-attachments/assets/97dad3b0-82d8-4ed4-86be-82060657b692" />

Teste depois de colocar o MUTEX

Casos de Teste: [1] -> A condição não foi cumprida, portanto, não está acontecendo racing condition [2] e [3] -> Segundo as condições, os dados estão sendo obtidos de maneira imprecisa pois está obtendo o "shared counter" após das threads.

<img width="383" height="161" alt="image" src="https://github.com/user-attachments/assets/398b676a-f730-412a-8674-f78425f56649" />


Teste depois de colocar o MUTEX e mover a posição dos "printk" para dentro do MUTEX

# Parte do Guilherme:

## Revisão do código anterior: 
O código do integrante Bruno Mora foi testado e percebi que o motivo de acontecer a race condition foi o k_yield entre as leituras e escritas das threads A e B, que verificam se outras threads estão prontas e passam o uso da cpu para frente. Por conta disso, as tarefas pausaram o que estavam fazendo no meio e permitiram que a outra thread iniciasse, ou seja, a variável global contador perdeu alguns aumentos, visto que A e B fazem a mesma leitura da variável global. Devido a isso ao final da execução, será possível ver que os logs indicarão que houve race condition.
<img width="1027" height="269" alt="erro" src="https://github.com/user-attachments/assets/5e5837ba-67a4-403c-9c35-a2705770ab47" />

## Planejamento de testes:
<img width="1440" height="123" alt="image2" src="https://github.com/user-attachments/assets/5bedaaa6-fe8f-48f3-8120-7c5ea51dfe7e" />

## Correção e reteste:
Após a correção, que utilizou mutex para trancar a thread enquanto ela funcionava e destrancar após o término da contagem, o código funcionou perfeitamente sem nenhuma race condition. Isso se deve a capacidade dos mutex de bloquear o escalonador e não permitir que seções críticas sejam interrompidas, tornando o código estável.
<img width="1138" height="245" alt="acerto" src="https://github.com/user-attachments/assets/ff6ae079-797e-428e-92b2-2a88914f5d55" />

Planejamento pós mudanças:
<img width="1439" height="122" alt="image4" src="https://github.com/user-attachments/assets/16ef106b-e673-478f-846b-fca4c2132f16" />

## Avaliação Interna:
A race condition observada no código original se devia ao compartilhamento da variável global, sem proteção, por duas threads. Esse erro pode ser facilmente evitado utilizando semáforos e o semáforo que utilizei foi um binário do tipo mutex. Ele foi responsável por proteger partes críticas do funcionamento do código e permitir que a race condition jamais aconteça, tornando o código estável e funcional.

PARTE DO TIAGO

1. o código do guilherme apresenta uma race condition entre as threads A e B devido a amb as utilizarem a variável global shared_pattern e a função apply_leds_pattern, gerando interrupções no código e comportamentos inesperados, como duas leds ligarem ao mesmo tempo.

2. planejamento dos testes:

   teste |                 pré condição                                |       etapas de teste              | pós condição
     1   |threads A e B com mesma prioridade e desprotegidas           | verificar funcionamento do código  |   race condition
     2   |threads A e B com prioridades diferentes e desprotegidas     |usar prioridades diferentes         |   race condition
     3   |threads A e B com mesma prioridade e protegidas              | utilizar mutex                     |   código funcional

   3. Correção do código: para corrigir o código foi utilizado mutex em ambas as threads assim que elas modificam a variável global ou acessam a função.
  
código original(tiago):

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

código corrigido(tiago):

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
    k_mutex_lock(&rand_mutex, K_FOREVER);

    int escolha = rand() % 2;
    
    k_mutex_unlock(&rand_mutex);

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
