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

int registrar_chegada(No **raiz, char data[], char horario[], char linha[]) {
    if (*raiz == NULL) {
        No *aux = (No *)malloc(sizeof(No));

        if (aux == NULL) {
            return -1;
        }

        strcpy(aux->data, data);
        strcpy(aux->horario, horario);
        strcpy(aux->linha, linha);

        aux->altura = 1;
        aux->esq = NULL;
        aux->dir = NULL;

        *raiz = aux;

        return 1;
    }

    int resultado;

    resultado = strcmp(data, (*raiz)->data);

    if (resultado == 0) {
        resultado = strcmp(horario, (*raiz)->horario);
    }

    if (resultado == 0) {
        resultado = strcmp(linha, (*raiz)->linha);
    }

    if (resultado == 0) {
        return 0;
    }

    if (resultado < 0) {
        return registrar_chegada(&(*raiz)->esq, data, horario, linha);
    } else {
        return registrar_chegada(&(*raiz)->dir, data, horario, linha);
    }
}

int main(){
    No *raiz = NULL;

    return 0;
}