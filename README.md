# Problema 2: A que horas passa o próximo ônibus?

Sistema de monitoramento e registro de horários de ônibus desenvolvido para auxiliar o coletivo de estudantes da **Universidade do Distrito Federal Professor Jorge Amaury Maia Nunes (UnDF)**. O sistema armazena o histórico de chegadas em pontos de ônibus para fornecer dados estatísticos e responder a consultas sobre pontualidade e tempos de espera.

---

## 📌 Visão Geral do Projeto

O objetivo principal é substituir estimativas genéricas por registros reais e auditáveis de chegadas de ônibus. Com esse histórico acumulado, o sistema permite responder a dúvidas como:
- *"O ônibus 136.1 já passou depois das 18h30?"*
- *"Quanto tempo esperou quem estava no ponto desde as 18h30?"*
- *"Uma linha de ônibus atrasa mais em determinados dias da semana?"*

Esses dados fundamentam reclamações coletivas junto à **Ouvidoria do DF** e à **Secretaria de Mobilidade (SEMOB/GDF)**.

---

## 🛠️ Regras de Implementação & Núcleo

Conforme o regulamento da disciplina, o **núcleo do sistema** deve ser totalmente construído pelos alunos (do zero), sem o uso de bibliotecas de estruturas de dados prontas.

- **Linguagem:** C.
- **Permitido no Núcleo:** Structs/tipos próprios, ponteiros, alocação dinâmica (`malloc`/`free`) e vetores simples da linguagem.
- **Proibido no Núcleo:** Funções ou bibliotecas de ordenação e busca prontas (ex: `qsort`, `bsearch`, `std::map`, etc.).
- **Chave de Identificação:** Cada chegada é identificada de forma única pela combinação de `(Data, Horário, Linha)`.

---

## 📋 Operações e Requisitos Mínimos

O sistema processa um arquivo de texto onde cada linha contém um comando no formato padrão:

| Código | Operação | Assinatura / Comando | Resposta Esperada |
| :---: | :--- | :--- | :--- |
| **R1** | Registrar Chegada | `REG data hora linha` | `OK` ou `DUPLICADO` |
| **R2** | Buscar Chegada | `BUSCA data hora linha` | `ACHOU` ou `NAO` |
| **R3** | Próximo (Geral) | `PROX data hora` | `"data hora linha"` ou `NENHUM` |
| **R4** | Próximo da Linha | `PROXL data hora linha` | `"data hora"` ou `NENHUM` |
| **R5** | Chegadas no Intervalo | `ENTRE data1 hora1 data2 hora2` | `k` (quantidade) seguido de `k` linhas ou `0` |
| **R6** | Remover Chegada | `REM data hora linha` | `OK` ou `NAO` |
| **R7** | Altura da Árvore | `ALT` | Número inteiro representando a altura |
| **R8** | Espera Semanal | `SEMANA linha data hora k` | `k` linhas no formato `"data espera"` ou `"data NAOPASSOU"` |

*Observação: Linhas em branco e comentários iniciados por `#` são ignorados no arquivo de entrada.*

---

## 📈 Fases do Desenvolvimento

1. **Fase 1: Árvore Binária de Busca (ABB)**
   - Implementação da estrutura de dados Árvore Binária de Busca construída do zero.
   - Garantia do funcionamento de todas as operações R1 a R8.
   - Remoção de nó com dois filhos utilizando o sucessor (menor elemento da subárvore direita).

2. **Fase 2: Medição do Funcionamento Real**
   - Testes de desempenho com arquivos de carga contendo $N = 1.000$, $5.000$ e $20.000$ registros.
   - Comparação entre dados inseridos *quase em ordem* vs. *embaralhados*.
   - Análise do impacto do desbalanceamento da árvore no tempo de busca e na altura.

3. **Fase 3: Autobalanceamento com Árvore AVL**
   - Substituição da ABB por uma **Árvore AVL** (fator de balanceamento, rotações simples/duplas).
   - Otimização das buscas das operações R4 e R8.
   - Medição comparativa do tempo, memória e altura final garantindo o limite de $h \le 1{,}45 \times \log_2(n+2)$.

---

## ⚙️ Compilação e Execução

O repositório contém o arquivo obrigatório `executar.txt` na raiz, com os comandos de compilação e execução para ambiente Windows/MinGW GCC:

```bash
# Compilação
gcc -O2 main.c -o sistema.exe

# Execução da Fase 1
./sistema.exe {arquivo}

# Execução da Fase 3
./sistema.exe {arquivo}
