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

int registrar_chegada(No **raiz, char data[], char horario[], char linha[]) {
    // Árvore vazia: cria um novo nó
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

    // Compara a nova chegada com o nó atual
    int resultado = strcmp(data, (*raiz)->data);

    if (resultado == 0) {
        resultado = strcmp(horario, (*raiz)->horario);
    }

    if (resultado == 0) {
        resultado = strcmp(linha, (*raiz)->linha);
    }

    // Chegada duplicada
    if (resultado == 0) {
        return 0;
    }

    // Insere na esquerda
    if (resultado < 0) {
        int status = registrar_chegada(&(*raiz)->esq, data, horario, linha);

        if (status != 1) {
            return status;
        }
    }
    // Insere na direita
    else {
        int status = registrar_chegada(&(*raiz)->dir, data, horario, linha);

        if (status != 1) {
            return status;
        }
    }

    // Atualiza e balanceia a árvore
    *raiz = balanceamento(*raiz);

    return 1;
}

int main(){
    No *raiz = NULL;

    return 0;
}