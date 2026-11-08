#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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
No* proximo_da_linha(No *raiz, const char *data, const char *horario, const char *linha) {

    if (raiz == NULL) {
        return NULL;
    }

    // Compara data e horario
    int resultado = strcmp(data, raiz->data);

    if (resultado == 0) {
        resultado = strcmp(horario, raiz->horario);
    }

    // Nó atual é anterior  consulta
    if (resultado > 0) {
        return proximo_da_linha(raiz->dir, data, horario, linha);
    }

    // Nó atual é igual ou posterior a consulta
    No *esquerda = proximo_da_linha(raiz->esq, data, horario, linha);

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
            raiz->esq = remover_chegada(
                raiz->esq, aux->data, aux->horario, aux->linha
            );
        }
    }

    return balanceamento(raiz);
}

int altura(No *raiz){
    return alturaNo(raiz) + 1;
}
int main(){
    No *raiz = NULL;

    return 0;
}