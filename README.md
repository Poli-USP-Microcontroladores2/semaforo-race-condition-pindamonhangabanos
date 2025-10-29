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
5.1 Código original(Guilherme):

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
    k_msleep(30);

    val = (shared_pattern & 0x02) ? 1U : 0U; /* green */
    gpio_pin_set_dt(&led_green, val);
   k_msleep(30);
    val = (shared_pattern & 0x04) ? 1U : 0U; /* blue */
    gpio_pin_set_dt(&led_blue, val);
   k_msleep(30);
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

    /* Indicate end by turning BLUE LED on steady */
    shared_pattern = 0x00;
    gpio_pin_set_dt(&led_red, 0);
    gpio_pin_set_dt(&led_green, 0);
    gpio_pin_set_dt(&led_blue, 1);

    printk("Demo finished — blue LED steady\n");
}


Código corrigido(Guilherme):

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

/* Shared global variable */
static volatile uint8_t shared_pattern = 0;

/* * 1. DEFINIÇÃO DO MUTEX
 * Define e inicializa estaticamente o mutex que protegerá o shared_pattern.
 */
K_MUTEX_DEFINE(pattern_mutex);


/* Helper que aplica LED outputs.
 * Esta função NÃO é mais o problema, pois só será chamada
 * de dentro de uma seção crítica protegida pelo mutex.
 * (Voltamos a usar k_busy_wait, pois k_msleep dentro de um 
 * mutex travado é uma má prática).
 */
static void apply_leds_pattern(void)
{
    uint8_t val;

    /* A thread que está aqui é dona do 'pattern_mutex',
     * então 'shared_pattern' não pode mudar. */

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

/* Thread A: toggles RED on/off repeatedly */
void thread_a(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    for (int i = 0; i < 40; i++) {
        
        /* 2. INÍCIO DA SEÇÃO CRÍTICA */
        k_mutex_lock(&pattern_mutex, K_FOREVER);
        
        shared_pattern = 0x01; /* request RED */
        apply_leds_pattern();

        /* 3. FIM DA SEÇÃO CRÍTICA */
        k_mutex_unlock(&pattern_mutex);

        k_msleep(80);

        /* --- Outra Seção Crítica --- */
        k_mutex_lock(&pattern_mutex, K_FOREVER);
        
        shared_pattern = 0x00; /* all off */
        apply_leds_pattern();
        
        k_mutex_unlock(&pattern_mutex);
        /* --- Fim da Seção Crítica --- */

        k_msleep(120);
    }

    printk("Thread A finished\n");
}

/* Thread B: cycles between GREEN and BLUE */
void thread_b(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    for (int i = 0; i < 40; i++) {
        
        /* 2. INÍCIO DA SEÇÃO CRÍTICA */
        k_mutex_lock(&pattern_mutex, K_FOREVER);
        
        shared_pattern = 0x02; /* request GREEN */
        apply_leds_pattern();
        
        /* 3. FIM DA SEÇÃO CRÍTICA */
        k_mutex_unlock(&pattern_mutex);

        k_msleep(100);

        /* --- Outra Seção Crítica --- */
        k_mutex_lock(&pattern_mutex, K_FOREVER);
        
        shared_pattern = 0x04; /* request BLUE */
        apply_leds_pattern();
        
        k_mutex_unlock(&pattern_mutex);
        /* --- Fim da Seção Crítica --- */

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

    printk("Starting MUTEX-safe LED demo\n");

    /* Configure LEDs from device-tree (igual a antes) */
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

    /* * 4. INICIALIZAÇÃO
     * Não é necessária uma chamada k_mutex_init() aqui, 
     * pois K_MUTEX_DEFINE já faz a inicialização.
     */

    /* Create both threads (igual a antes) */
    k_thread_create(&thread_a_data, stack_a, STACK_SIZE,
                      thread_a, NULL, NULL, NULL,
                      THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&thread_b_data, stack_b, STACK_SIZE,
                      thread_b, NULL, NULL, NULL,
                      THREAD_PRIORITY, 0, K_NO_WAIT);

    /* Wait for both threads to finish */
    k_thread_join(&thread_a_data, K_FOREVER);
    k_thread_join(&thread_b_data, K_FOREVER);

    /* Indicate end (igual a antes) */
    shared_pattern = 0x00;
    gpio_pin_set_dt(&led_red, 0);
    gpio_pin_set_dt(&led_green, 0);
    gpio_pin_set_dt(&led_blue, 1);


PARTE DO TIAGO

1. o código consertado pertence ao Bruno Mora

    printk("Demo finished — blue LED steady\n");
}
