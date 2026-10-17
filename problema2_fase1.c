#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// Estrutura básica do nó
typedef struct No {
    char data[11];
    char horario[9];
    char linha[9];
    struct No *esq;
    struct No *dir;
} No;

// Compara duas chegadas seguindo a ordem cronológica e o código da linha
int comparar(const char *d1, const char *h1, const char *l1, const char *d2, const char *h2, const char *l2) {
    int cmp = strcmp(d1, d2);
    if (cmp != 0) return cmp;
    
    cmp = strcmp(h1, h2);
    if (cmp != 0) return cmp;
    
    return strcmp(l1, l2);
}

// R1: Versão simples e direta para a Fase 1
// Retorna 1 para "OK" e 0 para "DUPLICADO"
int registrar_chegada(No **raiz, const char *data, const char *horario, const char *linha) {
    // Se a árvore (ou subárvore) estiver vazia, cria o nó aqui
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

    int cmp = comparar(data, horario, linha, (*raiz)->data, (*raiz)->horario, (*raiz)->linha);

    if (cmp < 0) {
        return registrar_chegada(&((*raiz)->esq), data, horario, linha);
    } else if (cmp > 0) {
        return registrar_chegada(&((*raiz)->dir), data, horario, linha);
    } else {
        return 0; // DUPLICADO (já existe na árvore)
    }
}
int main(){
	setlocale(LC_ALL, "Portuguese");
	printf("Função R1 (registrar_chegada) feita! - Joaquim");
}