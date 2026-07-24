#ifndef CACHE_H
#define CACHE_H

#include "BlocoMemoria.h"

typedef struct Cache {
    int numConjuntos;
    int vias;
    int totalLinhas;
    BlocoMemoria *linhas;
} Cache;

void inicializarCache(Cache* cache, int numConjuntos, int vias);
void liberarCache(Cache* cache);
// Busca um bloco na cache. Retorna o índice da linha se encontrado, senão -1.
int buscarNaCache(Cache* cache, int endBloco);
// Encontra a linha com menor contador (LRU) dentro do conjunto correspondente a endBloco.
int encontrarLRU(Cache* cache, int endBloco);
// Atualiza o contador de uma linha para o valor atual do relógio (LRU).
void atualizarLRU(Cache* cache, int indice, int *relogio);



#endif