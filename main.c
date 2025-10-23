#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

// Configuração do LOG (Módulo de Log)
// O nome do módulo deve ser definido no Kconfig ou diretamente aqui
LOG_MODULE_REGISTER(RACE_DEMO, LOG_LEVEL_INF);

/* Tamanho da pilha para as threads */
#define STACKSIZE 1024

/* Prioridade das threads. A prioridade igual favorece a preempção baseada em time-slicing */
#define PRIORITY 5

/* Número de incrementos que cada thread fará */
#define NUM_INCREMENTOS 10000

/* Recurso compartilhado - a variável que sofrerá a condição de corrida */
volatile int contador_compartilhado = 0;

/* --- Protótipos das Threads --- */
void thread_a_entry(void);
void thread_b_entry(void);

/* --- Definição das Stacks e Threads --- */

K_THREAD_STACK_DEFINE(thread_a_stack, STACKSIZE);
struct k_thread thread_a_data;

K_THREAD_STACK_DEFINE(thread_b_stack, STACKSIZE);
struct k_thread thread_b_data;


/**
 * @brief Função de entrada da Thread A.
 *
 * Tenta incrementar o recurso compartilhado NUM_INCREMENTOS vezes sem sincronização.
 */
void thread_a_entry(void)
{
    LOG_INF("Thread A iniciada. Incrementando o contador...");

    for (int i = 0; i < NUM_INCREMENTOS; i++) {
        // Início da Seção Crítica (Não Protegida!)
        // O compilador pode otimizar isso, e o RTOS pode preempcionar no meio:
        // 1. Lê contador_compartilhado para um registrador (R1 = contador_compartilhado)
        // 2. Incrementa o registrador (R1 = R1 + 1) -> **Pode ocorrer preempção aqui!**
        // 3. Escreve o registrador de volta (contador_compartilhado = R1)
        contador_compartilhado = contador_compartilhado + 1;
        // Fim da Seção Crítica
    }

    LOG_INF("Thread A terminou. Contador final por A: %d", contador_compartilhado);
}

/**
 * @brief Função de entrada da Thread B.
 *
 * Tenta incrementar o recurso compartilhado NUM_INCREMENTOS vezes sem sincronização.
 */
void thread_b_entry(void)
{
    LOG_INF("Thread B iniciada. Incrementando o contador...");

    for (int i = 0; i < NUM_INCREMENTOS; i++) {
        // Início da Seção Crítica (Não Protegida!)
        // A mesma sequência vulnerável de Leitura-Modificação-Escrita.
        contador_compartilhado = contador_compartilhado + 1;
        // Fim da Seção Crítica
    }

    LOG_INF("Thread B terminou. Contador final por B: %d", contador_compartilhado);
}


/**
 * @brief Função principal do aplicativo.
 *
 * Cria e inicia as duas threads.
 */
int main(void)
{
    int valor_esperado = 2 * NUM_INCREMENTOS;

    LOG_INF("Demonstração de Condição de Corrida (Race Condition) no Zephyr.");
    LOG_INF("Valor de incremento por thread: %d", NUM_INCREMENTOS);
    LOG_INF("Valor final **ESPERADO**: %d", valor_esperado);
    LOG_INF("-------------------------------------------------------");

    // Cria e inicia a Thread A
    k_thread_create(&thread_a_data, thread_a_stack,
                    STACKSIZE,
                    thread_a_entry, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    // Cria e inicia a Thread B
    k_thread_create(&thread_b_data, thread_b_stack,
                    STACKSIZE,
                    thread_b_entry, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    // Aguarda ambas as threads terminarem (opcional, mas bom para o logging final)
    k_thread_join(&thread_a_data, K_FOREVER);
    k_thread_join(&thread_b_data, K_FOREVER);

    LOG_INF("-------------------------------------------------------");
    LOG_INF("Valor final do contador COMPARTILHADO: %d", contador_compartilhado);
    
    if (contador_compartilhado == valor_esperado) {
        LOG_WRN("AVISO: Condição de corrida não detectada nesta execução. Tente rodar novamente!");
    } else {
        LOG_ERR("ERRO! O contador final (%d) é diferente do esperado (%d).", 
                contador_compartilhado, valor_esperado);
        LOG_ERR("Isto é a **Condição de Corrida** em ação.");
    }
    LOG_INF("Demonstração concluída.");

    return 0;
}
