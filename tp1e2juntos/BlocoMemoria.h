#ifndef BLOCO_MEMORIA_H
#define BLOCO_MEMORIA_H

typedef struct {
    int palavras[4]; // bloco de 4 palavras
    int endBloco;
    int atualizado;
    int custo;
    int cacheHit;

    int frequencia;    // para implementar algoritmo LFU de retirada
} BlocoMemoria;

#endif