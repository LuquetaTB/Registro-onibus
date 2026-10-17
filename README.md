# Problema 2: A que horas passa o próximo ônibus?

Sistema de monitoramento e registro de horários de ônibus desenvolvido para o coletivo de estudantes da **Universidade do Distrito Federal Professor Jorge Amaury Maia Nunes (UnDF)**. O sistema armazena o histórico real de chegadas informadas nas paradas de ônibus para responder a consultas sobre pontualidade e tempo de espera.

---

## 👥 Integrantes do Projeto

- **Hitallo Miguel**
- **Joaquim Lopes**
- **Lucas Sena**
- **Julio Batista**

---

## 📌 Visão Geral do Projeto

O objetivo principal do sistema é substituir estimativas genéricas por registros reais e auditáveis de chegadas de ônibus na parada da UnDF. Com o histórico acumulado de registros, o sistema responde a perguntas cruciais para a comunidade acadêmica:

- *"O 136.1 já passou depois das 18h30?"*
- *"Quanto tempo esperou quem estava no ponto desde as 18h30?"*
- *"A linha atrasa mais em determinados dias da semana?"*

Os dados gerados fundamentam cobranças coletivas com suporte estatístico junto à **Ouvidoria do DF** e à **Secretaria de Mobilidade (SEMOB/GDF)**.

---

## 📁 Estrutura esperada do projeto

.
├── main.c            # Ponto de entrada, leitura de comandos e medição de tempo
├── arvore.c          # Implementação das funções da árvore (ABB / AVL)
├── arvore.h          # Definição das estruturas (No), protótipos e constantes
├── executar.txt      # Comandos de compilação e execução automática
└── README.md         # Documentação e identificação do projeto

---

## 🛠️ Regras de Implementação & Núcleo

Conforme o regulamento da disciplina, o **núcleo do sistema** é inteiramente desenvolvido pelo grupo do zero, sem o uso de estruturas de dados ou funções de ordenação e busca prontas de bibliotecas externas.

- **Linguagem:** C (versão GCC/MinGW)
- **Permitido no Núcleo:** Structs/tipos próprios, ponteiros, alocação dinâmica (`malloc`/`free`), vetores simples da linguagem e tabelas hash desenvolvidas pelos integrantes em projetos anteriores.
- **Proibido no Núcleo:** Funções e bibliotecas prontas de ordenação ou busca (ex: `qsort`, `bsearch`, `std::map`, etc.).
- **Identificação das Chegadas:** Cada chegada é identificada de forma única pela chave composta por `(Data, Horário, Linha)`.

---

## 📋 Operações e Requisitos Mínimos

O sistema lê um arquivo de operações fornecido via argumento de linha de comando e processa uma instrução por linha. Linhas em branco e comentários iniciados por `#` são ignorados.

| Código | Operação | Comando | Resposta Esperada |
| :---: | :--- | :--- | :--- |
| **R1** | Registrar Chegada | `REG data hora linha` | `OK` ou `DUPLICADO` |
| **R2** | Buscar Chegada | `BUSCA data hora linha` | `ACHOU` ou `NAO` |
| **R3** | Próximo (Geral) | `PROX data hora` | `"data hora linha"` ou `NENHUM` |
| **R4** | Próximo da Linha | `PROXL data hora linha` | `"data hora"` ou `NENHUM` |
| **R5** | Chegadas no Intervalo | `ENTRE data1 hora1 data2 hora2` | `k` (quantidade) seguido de `k` linhas ou `0` |
| **R6** | Remover Chegada | `REM data hora linha` | `OK` ou `NAO` |
| **R7** | Altura da Árvore | `ALT` | Número inteiro com a altura da árvore |
| **R8** | Espera Semanal | `SEMANA linha data hora k` | `k` linhas no formato `"data espera"` ou `"data NAOPASSOU"` |

---

## 📈 Fases do Desenvolvimento

1. **Fase 1: Árvore Binária de Busca (ABB)**
   - Implementação do núcleo com uma Árvore Binária de Busca simples desenvolvida do zero.
   - Suporte completo às operações R1 a R8.
   - Remoção de nó com dois filhos utilizando o menor nó da subárvore direita (sucessor).

2. **Fase 2: Medição do Funcionamento Real**
   - Testes de desempenho utilizando arquivos de carga com $N = 1.000$, $5.000$ e $20.000$ registros.
   - Avaliação comparativa entre inserções *quase em ordem* vs. *embaralhadas*.
   - Medição da mediana de tempo de execução e da altura acumulada da árvore.

3. **Fase 3: Autobalanceamento com Árvore AVL**
   - Substituição do núcleo por uma **Árvore AVL** (fator de balanceamento, rotações simples e duplas).
   - Otimização das consultas específicas para R4 e R8.
   - Garantia da altura máxima limitada por $h \le 1{,}45 \times \log_2(n+2)$.

---

## ⚙️ Compilação e Execução (`executar.txt`)

O arquivo `executar.txt` localizado na raiz do repositório contém as instruções padronizadas para compilação e execução no ambiente Windows:

```text
gcc -O2 main.c -o sistema.exe
sistema.exe {arquivo}
sistema.exe {arquivo}
