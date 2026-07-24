#ifndef BLOCO_MEMORIA_H
#define BLOCO_MEMORIA_H

typedef struct {
    int palavras[4]; // bloco de 4 palavras
    int endBloco;
    int atualizado;
    int custo;
    int cacheHit; //4 PARA RAM E 5 PARA DISCO
    int contador;    // para implementar algoritmo LRU de retirada
} BlocoMemoria;

#endif