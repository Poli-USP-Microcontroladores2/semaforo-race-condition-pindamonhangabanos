# PSI-Microcontroladores2-Aula06
Atividade: Resolução de Race Condition com Semáforo

## 🎯 Objetivos da Atividade
Nesta atividade, os alunos deverão:
- Retomar o código gerado por IA em atividade anterior que apresenta **condições de corrida (race conditions)**.
- Trabalhar em **duplas ou trios**, com **avaliação cruzada interna** entre os integrantes do grupo.
- Aplicar **testes estruturados** com pré-condição, etapas de teste e pós-condição.
- Demonstrar como o problema de concorrência foi **identificado e resolvido** com uso de semáforo.

## 🧠 Etapas da Atividade

### **1️⃣ Revisão do Código Anterior**
- Cada integrante do grupo deverá **executar o código do colega** que contém a race condition original.
- Documentar:
  - O comportamento incorreto observado.
  - O momento em que o erro ocorre (condição específica, sequência de eventos, etc.).

### **2️⃣ Planejamento de Testes**
Para cada cenário, descreva **três casos de teste** seguindo o formato abaixo:

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | ... | ... | ... |
| 2 | ... | ... | ... |
| 3 | ... | ... | ... |

### **3️⃣ Correção e Reteste**
- Corrigir o código para **eliminar a race condition**.
- Reexecutar **os mesmos casos de teste** e registrar:
  - As mudanças feitas.
  - O resultado após a correção com evidências (capturas de tela por exemplo).

### **4️⃣ Avaliação Interna (entre colegas do mesmo grupo)**
Cada integrante deverá:
1. Executar o código original do colega conforme os testes planejados.
2. Executar o código corrigido do colega conforme os testes planejados.
3. Conferir se as condições de corrida foram eliminadas.  
4. Registrar uma **avaliação curta** (pode ser no final do README):
   - O que estava errado antes.  
   - O que mudou com a correção.
   - Se o comportamento agora é estável.  

## 📦 Entregáveis

No repositório do grupo, incluir:
1. `README.md` (este arquivo) contendo:
   - Bruno Mora, GUilherme Fernandes, Tiago Hayashi
   - Uso compartilhado da função rand() e Uso compartilhado de uma variável global
   - Casos de teste.
   - Descrição da race condition e da solução.
   - Avaliação de cada colega.
2. Código-fonte organizado (considerando um código original e um corrigido por cada integrante):
   - `codigo_original/`
   - `codigo_corrigido/`
3. Evidências (prints, logs, vídeos curtos, etc.) da execução dos testes.

---

**Repositório:** entregue via GitHub Classroom (um repositório por grupo) e um PDF do markdown final no Moodle.

# Planejamento de Testes
| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | O código está configurado com LOG e Serial | Verificar se a seed da thread é a mesma antes e depois da execução do código através dos LOGs | A seed deve ser diferente na mesma thread caso as funções sejam chamadas ao mesmo tempo |
| 2 | O código agora possui um MUTEX associado a obtenção da seed | Verficar se a seed da tread é a mesma antes e depois da execução do código através dos LOGs | A seed deve ser a mesma que a anterior na thread|

# O que estava errado antes?
  O Microcontrolador compartilha a função rand() com as duas threads e, por conta disso, o valor da seed pode ser alterado no caso de uma interrupção durante
  a chamada dessa função. Isso faz com que a integridade do gerador de funções não seja garantida e faça com que a execução do código não seja previsível.
# O que mudou com a correção?
  Foi aplicado um MUTEX quando a função rand() é chamada, assim, o mesmo não pode ser interrompido quando está em execução. Dessa forma, o valor utilizado pela função é preservado na thread e não afeta a integridade do gerador de funções.
# O comportamento agora está estável?
  Agora, a função não é interrompida no meio e o comportamento do LED no microcontrolador está estável, já que a variável salva não muda ao retornar para a thread após a interrupção.

# Evidências

Teste depois de colocar o MUTEX
<img width="687" height="166" alt="image" src="https://github.com/user-attachments/assets/fd869d30-b8d5-4ca6-845e-41265f320e41" />
