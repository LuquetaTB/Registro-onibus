										/*
 * ============================================================================
 * Universidade do Distrito Federal Professor Jorge Amaury Maia Nunes (UnDF)
 * Problema 2: A que horas passa o próximo ônibus?
 * Fase 1: Árvore Binária de Busca (ABB)
 *
 * Integrantes:
 * - Hitallo Miguel
 * - Joaquim Lopes
 * - Lucas Sena
 * - Julio Batista
 *
 * ----------------------------------------------------------------------------
 * REGRAS DO NÚCLEO E REQUISITOS ATENDIDOS:
 * - O núcleo do sistema é a árvore binária de busca construída pelo grupo do zero.
 * - Proibido no núcleo qualquer biblioteca de estruturas ou funções prontas
 *   de ordenação/busca (sem qsort, sem bsearch, sem maps/sets prontos).
 * - O vetor simples da linguagem e tipos próprios (struct) são permitidos.
 * - Conversão de data/horário usa a biblioteca padrão da linguagem (<time.h>).
 * - Ao remover nó com 2 filhos, substituição pelo sucessor (menor nó da subárvore direita).
 * - Medição de tempo computa estritamente a execução das operações.
 * - Formato de saída:
 *     stdout -> Respostas exatas das operações conforme enunciado.
 *     stderr -> TEMPO_MS valor (em milissegundos com 3 casas decimais).
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* ============================================================================
 * ESTRUTURA DO NÓ DA ÁRVORE BINÁRIA DE BUSCA (NÚCLEO)
 * ============================================================================
 * Cada chegada informada possui:
 * - data: formato AAAA-MM-DD (10 caracteres + '\0')
 * - horario: formato HH:MM:SS (8 caracteres + '\0')
 * - linha: até 8 caracteres alfanuméricos sem espaços + '\0'
 *
 * Na Fase 1, cada nó possui ponteiros para os filhos esquerdo e direito.
 * Na Fase 3 (AVL), esta mesma estrutura será estendida com o campo de altura
 * (ex: int altura) para permitir o cálculo do fator de balanceamento e rotações.
 */
typedef struct No {
    char data[12];       // AAAA-MM-DD
    char horario[10];    // HH:MM:SS
    char linha[10];      // código da linha (até 8 chars)
    struct No *esq;      // subárvore esquerda (< chave)
    struct No *dir;      // subárvore direita (> chave)
} No;

/* ============================================================================
 * FUNÇÕES DE COMPARAÇÃO DE CHAVE (NÚCLEO)
 * ============================================================================
 * Critério de ordenação estabelecido pelo enunciado:
 * 1. Data e horário cronológicos (instante).
 * 2. Em caso de empate no instante, desempate pelo código de linha em ordem
 *    lexicográfica (caractere a caractere pelo código ASCII).
 */

// Compara dois instantes (apenas data e horário).
// Retorna < 0 se i1 < i2, > 0 se i1 > i2, ou 0 se iguais.
int comparar_instante(const char *d1, const char *h1, const char *d2, const char *h2) {
    int cmp_data = strcmp(d1, d2);
    if (cmp_data != 0) {
        return cmp_data;
    }
    return strcmp(h1, h2);
}

// Compara a chave completa de duas chegadas: (data, horário, linha).
int comparar_chegadas(const char *d1, const char *h1, const char *l1,
                      const char *d2, const char *h2, const char *l2) {
    int cmp_instante = comparar_instante(d1, h1, d2, h2);
    if (cmp_instante != 0) {
        return cmp_instante;
    }
    return strcmp(l1, l2);
}

/* ============================================================================
 * FUNÇÕES AUXILIARES DE DATA E HORA
 * ============================================================================
 * Conforme o enunciado: "Para converter data e horário, vocês podem usar a
 * biblioteca de datas da linguagem escolhida, inclusive no núcleo do código."
 */

// Converte um horário "HH:MM:SS" para o total de segundos desde o início do dia.
long horario_para_segundos(const char *horario) {
    int h = 0, m = 0, s = 0;
    sscanf(horario, "%d:%d:%d", &h, &m, &s);
    return (long)h * 3600 + (long)m * 60 + (long)s;
}

