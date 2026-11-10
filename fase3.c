#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

typedef struct No {
    char data[11];
    char horario[10];
    char linha[10];

    int altura;

    struct No *esq;
    struct No *dir;
} No;

int maior(int a, int b){
    return (a > b)? a:b;
}

int alturaNo(No *no){
    if(no == NULL){
        return -1;
    }else{
        return no->altura;
    }
}

int fatorBalanceamento(No *no){
    if(no){
        return (alturaNo(no->esq) - alturaNo(no->dir));
    }else{
        return 0;
    }
}
No* rotacaoEsquerda(No *r){
    No *y, *f;

    y = r->dir;
    f = y->esq;

    y->esq = r;
    r->dir = f;

    r->altura = maior(alturaNo(r->esq), alturaNo(r->dir))+1;
    y->altura = maior(alturaNo(y->esq), alturaNo(y->dir))+1;

    return y;
}
No* rotacaoDireita(No *r){
    No *y, *f;

    y = r->esq;
    f = y->dir;

    y->dir = r;
    r->esq = f;

    r->altura = maior(alturaNo(r->esq), alturaNo(r->dir))+1;
    y->altura = maior(alturaNo(y->esq), alturaNo(y->dir))+1;

    return y;
}
No* rotacaoDireitaEsquerda(No *r){
    r->dir = rotacaoDireita(r->dir);
    return rotacaoEsquerda(r);
}
No* rotacaoEsquerdaDireita(No *r){
    r->esq = rotacaoEsquerda(r->esq);
    return rotacaoDireita(r);
}
No* balanceamento(No *raiz){
    if (raiz == NULL) {
        return NULL;
    }

    raiz->altura = 1 + maior(
        alturaNo(raiz->esq),
        alturaNo(raiz->dir)
    );

    int fb = fatorBalanceamento(raiz);

    if(fb < -1 && fatorBalanceamento(raiz->dir) <= 0){
        raiz = rotacaoEsquerda(raiz);
    }
    else if(fb > 1 && fatorBalanceamento(raiz->esq) >=0){
        raiz = rotacaoDireita(raiz);
    }
    else if(fb > 1 && fatorBalanceamento(raiz->esq) < 0){
        raiz = rotacaoEsquerdaDireita(raiz);
    }
    else if(fb < -1 && fatorBalanceamento(raiz->dir) > 0){
        raiz = rotacaoDireitaEsquerda(raiz);
    }

    return raiz;
}
int comparar_chegadas(const char *data1, const char *horario1, const char *linha1,
                      const char *data2, const char *horario2, const char *linha2) {

    int resultado = strcmp(data1, data2);

    if (resultado == 0) {
        resultado = strcmp(horario1, horario2);
    }

    if (resultado == 0) {
        resultado = strcmp(linha1, linha2);
    }

    return resultado;
}
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
long horario_para_segundos(const char *horario) {
    int h = 0, m = 0, s = 0;
    sscanf(horario, "%d:%d:%d", &h, &m, &s);
    return (long)h * 3600 + (long)m * 60 + (long)s;
}
int registrar_chegada(No **raiz, char data[], char horario[], char linha[]) {
    //Árvore vazia: cria um novo nó
    if (*raiz == NULL) {
        No *novo = (No *) malloc(sizeof(No));

        if (novo == NULL) {
            return -1;
        }

        strcpy(novo->data, data);
        strcpy(novo->horario, horario);
        strcpy(novo->linha, linha);

        novo->altura = 0;

        novo->esq = NULL;
        novo->dir = NULL;

        *raiz = novo;

        return 1;
    }

    //Compara a nova chegada com o nó atual
    int resultado = comparar_chegadas(data, horario, linha, (*raiz)->data, (*raiz)->horario, (*raiz)->linha);
    //Chegada duplicada
    if (resultado == 0) {
        return 0;
    }

    //Insere na esquerda
    if (resultado < 0) {
        int status = registrar_chegada(&(*raiz)->esq, data, horario, linha);

        if (status != 1) {
            return status;
        }
    }
    //Insere na direita
    else {
        int status = registrar_chegada(&(*raiz)->dir, data, horario, linha);

        if (status != 1) {
            return status;
        }
    }

    //Atualiza e balanceia a árvore
    *raiz = balanceamento(*raiz);

    return 1;
}
int buscar_chegada(No *raiz, const char *data,
                   const char *horario, const char *linha) {

    //Árvore vazia: chegada não encontrada
    if (raiz == NULL) {
        return 0;
    }

    //Compara a data
    int resultado = comparar_chegadas(data, horario, linha, raiz->data, raiz->horario, raiz->linha);
    //Chegada encontrada
    if (resultado == 0) {
        return 1;
    }

    //Procura na esquerda
    if (resultado < 0) {
        return buscar_chegada(raiz->esq, data, horario, linha);
    }

    //Procura na direita
    return buscar_chegada(raiz->dir, data, horario, linha);
}
No* proximo(No *raiz, const char *data, const char *horario) {

    //arvore vazia: não existe chegada
    if (raiz == NULL) {
        return NULL;
    }

    //Compara a data e o horario
    int resultado = strcmp(data, raiz->data);

    if (resultado == 0) {
        resultado = strcmp(horario, raiz->horario);
    }

    //A chegada atual é anterior a procurada
    if (resultado > 0) {
        return proximo(raiz->dir, data, horario);
    }

    //A chegada atual é igual ou posterior a procurada
    else {
        No *candidato = proximo(raiz->esq, data, horario);

        if (candidato != NULL) {
            return candidato;
        }

        return raiz;
    }
}
const No* proximo_da_linha(const No *raiz, const char *data, const char *horario, const char *linha) {

    if (raiz == NULL) {
        return NULL;
    }

    //Compara data e horario
    int resultado = strcmp(data, raiz->data);

    if (resultado == 0) {
        resultado = strcmp(horario, raiz->horario);
    }

    //Nó atual é anterior  consulta
    if (resultado > 0) {
        return proximo_da_linha(raiz->dir, data, horario, linha);
    }

    //Nó atual é igual ou posterior a consulta
    const No *esquerda = proximo_da_linha(raiz->esq, data, horario, linha);

    //se encontrou uma chegada válida a esquerda, ela é a melhor
    if (esquerda != NULL) {
        return esquerda;
    }

    //Verifica se o próprio nó corresponde à linha
    if (strcmp(linha, raiz->linha) == 0) {
        return raiz;
    }

    //Procura na direita se o nó atual não corresponde
    return proximo_da_linha(raiz->dir, data, horario, linha);
}
void entre(No *raiz, const char *data1, const char *horario1, const char *data2, const char *horario2){
    if(raiz == NULL){
        return;
    }

    int inicio = strcmp(raiz->data, data1);
    if(inicio==0){
        inicio = strcmp(raiz->horario, horario1);
    }

    int fim = strcmp(raiz->data, data2);
    if(fim ==0){
        fim = strcmp(raiz->horario, horario2);
    }

    if(inicio < 0){
        entre(raiz->dir, data1, horario1, data2, horario2);
    }
    else if(fim > 0){
        entre(raiz->esq, data1, horario1, data2, horario2);
    }else{
        entre(raiz->esq, data1, horario1, data2, horario2);
        
        printf("%s %s %s\n", raiz->data, raiz->horario, raiz->linha);

        entre(raiz->dir, data1, horario1, data2, horario2);
    }
}
No* remover_chegada(No *raiz, const char *data,
                    const char *horario, const char *linha) {

    if (raiz == NULL) {
        return NULL;
    }

    int resultado = comparar_chegadas(
        data, horario, linha,
        raiz->data, raiz->horario, raiz->linha
    );

    if (resultado < 0) {
        raiz->esq = remover_chegada(
            raiz->esq, data, horario, linha
        );
    }
    else if (resultado > 0) {
        raiz->dir = remover_chegada(
            raiz->dir, data, horario, linha
        );
    }
    else {
        //Caso 1: folha ou apenas um filho
        if (raiz->esq == NULL || raiz->dir == NULL) {
            No *aux = raiz;

            if (raiz->esq != NULL) {
                raiz = raiz->esq;
            } else {
                raiz = raiz->dir;
            }

            free(aux);
        }

        //Caso 2: dois filhos
        else {
            No *aux = raiz->esq;

            //Encontrar o antecessor
            while (aux->dir != NULL) {
                aux = aux->dir;
            }

            //Copiar os dados do antecessor
            strcpy(raiz->data, aux->data);
            strcpy(raiz->horario, aux->horario);
            strcpy(raiz->linha, aux->linha);

            //Remover o antecessor original
            raiz->esq = remover_chegada(raiz->esq, aux->data, aux->horario, aux->linha);
        }
    }

    return balanceamento(raiz);
}
void executar_espera_semanal(const No *raiz, const char *linha,
                             const char *data, const char *horario,
                             int k) {
    if (k <= 0) return;

    long seg_referencia = horario_para_segundos(horario);

    for (int sem = k - 1; sem >= 0; sem--) {
        char data_semana[12];

        calcular_data_offset_semanas(data, sem, data_semana);

        const No *achou = proximo_da_linha(
            raiz, data_semana, horario, linha
        );

        if (achou != NULL && strcmp(achou->data, data_semana) == 0) {
            long seg_chegada = horario_para_segundos(achou->horario);
            long espera = seg_chegada - seg_referencia;

            if (espera < 0) espera = 0;

            printf("%s %ld\n", data_semana, espera);
        } else {
            printf("%s NAOPASSOU\n", data_semana);
        }
    }
}
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
void liberar_arvore(No *raiz) {
    if (raiz == NULL) {
        return;
    }

    liberar_arvore(raiz->esq);
    liberar_arvore(raiz->dir);

    free(raiz);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_operacoes>\n", argv[0]);
        return 1;
    }

    const char *caminho_arquivo = argv[1];

    ListaLinhas operacoes;

    if (!carregar_arquivo_em_memoria(caminho_arquivo, &operacoes)) {
        fprintf(stderr, "Erro ao abrir ou ler o arquivo: %s\n",
                caminho_arquivo);
        return 1;
    }

    No *raiz = NULL;

    // Inicia medicao de tempo
    clock_t c_inicio = clock();

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

            if (sscanf(ptr, "%*s %15s %15s %15s",
                       data, hora, linha_onibus) == 3) {

                int res = registrar_chegada(
                    &raiz, data, hora, linha_onibus
                );

                if (res == 1) {
                    printf("OK\n");
                }
                else if (res == 0) {
                    printf("DUPLICADO\n");
                }
                else {
                    fprintf(stderr, "Erro de alocacao de memoria\n");
                }
            }
        }

        // R2: Busca de chegada
        else if (strcmp(operacao, "BUSCA") == 0) {
            char data[16], hora[16], linha_onibus[16];

            if (sscanf(ptr, "%*s %15s %15s %15s",
                       data, hora, linha_onibus) == 3) {

                int res = buscar_chegada(
                    raiz, data, hora, linha_onibus
                );

                if (res == 1) {
                    printf("ACHOU\n");
                } else {
                    printf("NAO\n");
                }
            }
        }

        // R3: Proxima chegada geral
        else if (strcmp(operacao, "PROX") == 0) {
            char data[16], hora[16];

            if (sscanf(ptr, "%*s %15s %15s", data, hora) == 2) {

                const No *res = proximo(raiz, data, hora);

                if (res != NULL) {
                    printf("%s %s %s\n",
                           res->data, res->horario, res->linha);
                } else {
                    printf("NENHUM\n");
                }
            }
        }

        // R4: Proxima chegada de linha especifica
        else if (strcmp(operacao, "PROXL") == 0) {
            char data[16], hora[16], linha_onibus[16];

            if (sscanf(ptr, "%*s %15s %15s %15s",
                       data, hora, linha_onibus) == 3) {

                const No *res = proximo_da_linha(
                    raiz, data, hora, linha_onibus
                );

                if (res != NULL) {
                    printf("%s %s\n", res->data, res->horario);
                } else {
                    printf("NENHUM\n");
                }
            }
        }

        // R5: Chegadas em intervalo inclusivo
        else if (strcmp(operacao, "ENTRE") == 0) {
            char d1[16], h1[16], d2[16], h2[16];

            if (sscanf(ptr, "%*s %15s %15s %15s %15s",
                       d1, h1, d2, h2) == 4) {

                entre(raiz, d1, h1, d2, h2);
            }
        }

        // R6: Remocao de chegada
        else if (strcmp(operacao, "REM") == 0) {
            char data[16], hora[16], linha_onibus[16];

            if (sscanf(ptr, "%*s %15s %15s %15s",
                       data, hora, linha_onibus) == 3) {

                if (buscar_chegada(raiz, data, hora, linha_onibus)) {
                    raiz = remover_chegada(
                        raiz, data, hora, linha_onibus
                    );

                    printf("OK\n");
                } else {
                    printf("NAO\n");
                }
            }
        }

        // R7: Altura da arvore
        else if (strcmp(operacao, "ALT") == 0) {
            printf("%d\n", alturaNo(raiz) + 1);
        }

        // R8: Espera semanal
        else if (strcmp(operacao, "SEMANA") == 0) {
            char linha_onibus[16], data[16], hora[16];
            int k = 0;

            if (sscanf(ptr, "%*s %15s %15s %15s %d",
                       linha_onibus, data, hora, &k) == 4) {

                executar_espera_semanal(
                    raiz, linha_onibus, data, hora, k
                );
            }
        }
    }

    // Finaliza medicao de tempo
    clock_t c_fim = clock();

    double tempo_ms = (double)(c_fim - c_inicio)
                      * 1000.0 / CLOCKS_PER_SEC;

    fprintf(stderr, "TEMPO_MS %.3f\n", tempo_ms);

    // Desaloca memoria
    liberar_lista_linhas(&operacoes);
    liberar_arvore(raiz);

    return 0;
}