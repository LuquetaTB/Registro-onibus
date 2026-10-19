# Problema 2: A que horas passa o próximo ônibus?

Sistema de monitoramento e registro de chegadas de ônibus para o coletivo de estudantes da **Universidade do Distrito Federal Professor Jorge Amaury Maia Nunes (UnDF)**. Processa logs de horários em paradas de ônibus para responder a consultas de pontualidade e tempos de espera.

## Integrantes

- Hitallo Miguel
- Joaquim Lopes
- Lucas Sena
- Julio Batista

---

## O Sistema

O sistema substitui estimativas informais por registros auditáveis baseados no histórico real de chegadas[cite: 1], permitindo responder a consultas como:
- O ônibus de determinada linha já passou após um horário específico?
- Qual foi o tempo de espera real para quem aguardava no ponto?
- A linha apresenta atrasos recorrentes em dias específicos da semana?

Os dados gerados fornecem embasamento estatístico para solicitações junto à Ouvidoria do DF e à Secretaria de Mobilidade (SEMOB/GDF).

---

## Especificações Técnicas & Núcleo

Conforme os requisitos do projeto, o **núcleo** foi implementado inteiramente do zero, sem o uso de estruturas de dados ou bibliotecas prontas de ordenação e busca.

- **Linguagem:** C (GCC / MinGW)
- **Permitido no núcleo:** Alocação dinâmica (`malloc`/`free`), ponteiros, tipos próprios (`struct`) e vetores simples.
- **Proibido no núcleo:** Funções prontas de busca ou ordenação (ex: `qsort`, `bsearch`, `std::map`, etc.).
- **Identificação:** Cada registro de chegada é unicamente mapeado pela chave composta por `(Data, Horário, Linha)`.

---

## Operações Suportadas

O programa lê um arquivo de texto de entrada contendo uma instrução por linha (linhas em branco e comentários iniciados por `#` são ignorados):

| Operação | Comando | Descrição / Retorno |
| :--- | :--- | :--- |
| **R1** | `REG data hora linha` | Registra uma chegada (`OK` ou `DUPLICADO`) |
| **R2** | `BUSCA data hora linha` | Verifica se a chegada está registrada (`ACHOU` ou `NAO`) |
| **R3** | `PROX data hora` | Retorna o próximo registro geral a partir do instante (`"data hora linha"` ou `NENHUM`)|
| **R4** | `PROXL data hora linha` | Retorna a próxima chegada da linha específica (`"data hora"` ou `NENHUM`) |
| **R5** | `ENTRE data1 hora1 data2 hora2` | Retorna a quantidade $k$ e os registros compreendidos no intervalo inclusivo |
| **R6** | `REM data hora linha` | Remove uma chegada informada incorretamente (`OK` ou `NAO`) |
| **R7** | `ALT` | Retorna a altura atual da árvore estrutural |
| **R8** | `SEMANA linha data hora k` | Analisa o comportamento e a espera da linha em até $k$ semanas anteriores |

---

## Fases do Projeto

1. **Fase 1 (ABB):** Implementação inicial utilizando uma Árvore Binária de Busca construída do zero, suportando todas as operações (R1–R8).
2. **Fase 2 (Análise):** Bateria de testes de desempenho com arquivos de carga de $1.000$ a $20.000$ registros, medindo tempos de execução e o comportamento da altura da árvore.
3. **Fase 3 (AVL):** Substituição da estrutura principal por uma Árvore AVL auto-balanceada com rotações, otimizando as consultas e garantindo o limite estrito de altura $h \le 1{,}45 \times \log_2(n+2)$.

---

## Compilação e Execução

O repositório inclui o arquivo `executar.txt` na raiz, configurado para automação dos testes no Windows:

```text
gcc -O2 main.c -o sistema.exe
sistema.exe {arquivo}
sistema.exe {arquivo}
