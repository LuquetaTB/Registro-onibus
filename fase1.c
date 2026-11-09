#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

// No da Arvore Binaria de Busca (ABB)
typedef struct No {
    char data[12];
    char horario[10];
    char linha[10];
    struct No *esq;
    struct No *dir;
} No;

// Compara dois instantes (data e horario)
int comparar_instante(const char *d1, const char *h1, const char *d2, const char *h2) {
    int cmp_data = strcmp(d1, d2);
    if (cmp_data != 0) return cmp_data;
    return strcmp(h1, h2);
}

// Compara a chave completa: (data, horario, linha)
int comparar_chegadas(const char *d1, const char *h1, const char *l1,
                      const char *d2, const char *h2, const char *l2) {
    int cmp_instante = comparar_instante(d1, h1, d2, h2);
    if (cmp_instante != 0) return cmp_instante;
    return strcmp(l1, l2);
}

// Converte horario "HH:MM:SS" em segundos do dia
long horario_para_segundos(const char *horario) {
    int h = 0, m = 0, s = 0;
    sscanf(horario, "%d:%d:%d", &h, &m, &s);
    return (long)h * 3600 + (long)m * 60 + (long)s;
}

// Subtrai semanas de uma data (formato AAAA-MM-DD)
void calcular_data_offset_semanas(const char *data_base, int semanas_atras, char *data_saida) {
    int ano = 0, mes = 0, dia = 0;
    sscanf(data_base, "%d-%d-%d", &ano, &mes, &dia);

    struct tm t;
    memset(&t, 0, sizeof(struct tm));
    t.tm_year = ano - 1900;
    t.tm_mon = mes - 1;
    t.tm_mday = dia - (7 * semanas_atras);
    t.tm_hour = 12;
    t.tm_isdst = -1;

    mktime(&t);
    sprintf(data_saida, "%04d-%02d-%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
}

// Buffer dinamico para acumular respostas em memoria
typedef struct {
    char *dados;
    size_t tamanho;
    size_t capacidade;
} BufferSaida;

// Inicializa o buffer de saida
void buffer_inicializar(BufferSaida *b) {
    b->capacidade = 1024 * 1024;
    b->dados = (char *)malloc(b->capacidade);
    b->tamanho = 0;
    if (b->dados != NULL) b->dados[0] = '\0';
}

// Concatena string no buffer
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

// Concatena texto formatado no buffer
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

// Libera memoria do buffer
void buffer_destruir(BufferSaida *b) {
    if (b->dados != NULL) free(b->dados);
    b->dados = NULL;
    b->tamanho = 0;
    b->capacidade = 0;
}

// R1: Insere nova chegada na ABB (iterativo para evitar stack overflow)
int registrar_chegada(No **raiz, const char *data, const char *horario, const char *linha) {
    if (*raiz == NULL) {
        No *novo = (No *)malloc(sizeof(No));
        strcpy(novo->data, data);
        strcpy(novo->horario, horario);
        strcpy(novo->linha, linha);
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }

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
                return 1;
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
                return 1;
            }
            atual = atual->dir;
        } else {
            return 0; // Registro duplicado
        }
    }
}

// R2: Busca chegada exata por (data, horario, linha)
int buscar_chegada(const No *raiz, const char *data, const char *horario, const char *linha) {
    const No *atual = raiz;
    while (atual != NULL) {
        int cmp = comparar_chegadas(data, horario, linha, atual->data, atual->horario, atual->linha);
        if (cmp == 0) return 1;
        else if (cmp < 0) atual = atual->esq;
        else atual = atual->dir;
    }
    return 0;
}

// R3: Busca a primeira chegada a partir de (data, horario)
const No* proximo_chegada(const No *raiz, const char *data, const char *horario) {
    const No *candidato = NULL;
    const No *atual = raiz;

    while (atual != NULL) {
        int cmp = comparar_chegadas(atual->data, atual->horario, atual->linha, data, horario, "");
        if (cmp >= 0) {
            candidato = atual;
            atual = atual->esq;
        } else {
            atual = atual->dir;
        }
    }
    return candidato;
}

// R4: Busca recursiva da primeira chegada de uma linha a partir do instante
int proximo_da_linha_rec(const No *raiz, const char *data, const char *horario, const char *linha,
                         char *res_data, char *res_horario) {
    if (raiz == NULL) return 0;

    // Poda subarvore esquerda se for anterior ao instante
    if (comparar_instante(raiz->data, raiz->horario, data, horario) < 0) {
        return proximo_da_linha_rec(raiz->dir, data, horario, linha, res_data, res_horario);
    }

    if (proximo_da_linha_rec(raiz->esq, data, horario, linha, res_data, res_horario)) return 1;

    if (strcmp(raiz->linha, linha) == 0) {
        strcpy(res_data, raiz->data);
        strcpy(res_horario, raiz->horario);
        return 1;
    }

    return proximo_da_linha_rec(raiz->dir, data, horario, linha, res_data, res_horario);
}

