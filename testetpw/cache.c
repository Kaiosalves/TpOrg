#include "cache.h"
#include <stdlib.h>
#include <stdio.h>


void inicializarCache(Cache* cache, int numConjuntos, int vias){
    cache->numConjuntos = numConjuntos;
    cache->vias = vias;
    cache->totalLinhas = numConjuntos * vias;
    cache->linhas = (BlocoMemoria*) malloc(cache->totalLinhas * sizeof(BlocoMemoria));
    for (int i = 0; i < cache->totalLinhas; i++) {
        cache->linhas[i].endBloco = -1;
        cache->linhas[i].atualizado = 0;
        cache->linhas[i].contador = 0;
        cache->linhas[i].custo = 0;
        cache->linhas[i].cacheHit = 0;
        for (int j = 0; j < 4; j++) {
            cache->linhas[i].palavras[j] = 0;
        }
    }
}

void liberarCache(Cache *cache) {
    if (cache->linhas) {
        free(cache->linhas);
        cache->linhas = NULL;
    }
}

int buscarNaCache(Cache *cache, int endBloco) {
    int conj = endBloco % cache->numConjuntos;
    int inicio = conj * cache->vias;
    for (int i = 0; i < cache->vias; i++) {
        int id = inicio + i;
        if (cache->linhas[id].endBloco == endBloco) {
            return id;
        }
    }
    return -1;
}

int encontrarLRU(Cache *cache, int endBloco) {
    int conj = endBloco % cache->numConjuntos;
    int inicio = conj * cache->vias;
    int menor = inicio;
    for(int i = 1; i < cache->vias; i++){
        int id = inicio + i;
        if(cache->linhas[id].contador < cache->linhas[menor].contador){
            menor = id;
        }
    }
    return menor;
}

void atualizarLRU(Cache *cache, int id, int *relogio) {
    cache->linhas[id].contador = (*relogio)++;
}