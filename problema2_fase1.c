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

// R2: buscar_chegada(data, horario, linha)
// Retorna 1 para "ACHOU" e 0 para "NAO"
int buscar_chegada(No *raiz, const char *data, const char *horario, const char *linha) {
    if (raiz == NULL) {
        return 0; // NAO encontrou
    }

    int cmp = comparar(data, horario, linha, raiz->data, raiz->horario, raiz->linha);

    if (cmp == 0) {
        return 1; // ACHOU
    } else if (cmp < 0) {
        return buscar_chegada(raiz->esq, data, horario, linha);
    } else {
        return buscar_chegada(raiz->dir, data, horario, linha);
    }
}

// Função auxiliar para comparar se o instante (data, horario) é maior ou igual
// Retorna 1 se (d1, h1) >= (d2, h2), e 0 caso contrário.
int instante_maior_igual(const char *d1, const char *h1, const char *d2, const char *h2) {
    int cmp = strcmp(d1, d2);
    if (cmp > 0) return 1;
    if (cmp < 0) return 0;
    return strcmp(h1, h2) >= 0;
}

// R3: proximo(data, horario)
// Retorna um ponteiro para o nó correspondente ou NULL se não houver chegada a partir do instante
No* proximo_chegada(No *raiz, const char *data, const char *horario) {
    No *atual = raiz;
    No *candidato = NULL;

    while (atual != NULL) {
        // Compara o nó atual com o instante buscado
        int cmp_data = strcmp(atual->data, data);
        int cmp_horario = strcmp(atual->horario, horario);

        // Verifica se o nó atual é posterior ou igual ao instante de referência
        int eh_valido = (cmp_data > 0) || (cmp_data == 0 && cmp_horario >= 0);

        if (eh_valido) {
            // Pode ser o nosso candidato; guardamos e tentamos buscar um ainda mais próximo (à esquerda)
            candidato = atual;
            atual = atual->esq;
        } else {
            // O nó atual é anterior ao instante, então o próximo válido só pode estar à direita
            atual = atual->dir;
        }
    }

    return candidato;
}

int main(){
	setlocale(LC_ALL, "Portuguese");
	printf("Função R1 (registrar_chegada) feita! - Joaquim");
	printf("Função R2 (buscar_chegada) feita! - Joaquim");
	printf("Função R3 (proximo_chegada) feita! - Joaquim");
	
	char operacao[10], data[11], horario[9], linha_onibus[9];
	
	//Integração do R1:
	if (sscanf(linha_arquivo, "%s %s %s %s", operacao, data, horario, linha_onibus) == 4) {
    
    if (strcmp(operacao, "REG") == 0) {
        // 2. Chama a sua função R1 passando o ponteiro para a raiz
        int resultado = registrar_chegada(&raiz, data, horario, linha_onibus);
        
        // 3. Imprime a resposta exata exigida pelo enunciado
        if (resultado == 1) {
            printf("OK\n");
        } else {
            printf("DUPLICADO\n");
        }
    }
	}
	
	
	//Integração do R2:
	if (strcmp(operacao, "BUSCA") == 0) {
    int res = buscar_chegada(raiz, data, horario, linha_onibus);
    if (res == 1) {
        printf("ACHOU\n");
    } else {
        printf("NAO\n");
    }
    
    //Integração do R3:
    if (strcmp(operacao, "PROX") == 0) {
    char data_buscada[11], horario_buscado[9];
    // O comando PROX lê apenas data e horário[cite: 2]
    if (sscanf(linha_arquivo, "%s %s %s", operacao, data_buscada, horario_buscado) == 3) {
        No *res = proximo_chegada(raiz, data_buscada, horario_buscado);
        if (res != NULL) {
            // Responde no formato "data hora linha"[cite: 2]
            printf("%s %s %s\n", res->data, res->horario, res->linha);
        } else {
            printf("NENHUM\n");
        }
    }
	}
    
    
	}


}