// R4: Interface para buscar proxima chegada da linha
int proximo_da_linha(const No *raiz, const char *data, const char *horario, const char *linha,
                     char *res_data, char *res_horario) {
    return proximo_da_linha_rec(raiz, data, horario, linha, res_data, res_horario);
}

// Conta chegadas no intervalo inclusivo [instante1, instante2]
void contar_chegadas_entre(const No *raiz, const char *d1, const char *h1,
                           const char *d2, const char *h2, int *total) {
    if (raiz == NULL) return;

    int cmp1 = comparar_instante(raiz->data, raiz->horario, d1, h1);
    int cmp2 = comparar_instante(raiz->data, raiz->horario, d2, h2);

    if (cmp1 >= 0) contar_chegadas_entre(raiz->esq, d1, h1, d2, h2, total);
    if (cmp1 >= 0 && cmp2 <= 0) (*total)++;
    if (cmp2 <= 0) contar_chegadas_entre(raiz->dir, d1, h1, d2, h2, total);
}

// Coleta e formata chegadas no intervalo em ordem simetrica
void coletar_chegadas_entre(const No *raiz, const char *d1, const char *h1,
                            const char *d2, const char *h2, BufferSaida *buf) {
    if (raiz == NULL) return;

    int cmp1 = comparar_instante(raiz->data, raiz->horario, d1, h1);
    int cmp2 = comparar_instante(raiz->data, raiz->horario, d2, h2);

    if (cmp1 >= 0) coletar_chegadas_entre(raiz->esq, d1, h1, d2, h2, buf);
    if (cmp1 >= 0 && cmp2 <= 0) {
        buffer_acrescentar_formato(buf, "%s %s %s\n", raiz->data, raiz->horario, raiz->linha);
    }
    if (cmp2 <= 0) coletar_chegadas_entre(raiz->dir, d1, h1, d2, h2, buf);
}

// R5: Processa consulta de chegadas entre dois instantes
void executar_entre(const No *raiz, const char *d1, const char *h1,
                    const char *d2, const char *h2, BufferSaida *buf) {
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

// Encontra o menor no da subarvore (sucessor)
No* obter_menor_no(No *nodo) {
    No *atual = nodo;
    while (atual != NULL && atual->esq != NULL) atual = atual->esq;
    return atual;
}

// R6: Remove chegada substituindo por sucessor quando ha 2 filhos
int remover_chegada(No **raiz, const char *data, const char *horario, const char *linha) {
    if (*raiz == NULL) return 0;

    int cmp = comparar_chegadas(data, horario, linha, (*raiz)->data, (*raiz)->horario, (*raiz)->linha);
    if (cmp < 0) {
        return remover_chegada(&((*raiz)->esq), data, horario, linha);
    } else if (cmp > 0) {
        return remover_chegada(&((*raiz)->dir), data, horario, linha);
    } else {
        No *alvo = *raiz;
        if ((*raiz)->esq == NULL) {
            *raiz = (*raiz)->dir;
            free(alvo);
        } else if ((*raiz)->dir == NULL) {
            *raiz = (*raiz)->esq;
            free(alvo);
        } else {
            No *sucessor = obter_menor_no((*raiz)->dir);
            strcpy((*raiz)->data, sucessor->data);
            strcpy((*raiz)->horario, sucessor->horario);
            strcpy((*raiz)->linha, sucessor->linha);
            remover_chegada(&((*raiz)->dir), sucessor->data, sucessor->horario, sucessor->linha);
        }
        return 1;
    }
}

// R7: Retorna a altura da arvore (0 para vazia, 1 para unico no)
int altura(const No *raiz) {
    if (raiz == NULL) return 0;
    int h_esq = altura(raiz->esq);
    int h_dir = altura(raiz->dir);
    return 1 + (h_esq > h_dir ? h_esq : h_dir);
}

// R8: Calcula espera da linha nas ultimas k semanas
void executar_espera_semanal(const No *raiz, const char *linha, const char *data,
                             const char *horario, int k, BufferSaida *buf) {
    if (k <= 0) return;
    long seg_referencia = horario_para_segundos(horario);

    for (int sem = k - 1; sem >= 0; sem--) {
        char data_semana[12];
        calcular_data_offset_semanas(data, sem, data_semana);

        char achou_data[12], achou_horario[10];
        int achou = proximo_da_linha(raiz, data_semana, horario, linha, achou_data, achou_horario);

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

// Libera nos da arvore por linearizacao segura
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

// Estrutura para armazenar linhas lidas do arquivo
typedef struct {
    char **linhas;
    size_t total;
    size_t capacidade;
    char *conteudo_bruto;
} ListaLinhas;

// Carrega o arquivo inteiro na memoria antes da execucao
int carregar_arquivo_em_memoria(const char *caminho, ListaLinhas *lista) {
    FILE *f = fopen(caminho, "rb");
    if (!f) return 0;

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
        while (*p == '\r' || *p == '\n') p++;
        if (*p == '\0') break;

        char *inicio_linha = p;
        while (*p != '\0' && *p != '\r' && *p != '\n') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }

        if (lista->total >= lista->capacidade) {
            lista->capacidade *= 2;
            lista->linhas = (char **)realloc(lista->linhas, lista->capacidade * sizeof(char *));
        }
        lista->linhas[lista->total++] = inicio_linha;
    }
    return 1;
}

// Libera memoria das linhas carregadas
void liberar_lista_linhas(ListaLinhas *lista) {
    if (lista->linhas) free(lista->linhas);
    if (lista->conteudo_bruto) free(lista->conteudo_bruto);
    lista->linhas = NULL;
    lista->conteudo_bruto = NULL;
    lista->total = 0;
    lista->capacidade = 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_operacoes>\n", argv[0]);
        return 1;
    }

    const char *caminho_arquivo = argv[1];

    ListaLinhas operacoes;
    if (!carregar_arquivo_em_memoria(caminho_arquivo, &operacoes)) {
        fprintf(stderr, "Erro ao abrir ou ler o arquivo: %s\n", caminho_arquivo);
        return 1;
    }

    No *raiz = NULL;
    BufferSaida saida;
    buffer_inicializar(&saida);

    // Inicia medicao de tempo das operacoes
#ifdef _WIN32
    LARGE_INTEGER freq, t_inicio, t_fim;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t_inicio);
