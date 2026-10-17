# Problema 2: A que horas passa o próximo ônibus?

Sistema de monitoramento e registro de horários de ônibus desenvolvido para auxiliar o coletivo de estudantes da **Universidade do Distrito Federal Professor Jorge Amaury Maia Nunes (UnDF)**[cite: 1]. O sistema armazena o histórico de chegadas em pontos de ônibus para fornecer dados estatísticos e responder a consultas sobre pontualidade e tempos de espera[cite: 1].

---

## 📌 Visão Geral do Projeto

O objetivo principal é substituir estimativas genéricas por registros reais e auditáveis de chegadas de ônibus[cite: 1]. Com esse histórico acumulado, o sistema permite responder a dúvidas como:
- *"O ônibus 136.1 já passou depois das 18h30?"*[cite: 1]
- *"Quanto tempo esperou quem estava no ponto desde as 18h30?"*[cite: 1]
- *"Uma linha de ônibus atrasa mais em determinados dias da semana?"*[cite: 1]

Esses dados fundamentam reclamações coletivas junto à **Ouvidoria do DF** e à **Secretaria de Mobilidade (SEMOB/GDF)**[cite: 1].

---

## 🛠️ Regras de Implementação & Núcleo

Conforme o regulamento da disciplina, o **núcleo do sistema** deve ser totalmente construído pelos alunos (do zero), sem o uso de bibliotecas de estruturas de dados prontas[cite: 3].

- **Linguagem:** C[cite: 2, 4]
- **Permitido no Núcleo:** Structs/tipos próprios, ponteiros, alocação dinâmica (`malloc`/`free`) e vetores simples da linguagem[cite: 3].
- **Proibido no Núcleo:** Funções ou bibliotecas de ordenação e busca prontas (ex: `qsort`, `bsearch`, `std::map`, etc.)[cite: 3].
- **Chave de Identificação:** Cada chegada é identificada de forma única pela combinação de `(Data, Horário, Linha)`[cite: 1].

---

## 📋 Operações e Requisitos Mínimos

O sistema processa um arquivo de texto onde cada linha contém um comando no formato padrão:

| Código | Operação | Assinatura / Comando | Resposta Esperada |
| :---: | :--- | :--- | :--- |
| **R1** | Registrar Chegada | `REG data hora linha` | `OK` ou `DUPLICADO`[cite: 2] |
| **R2** | Buscar Chegada | `BUSCA data hora linha` | `ACHOU` ou `NAO`[cite: 2] |
| **R3** | Próximo (Geral) | `PROX data hora` | `"data hora linha"` ou `NENHUM`[cite: 2] |
| **R4** | Próximo da Linha | `PROXL data hora linha` | `"data hora"` ou `NENHUM`[cite: 2] |
| **R5** | Chegadas no Intervalo | `ENTRE data1 hora1 data2 hora2` | `k` (quantidade) seguido de `k` linhas ou `0`[cite: 2] |
| **R6** | Remover Chegada | `REM data hora linha` | `OK` ou `NAO`[cite: 2] |
| **R7** | Altura da Árvore | `ALT` | Número inteiro representando a altura[cite: 2] |
| **R8** | Espera Semanal | `SEMANA linha data hora k` | `k` linhas no formato `"data espera"` ou `"data NAOPASSOU"`[cite: 2] |

*Observação: Linhas em branco e comentários iniciados por `#` são ignorados no arquivo de entrada[cite: 2].*

---

## 📈 Fases do Desenvolvimento

1. **Fase 1: Árvore Binária de Busca (ABB)**
   - Implementação da estrutura de dados Árvore Binária de Busca construída do zero[cite: 2].
   - Garantia do funcionamento de todas as operações R1 a R8[cite: 2].
   - Remoção de nó com dois filhos utilizando o sucessor (menor elemento da subárvore direita)[cite: 2].

2. **Fase 2: Medição do Funcionamento Real**
   - Testes de desempenho com arquivos de carga contendo $N = 1.000$, $5.000$ e $20.000$ registros[cite: 2].
   - Comparação entre dados inseridos *quase em ordem* vs. *embaralhados*[cite: 2].
   - Análise do impacto do desbalanceamento da árvore no tempo de busca e na altura[cite: 2].

3. **Fase 3: Autobalanceamento com Árvore AVL**
   - Substituição da ABB por uma **Árvore AVL** (fator de balanceamento, rotações simples/duplas)[cite: 2, 3].
   - Otimização das buscas das operações R4 e R8[cite: 3].
   - Medição comparativa do tempo, memória e altura final garantindo o limite de $h \le 1{,}45 \times \log_2(n+2)$[cite: 3].

---

## ⚙️ Compilação e Execução

O repositório contém o arquivo obrigatório `executar.txt` na raiz, com os comandos de compilação e execução para ambiente Windows/MinGW GCC[cite: 2]:

```bash
# Compilação
gcc -O2 main.c -o sistema.exe

# Execução da Fase 1
./sistema.exe {arquivo}

# Execução da Fase 3
./sistema.exe {arquivo}