// Calcula a data ocorrida 'semanas_atras' antes de 'data_base', no formato AAAA-MM-DD.
// Usa struct tm e mktime para garantir tratamento correto de anos bissextos e viradas de mês/ano.
void calcular_data_offset_semanas(const char *data_base, int semanas_atras, char *data_saida) {
    int ano = 0, mes = 0, dia = 0;
    sscanf(data_base, "%d-%d-%d", &ano, &mes, &dia);

    struct tm t;
    memset(&t, 0, sizeof(struct tm));
    t.tm_year = ano - 1900;
    t.tm_mon = mes - 1;
    t.tm_mday = dia - (7 * semanas_atras);
    t.tm_hour = 12; // horário fixo ao meio-dia para neutralizar variações de fuso/DST
    t.tm_isdst = -1;

    mktime(&t); // normaliza dia, mês e ano automaticamente

    sprintf(data_saida, "%04d-%02d-%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
}

/* ============================================================================
 * BUFFER DE SAÍDA (INFRAESTRUTURA FORA DO NÚCLEO)
 * ============================================================================
 * O enunciado exige: "O programa lê o arquivo inteiro antes de executar as
 * operações e só escreve as respostas no final, na saída padrão... Ele mede o
 * tempo gasto apenas com as operações".
 * Para evitar I/O no console durante a medição de tempo, as respostas são
 * acumuladas em memória em um buffer dinâmico e enviadas à saída padrão ao final.
 */
typedef struct {
    char *dados;
    size_t tamanho;
    size_t capacidade;
} BufferSaida;

void buffer_inicializar(BufferSaida *b) {
    b->capacidade = 1024 * 1024; // 1 MB inicial
    b->dados = (char *)malloc(b->capacidade);
    b->tamanho = 0;
    if (b->dados != NULL) {
        b->dados[0] = '\0';
    }
}

void buffer_acrescentar_str(BufferSaida *b, const char *s) {
    size_t len = strlen(s);
    while (b->tamanho + len + 1 >= b->capacidade) {
        b->capacidade *= 2;
        b->dados = (char *)realloc(b->dados, b->capacidade);
    }
    memcpy(b->dados + b->tamanho, s, len);
    b->tamanho += len;
    b->dados[b->tamanho] = '\0';
}

void buffer_acrescentar_formato(BufferSaida *b, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    va_list args_copia;
    va_copy(args_copia, args);
    int necessarios = vsnprintf(NULL, 0, fmt, args_copia);
    va_end(args_copia);

    if (necessarios > 0) {
        while (b->tamanho + (size_t)necessarios + 1 >= b->capacidade) {
            b->capacidade *= 2;
            b->dados = (char *)realloc(b->dados, b->capacidade);
        }
        vsnprintf(b->dados + b->tamanho, (size_t)necessarios + 1, fmt, args);
        b->tamanho += (size_t)necessarios;
    }
    va_end(args);
}

void buffer_destruir(BufferSaida *b) {
    if (b->dados != NULL) {
        free(b->dados);
        b->dados = NULL;
    }
    b->tamanho = 0;
    b->capacidade = 0;
}

/* ============================================================================
 * IMPLEMENTAÇÃO DOS REQUISITOS (R1 A R8) - NÚCLEO
 * ============================================================================
 */

/*
 * ----------------------------------------------------------------------------
 * R1: registrar_chegada(data, horario, linha)
 * ----------------------------------------------------------------------------
 * Insere um novo registro de chegada na Árvore Binária de Busca.
 * Implementação iterativa para garantir que não haja estouro de pilha
 * (stack overflow) mesmo quando arquivos com 20.000 chegadas quase em ordem
 * forem processados na Fase 2.
 *
 * Retorno:
 *   1 -> Inserção bem-sucedida ("OK")
 *   0 -> Registro já existente com a mesma chave composta ("DUPLICADO")
 */
int registrar_chegada(No **raiz, const char *data, const char *horario, const char *linha) {
    // Caso 1: Árvore inicialmente vazia
    if (*raiz == NULL) {
        No *novo = (No *)malloc(sizeof(No));
        strcpy(novo->data, data);
        strcpy(novo->horario, horario);
        strcpy(novo->linha, linha);
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1; // OK
    }

    // Caso 2: Busca pelo ponto de inserção de forma iterativa
    No *atual = *raiz;
    while (1) {
        int cmp = comparar_chegadas(data, horario, linha, atual->data, atual->horario, atual->linha);
        if (cmp < 0) {
            if (atual->esq == NULL) {
                No *novo = (No *)malloc(sizeof(No));
                strcpy(novo->data, data);
                strcpy(novo->horario, horario);
                strcpy(novo->linha, linha);
                novo->esq = NULL;
                novo->dir = NULL;
                atual->esq = novo;
                return 1; // OK
            }
            atual = atual->esq;
        } else if (cmp > 0) {
            if (atual->dir == NULL) {
                No *novo = (No *)malloc(sizeof(No));
                strcpy(novo->data, data);
                strcpy(novo->horario, horario);
                strcpy(novo->linha, linha);
                novo->esq = NULL;
                novo->dir = NULL;
                atual->dir = novo;
                return 1; // OK
            }
            atual = atual->dir;
        } else {
            // Chave idêntica (data, horário e linha já registrados)
            return 0; // DUPLICADO
        }
    }
}

/*
 * ----------------------------------------------------------------------------
 * R2: buscar_chegada(data, horario, linha)
 * ----------------------------------------------------------------------------
 * Verifica se uma chegada exata está registrada na árvore.
 *
 * Retorno:
 *   1 -> Chegada cadastrada ("ACHOU")
 *   0 -> Chegada não encontrada ("NAO")
 */
int buscar_chegada(const No *raiz, const char *data, const char *horario, const char *linha) {
    const No *atual = raiz;
    while (atual != NULL) {
        int cmp = comparar_chegadas(data, horario, linha, atual->data, atual->horario, atual->linha);
        if (cmp == 0) {
            return 1; // ACHOU
        } else if (cmp < 0) {
            atual = atual->esq;
        } else {
            atual = atual->dir;
        }
    }
    return 0; // NAO
}

/*
 * ----------------------------------------------------------------------------
 * R3: proximo(data, horario)
 * ----------------------------------------------------------------------------
 * Retorna a primeira chegada registrada a partir daquele instante, de qualquer
 * linha. Havendo chegadas no mesmo instante, retorna a de menor linha lexicográfica.
 *
 * Na ABB, a primeira chegada >= (data, horario) equivale a buscar o menor nó
 * cuja chave seja >= (data, horario, "").
 * Como qualquer linha válida possui comprimento >= 1, (data, horario, "") é
 * estritamente menor que qualquer chegada no instante (data, horario).
 *
 * Retorno:
 *   Ponteiro para o nó correspondente ou NULL se não houver chegadas a partir dele.
 */
const No* proximo_chegada(const No *raiz, const char *data, const char *horario) {
    const No *candidato = NULL;
    const No *atual = raiz;

    while (atual != NULL) {
        // Compara com a chave base (data, horario, "")
        int cmp = comparar_chegadas(atual->data, atual->horario, atual->linha, data, horario, "");
        if (cmp >= 0) {
            // O nó atual ocorreu a partir do instante; é um candidato válido.
            // Tentamos encontrar um instante ainda menor na subárvore esquerda.
            candidato = atual;
            atual = atual->esq;
        } else {
            // O nó atual ocorreu estritamente antes do instante; buscamos à direita.
            atual = atual->dir;
        }
    }
    return candidato;
}

/*
 * ----------------------------------------------------------------------------
 * R4: proximo_da_linha(data, horario, linha)
 * ----------------------------------------------------------------------------
 * Retorna a primeira chegada da linha a partir daquele instante.
 * Conforme o enunciado para a Fase 1:
 * "Para R4 e R8, nesta fase, basta a solução mais simples que funcione sobre
 * a própria árvore."
 *
 * A solução percorre a árvore em ordem simétrica (in-order), podando qualquer
 * subárvore que seja estritamente anterior ao instante pesquisado. O primeiro
 * nó encontrado com a linha desejada é garantidamente o mais cedo.
 *
 * Retorno:
 *   1 -> Encontrou (preenche res_data e res_horario)
 *   0 -> Não há registro daquela linha a partir do instante
 */
int proximo_da_linha_rec(const No *raiz, const char *data, const char *horario, const char *linha,
                         char *res_data, char *res_horario) {
    if (raiz == NULL) {
        return 0;
    }

    // Se o nó atual é estritamente anterior ao instante buscado,
    // toda a subárvore esquerda também é anterior, logo podemos podá-la.
    if (comparar_instante(raiz->data, raiz->horario, data, horario) < 0) {
        return proximo_da_linha_rec(raiz->dir, data, horario, linha, res_data, res_horario);
    }

    // 1. Tenta encontrar primeiro na subárvore esquerda (instantes menores)
    if (proximo_da_linha_rec(raiz->esq, data, horario, linha, res_data, res_horario)) {
        return 1;
    }

    // 2. Verifica o nó atual (está garantidamente no instante ou depois dele)
    if (strcmp(raiz->linha, linha) == 0) {
        strcpy(res_data, raiz->data);
        strcpy(res_horario, raiz->horario);
        return 1;
    }

    // 3. Tenta encontrar na subárvore direita
    return proximo_da_linha_rec(raiz->dir, data, horario, linha, res_data, res_horario);
}

int proximo_da_linha(const No *raiz, const char *data, const char *horario, const char *linha,
                     char *res_data, char *res_horario) {
    return proximo_da_linha_rec(raiz, data, horario, linha, res_data, res_horario);
}

/*
 * ----------------------------------------------------------------------------
 * R5: entre(data1, horario1, data2, horario2)
 * ----------------------------------------------------------------------------
 * Retorna todas as chegadas registradas entre os dois instantes, inclusive,
 * em ordem cronológica (e por código de linha em caso de empate).
 * Formato de resposta: quantidade k, seguida por k linhas com "data hora linha".
 * Se o primeiro instante for posterior ao segundo, responde 0.
 *
 * Utiliza busca por faixa (range search) podando ramos da ABB fora do intervalo.
 */
void contar_chegadas_entre(const No *raiz, const char *d1, const char *h1,
                           const char *d2, const char *h2, int *total) {
    if (raiz == NULL) return;

    int cmp1 = comparar_instante(raiz->data, raiz->horario, d1, h1);
    int cmp2 = comparar_instante(raiz->data, raiz->horario, d2, h2);

    // Se a raiz >= instante1, podem existir nós válidos à esquerda
    if (cmp1 >= 0) {
        contar_chegadas_entre(raiz->esq, d1, h1, d2, h2, total);
    }
    // Se instante1 <= raiz <= instante2, nó está contido no intervalo inclusivo
    if (cmp1 >= 0 && cmp2 <= 0) {
        (*total)++;
    }
    // Se a raiz <= instante2, podem existir nós válidos à direita
    if (cmp2 <= 0) {
        contar_chegadas_entre(raiz->dir, d1, h1, d2, h2, total);
    }
}

void coletar_chegadas_entre(const No *raiz, const char *d1, const char *h1,
                            const char *d2, const char *h2, BufferSaida *buf) {
    if (raiz == NULL) return;

    int cmp1 = comparar_instante(raiz->data, raiz->horario, d1, h1);
    int cmp2 = comparar_instante(raiz->data, raiz->horario, d2, h2);

    if (cmp1 >= 0) {
        coletar_chegadas_entre(raiz->esq, d1, h1, d2, h2, buf);
    }
    if (cmp1 >= 0 && cmp2 <= 0) {
        buffer_acrescentar_formato(buf, "%s %s %s\n", raiz->data, raiz->horario, raiz->linha);
    }
    if (cmp2 <= 0) {
        coletar_chegadas_entre(raiz->dir, d1, h1, d2, h2, buf);
    }
}

void executar_entre(const No *raiz, const char *d1, const char *h1,
                    const char *d2, const char *h2, BufferSaida *buf) {
    // Se o primeiro instante for posterior ao segundo, responde 0
    if (comparar_instante(d1, h1, d2, h2) > 0) {
        buffer_acrescentar_str(buf, "0\n");
        return;
    }

    int total = 0;
    contar_chegadas_entre(raiz, d1, h1, d2, h2, &total);

    buffer_acrescentar_formato(buf, "%d\n", total);
    if (total > 0) {
        coletar_chegadas_entre(raiz, d1, h1, d2, h2, buf);
    }
}

/*
 * ----------------------------------------------------------------------------
 * R6: remover_chegada(data, horario, linha)
 * ----------------------------------------------------------------------------
 * Remove uma chegada registrada.
 * Exigência obrigatória da Fase 1:
 * "Ao remover um nó que tem dois filhos, substituam-no pelo sucessor, isto é,
 * pelo menor nó da subárvore da direita."
 *
 * Retorno:
 *   1 -> Remoção com sucesso ("OK")
 *   0 -> Não encontrada na árvore ("NAO")
 */
No* obter_menor_no(No *nodo) {
    No *atual = nodo;
    while (atual != NULL && atual->esq != NULL) {
        atual = atual->esq;
    }
    return atual;
}

int remover_chegada(No **raiz, const char *data, const char *horario, const char *linha) {
    if (*raiz == NULL) {
        return 0; // NAO encontrada
    }

    int cmp = comparar_chegadas(data, horario, linha, (*raiz)->data, (*raiz)->horario, (*raiz)->linha);
    if (cmp < 0) {
        return remover_chegada(&((*raiz)->esq), data, horario, linha);
    } else if (cmp > 0) {
        return remover_chegada(&((*raiz)->dir), data, horario, linha);
    } else {
        // Encontrou o nó a ser removido
        No *alvo = *raiz;

        // Caso 1: Nó sem filho esquerdo
        if ((*raiz)->esq == NULL) {
            *raiz = (*raiz)->dir;
            free(alvo);
        }
        // Caso 2: Nó sem filho direito
        else if ((*raiz)->dir == NULL) {
            *raiz = (*raiz)->esq;
            free(alvo);
        }
        // Caso 3: Nó com dois filhos
        // Conforme enunciado: substituição pelo menor nó da subárvore da direita (sucessor)
        else {
            No *sucessor = obter_menor_no((*raiz)->dir);

            // Copia os dados do sucessor para o nó corrente
            strcpy((*raiz)->data, sucessor->data);
            strcpy((*raiz)->horario, sucessor->horario);
            strcpy((*raiz)->linha, sucessor->linha);

            // Remove o nó sucessor da subárvore direita
            remover_chegada(&((*raiz)->dir), sucessor->data, sucessor->horario, sucessor->linha);
        }
        return 1; // OK
    }
}

/*
 * ----------------------------------------------------------------------------
 * R7: altura()
 * ----------------------------------------------------------------------------
 * Retorna a altura da árvore que guarda as chegadas.
 * Conforme o enunciado:
 * "A árvore vazia tem altura 0, e uma árvore com um único nó tem altura 1."
 */
int altura(const No *raiz) {
    if (raiz == NULL) {
        return 0;
    }
    int h_esq = altura(raiz->esq);
    int h_dir = altura(raiz->dir);
    return 1 + (h_esq > h_dir ? h_esq : h_dir);
}

/*
 * ----------------------------------------------------------------------------
 * R8: espera_semanal(linha, data, horario, k)
 * ----------------------------------------------------------------------------
 * Para a data informada e para as semanas anteriores, até completar k semanas,
 * sempre no mesmo dia da semana e no mesmo horário, calcula a espera em segundos
 * ou reporta "NAOPASSOU".
 * Responde k linhas, da mais antiga até a data informada.
 */
void executar_espera_semanal(const No *raiz, const char *linha, const char *data,
                             const char *horario, int k, BufferSaida *buf) {
    if (k <= 0) {
        return;
    }

    long seg_referencia = horario_para_segundos(horario);

    // Percorre das k-1 semanas anteriores até a semana 0 (data informada)
    // Garantindo a ordem: "da mais antiga até a data informada"
    for (int sem = k - 1; sem >= 0; sem--) {
        char data_semana[12];
        calcular_data_offset_semanas(data, sem, data_semana);

        char achou_data[12];
        char achou_horario[10];

        // Busca a primeira chegada da linha a partir de (data_semana, horario)
        int achou = proximo_da_linha(raiz, data_semana, horario, linha, achou_data, achou_horario);

        // Deve ter passado no mesmo dia
        if (achou && strcmp(achou_data, data_semana) == 0) {
            long seg_chegada = horario_para_segundos(achou_horario);
            long espera = seg_chegada - seg_referencia;
            if (espera < 0) espera = 0;
            buffer_acrescentar_formato(buf, "%s %ld\n", data_semana, espera);
        } else {
            buffer_acrescentar_formato(buf, "%s NAOPASSOU\n", data_semana);
        }
    }
}

/*
 * ----------------------------------------------------------------------------
 * LIBERAÇÃO DE MEMÓRIA DA ÁRVORE (NÚCLEO)
 * ----------------------------------------------------------------------------
 * Desaloca toda a árvore ao término da execução de forma segura contra
 * estouros de pilha através de rotações de linearização (Right Vine Flattening).
 */
void liberar_arvore(No *raiz) {
    while (raiz != NULL) {
        if (raiz->esq != NULL) {
            No *esq = raiz->esq;
            raiz->esq = esq->dir;
            esq->dir = raiz;
            raiz = esq;
        } else {
            No *prox = raiz->dir;
            free(raiz);
            raiz = prox;
        }
    }
}

/* ============================================================================
 * LEITURA COMPLETA DO ARQUIVO ANTES DA EXECUÇÃO
 * ============================================================================
 * Carrega todas as linhas válidas do arquivo de operações para a memória.
 */
typedef struct {
    char **linhas;
    size_t total;
    size_t capacidade;
    char *conteudo_bruto;
} ListaLinhas;

int carregar_arquivo_em_memoria(const char *caminho, ListaLinhas *lista) {
    FILE *f = fopen(caminho, "rb");
    if (!f) {
        return 0;
    }

    fseek(f, 0, SEEK_END);
    long tam = ftell(f);
    fseek(f, 0, SEEK_SET);

    lista->conteudo_bruto = (char *)malloc((size_t)tam + 2);
    if (!lista->conteudo_bruto) {
        fclose(f);
        return 0;
    }

    size_t lidos = fread(lista->conteudo_bruto, 1, (size_t)tam, f);
    lista->conteudo_bruto[lidos] = '\0';
    fclose(f);

    lista->capacidade = 1024;
    lista->total = 0;
    lista->linhas = (char **)malloc(lista->capacidade * sizeof(char *));

    char *p = lista->conteudo_bruto;
    while (*p != '\0') {
        // Pula quebras de linha anteriores
        while (*p == '\r' || *p == '\n') {
            p++;
        }
        if (*p == '\0') break;

        char *inicio_linha = p;
        while (*p != '\0' && *p != '\r' && *p != '\n') {
            p++;
        }
        if (*p != '\0') {
            *p = '\0';
            p++;
        }

        // Armazena a linha na lista
        if (lista->total >= lista->capacidade) {
            lista->capacidade *= 2;
            lista->linhas = (char **)realloc(lista->linhas, lista->capacidade * sizeof(char *));
        }
        lista->linhas[lista->total++] = inicio_linha;
    }

    return 1;
}

void liberar_lista_linhas(ListaLinhas *lista) {
    if (lista->linhas) free(lista->linhas);
    if (lista->conteudo_bruto) free(lista->conteudo_bruto);
    lista->linhas = NULL;
    lista->conteudo_bruto = NULL;
    lista->total = 0;
    lista->capacidade = 0;
}

/* ============================================================================
 * FUNÇÃO MAIN - FLUXO DE EXECUÇÃO E DESPACHO DAS FUNÇÕES
 * ============================================================================
 */
int main(int argc, char *argv[]) {
    // 1. Verificação dos argumentos de linha de comando
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_operacoes>\n", argv[0]);
        return 1;
    }

    const char *caminho_arquivo = argv[1];

    // 2. Leitura completa do arquivo de operações para a memória ANTES do timer
    ListaLinhas operacoes;
    if (!carregar_arquivo_em_memoria(caminho_arquivo, &operacoes)) {
        fprintf(stderr, "Erro ao abrir ou ler o arquivo: %s\n", caminho_arquivo);
        return 1;
    }

    // Inicialização da raiz da Árvore Binária de Busca e do buffer de saída
    No *raiz = NULL;
    BufferSaida saida;
    buffer_inicializar(&saida);

    // 3. Início da medição de tempo de alta precisão (exclusivo para as operações)
#ifdef _WIN32
    LARGE_INTEGER freq, t_inicio, t_fim;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t_inicio);