#else
    clock_t c_inicio = clock();
#endif

    // Loop de despacho de operacoes
    for (size_t i = 0; i < operacoes.total; i++) {
        char *ptr = operacoes.linhas[i];
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == '\0' || *ptr == '#') continue;

        char operacao[32];
        if (sscanf(ptr, "%31s", operacao) != 1) continue;

        // R1: Registro de chegada
        if (strcmp(operacao, "REG") == 0) {
            char data[16], hora[16], linha_onibus[16];
            if (sscanf(ptr, "%*s %15s %15s %15s", data, hora, linha_onibus) == 3) {
                int res = registrar_chegada(&raiz, data, hora, linha_onibus);
                if (res == 1) buffer_acrescentar_str(&saida, "OK\n");
                else buffer_acrescentar_str(&saida, "DUPLICADO\n");
            }
        }
        // R2: Busca de chegada
        else if (strcmp(operacao, "BUSCA") == 0) {
            char data[16], hora[16], linha_onibus[16];
            if (sscanf(ptr, "%*s %15s %15s %15s", data, hora, linha_onibus) == 3) {
                int res = buscar_chegada(raiz, data, hora, linha_onibus);
                if (res == 1) buffer_acrescentar_str(&saida, "ACHOU\n");
                else buffer_acrescentar_str(&saida, "NAO\n");
            }
        }
        // R3: Proxima chegada geral
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
        // R4: Proxima chegada de linha especifica
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
        // R5: Chegadas em intervalo inclusivo
        else if (strcmp(operacao, "ENTRE") == 0) {
            char d1[16], h1[16], d2[16], h2[16];
            if (sscanf(ptr, "%*s %15s %15s %15s %15s", d1, h1, d2, h2) == 4) {
                executar_entre(raiz, d1, h1, d2, h2, &saida);
            }
        }
        // R6: Remocao de chegada
        else if (strcmp(operacao, "REM") == 0) {
            char data[16], hora[16], linha_onibus[16];
            if (sscanf(ptr, "%*s %15s %15s %15s", data, hora, linha_onibus) == 3) {
                int res = remover_chegada(&raiz, data, hora, linha_onibus);
                if (res == 1) buffer_acrescentar_str(&saida, "OK\n");
                else buffer_acrescentar_str(&saida, "NAO\n");
            }
        }
        // R7: Altura da arvore
        else if (strcmp(operacao, "ALT") == 0) {
            int h = altura(raiz);
            buffer_acrescentar_formato(&saida, "%d\n", h);
        }
        // R8: Espera semanal
        else if (strcmp(operacao, "SEMANA") == 0) {
            char linha_onibus[16], data[16], hora[16];
            int k = 0;
            if (sscanf(ptr, "%*s %15s %15s %15s %d", linha_onibus, data, hora, &k) == 4) {
                executar_espera_semanal(raiz, linha_onibus, data, hora, k, &saida);
            }
        }
    }

    // Finaliza medicao de tempo
#ifdef _WIN32
    QueryPerformanceCounter(&t_fim);
    double tempo_ms = (double)(t_fim.QuadPart - t_inicio.QuadPart) * 1000.0 / (double)freq.QuadPart;
#else
    clock_t c_fim = clock();
    double tempo_ms = (double)(c_fim - c_inicio) * 1000.0 / CLOCKS_PER_SEC;
#endif

    // Imprime respostas no stdout
    if (saida.dados != NULL && saida.tamanho > 0) {
        fwrite(saida.dados, 1, saida.tamanho, stdout);
    }

    // Imprime tempo no stderr
    fprintf(stderr, "TEMPO_MS %.3f\n", tempo_ms);

    // Desaloca memoria
    buffer_destruir(&saida);
    liberar_lista_linhas(&operacoes);
    liberar_arvore(raiz);

    return 0;
}
