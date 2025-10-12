# Atividade - Prioridades

Faça um 'clone' do repositório criado pelo git classroom, trabalhe nele no seu computador e faça um branch e push quando solicitado. As respostas das perguntas devem ser enviadas no Moodle. Não esqueça de configurar o [link do repositório](https://frdm-kl25z-com-zephyr-rtos.super.site/tutoriais/git-e-github-controle-de-verses#block-213a7f4c2a2b8004802ff3b239027a61) antes de fazer o 'push'.

## 🔹 Perguntas:

1. Utilize o [Systick](https://frdm-kl25z-com-zephyr-rtos.super.site/systick)  para medir o tempo de cada thread e faça um diagrama temporal da execução do código fornecido para diferentes casos de prioridade e tempos (`k_sleep()`) e explique cada caso.
    
    Dica: utilize [Frequência da CPU](https://frdm-kl25z-com-zephyr-rtos.super.site/tutoriais/frequncia-da-cpu) para calcular o tempo, se preciso. 

- Crie um 'branch' e faça um 'push' com o código que calcula o tempo das threads e mostra um caso diferente de prioridades do fornecido.
    
2. O que acontece se removermos o `k_sleep()` de uma das threads, explique? Se as threads têm prioridades iguais, qual será executada primeiro? Como funciona threads cooperativas (prioridade -1), explique? 
3. É possível que uma thread nunca seja executada? Em qual combinação de prioridade/tempo isso acontece? Como é conhecido isso? 
4. O que acontece com um a interrupção de hardware (ISR)? Implemente um botão ([Botão com Interrupção](https://frdm-kl25z-com-zephyr-rtos.super.site/tutoriais/boto-com-pull-up-zephyr-42#block-286a7f4c2a2b80ee9714e7bad68b7822)) com interrupção para mostrar o comportamento e prioridade da ISR.

- Crie um 'branch' e faça um 'push' com o código que crie um interrupção de hardware utilizando um botão. 

5. Substitua os comandos de printk por comandos com Logging. Inclua mais informações (como os tempos intermediários do timer) que podem ser utilizados para depuração do código. Discuta a diferença entre usar o Logging, printk, printf e debug.

- Crie um 'branch' e faça um 'push' com o código com o Logging. 