#else
    clock_t c_inicio = clock();
#endif

    // 4. Execução sequencial de cada operação do arquivo
    for (size_t i = 0; i < operacoes.total; i++) {
        char *linha_atual = operacoes.linhas[i];

        // Linhas em branco ou comentários iniciados por '#' são estritamente ignorados
        char *ptr = linha_atual;
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == '\0' || *ptr == '#') {
            continue;
        }

        char operacao[32];
        if (sscanf(ptr, "%31s", operacao) != 1) {
            continue;
        }

        /*
         * ====================================================================
         * DESPACHO R1: REG data hora linha
         * ====================================================================
         * Função aplicada: registrar_chegada(&raiz, data, hora, linha_onibus)
         * Retorno esperado: OK ou DUPLICADO
         * Se a chegada já tiver sido registrada, imprime DUPLICADO; senão insere e OK.
         */
        if (strcmp(operacao, "REG") == 0) {
            char data[16], hora[16], linha_onibus[16];
            if (sscanf(ptr, "%*s %15s %15s %15s", data, hora, linha_onibus) == 3) {
                int res = registrar_chegada(&raiz, data, hora, linha_onibus);
                if (res == 1) {
                    buffer_acrescentar_str(&saida, "OK\n");
                } else {
                    buffer_acrescentar_str(&saida, "DUPLICADO\n");
                }
            }
        }

        /*
         * ====================================================================
         * DESPACHO R2: BUSCA data hora linha
         * ====================================================================
         * Função aplicada: buscar_chegada(raiz, data, hora, linha_onibus)
         * Retorno esperado: ACHOU ou NAO
         * Realiza a pesquisa binária exata pela chave completa na ABB.
         */
        else if (strcmp(operacao, "BUSCA") == 0) {
            char data[16], hora[16], linha_onibus[16];
            if (sscanf(ptr, "%*s %15s %15s %15s", data, hora, linha_onibus) == 3) {
                int res = buscar_chegada(raiz, data, hora, linha_onibus);
                if (res == 1) {
                    buffer_acrescentar_str(&saida, "ACHOU\n");
                } else {
                    buffer_acrescentar_str(&saida, "NAO\n");
                }
            }
        }

        /*
         * ====================================================================
         * DESPACHO R3: PROX data hora
         * ====================================================================
         * Função aplicada: proximo_chegada(raiz, data, hora)
         * Retorno esperado: "data hora linha" ou NENHUM
         * Encontra a primeira chegada registrada a partir do instante, de qualquer linha.
         */
        else if (strcmp(operacao, "PROX") == 0) {
            char data[16], hora[16];
            if (sscanf(ptr, "%*s %15s %15s", data, hora) == 2) {
                const No *res = proximo_chegada(raiz, data, hora);
                if (res != NULL) {
                    buffer_acrescentar_formato(&saida, "%s %s %s\n", res->data, res->horario, res->linha);
                } else {
                    buffer_acrescentar_str(&saida, "NENHUM\n");
                }
            }
        }

        /*
         * ====================================================================
         * DESPACHO R4: PROXL data hora linha
         * ====================================================================
         * Função aplicada: proximo_da_linha(raiz, data, hora, linha_onibus, ...)
         * Retorno esperado: "data hora" ou NENHUM
         * Encontra a primeira chegada da linha específica a partir do instante.
         * Na Fase 1, opera diretamente na ABB através de travessia in-order podada.
         */
        else if (strcmp(operacao, "PROXL") == 0) {
            char data[16], hora[16], linha_onibus[16];
            if (sscanf(ptr, "%*s %15s %15s %15s", data, hora, linha_onibus) == 3) {
                char res_data[16], res_hora[16];
                int achou = proximo_da_linha(raiz, data, hora, linha_onibus, res_data, res_hora);
                if (achou) {
                    buffer_acrescentar_formato(&saida, "%s %s\n", res_data, res_hora);
                } else {
                    buffer_acrescentar_str(&saida, "NENHUM\n");
                }
            }
        }

        /*
         * ====================================================================
         * DESPACHO R5: ENTRE data1 hora1 data2 hora2
         * ====================================================================
         * Função aplicada: executar_entre(raiz, d1, h1, d2, h2, &saida)
         * Retorno esperado: quantidade k seguida por k linhas com "data hora linha".
         * Se instante1 for posterior a instante2, responde 0.
         */
        else if (strcmp(operacao, "ENTRE") == 0) {
            char d1[16], h1[16], d2[16], h2[16];
            if (sscanf(ptr, "%*s %15s %15s %15s %15s", d1, h1, d2, h2) == 4) {
                executar_entre(raiz, d1, h1, d2, h2, &saida);
            }
        }

        /*
         * ====================================================================
         * DESPACHO R6: REM data hora linha
         * ====================================================================
         * Função aplicada: remover_chegada(&raiz, data, hora, linha_onibus)
         * Retorno esperado: OK ou NAO
         * Remove o nó especificado. Em nós com 2 filhos, aplica a substituição
         * obrigatória pelo sucessor (menor da subárvore direita).
         */
        else if (strcmp(operacao, "REM") == 0) {
            char data[16], hora[16], linha_onibus[16];
            if (sscanf(ptr, "%*s %15s %15s %15s", data, hora, linha_onibus) == 3) {
                int res = remover_chegada(&raiz, data, hora, linha_onibus);
                if (res == 1) {
                    buffer_acrescentar_str(&saida, "OK\n");
                } else {
                    buffer_acrescentar_str(&saida, "NAO\n");
                }
            }
        }

        /*
         * ====================================================================
         * DESPACHO R7: ALT
         * ====================================================================
         * Função aplicada: altura(raiz)
         * Retorno esperado: número inteiro (0 para árvore vazia, 1 para único nó).
         */
        else if (strcmp(operacao, "ALT") == 0) {
            int h = altura(raiz);
            buffer_acrescentar_formato(&saida, "%d\n", h);
        }

        /*
         * ====================================================================
         * DESPACHO R8: SEMANA linha data hora k
         * ====================================================================
         * Função aplicada: executar_espera_semanal(raiz, linha_onibus, data, hora, k, &saida)
         * Retorno esperado: k linhas da mais antiga até a data informada com
         * "data espera" ou "data NAOPASSOU".
         */
        else if (strcmp(operacao, "SEMANA") == 0) {
            char linha_onibus[16], data[16], hora[16];
            int k = 0;
            if (sscanf(ptr, "%*s %15s %15s %15s %d", linha_onibus, data, hora, &k) == 4) {
                executar_espera_semanal(raiz, linha_onibus, data, hora, k, &saida);
            }
        }
    }

    // 5. Término da medição do tempo das operações
#ifdef _WIN32
    QueryPerformanceCounter(&t_fim);
    double tempo_ms = (double)(t_fim.QuadPart - t_inicio.QuadPart) * 1000.0 / (double)freq.QuadPart;
#else
    clock_t c_fim = clock();
    double tempo_ms = (double)(c_fim - c_inicio) * 1000.0 / CLOCKS_PER_SEC;
#endif

    // 6. Escrita obrigatória de respostas no final na saída padrão (stdout)
    if (saida.dados != NULL && saida.tamanho > 0) {
        fwrite(saida.dados, 1, saida.tamanho, stdout);
    }

    // 7. Escrita do tempo gasto exclusivamente com operações na saída de erro (stderr)
    // Formato obrigatório: TEMPO_MS valor com ponto decimal e 3 casas decimais
    fprintf(stderr, "TEMPO_MS %.3f\n", tempo_ms);

    // 8. Desalocação de todos os recursos de memória
    buffer_destruir(&saida);
    liberar_lista_linhas(&operacoes);
    liberar_arvore(raiz);

    return 0;
}
