#include "UCM.h"
#include <stdio.h>
#include <stdlib.h>




static void subirBloco(Cache *dest, int endBloco, Cache *origem, int idOrig, Ram* ram, int *relogio){
//encontra a posicao para substituir no destino
    int idDest = encontrarLRU(dest, endBloco);
//escreve na RAM se a linha de destino esta suja
    if(dest->linhas[idDest].atualizado && dest->linhas[idDest].endBloco != -1){
        setBloco(ram, dest->linhas[idDest].endBloco, dest->linhas[idDest]);
    }

//copia o bloco da origem par o destino
    dest->linhas[idDest] = origem->linhas[idOrig];
//atualiza contador lru
    dest->linhas[idDest].contador = (*relogio)++;
    dest->linhas[idDest].atualizado = 0;
}


void inicializarUCM(UCM *ucm, Cache *L1, Cache *L2, Cache *L3, Ram *ram){
    ucm->L1 = L1;
    ucm->L2 = L2;
    ucm->L3 = L3;
    ucm->ram = ram;
    ucm->relogio = 0;

    ucm->hitsL1 = ucm->missesL1 = 0;
    ucm->hitsL2 = ucm->missesL2 = 0;
    ucm->hitsL3 = ucm->missesL3 = 0;
    ucm->tempoTotal = 0;
}


BlocoMemoria* ucm_ler(UCM *ucm, Endereco end){

}

void ucm_escrever(UCM *ucm, Endereco end, int valor){
    int bloco = end.endBloco;
    int offset = end.endPalavra;
    int id;

// tenta na L1
    id = buscarNaCache(ucm->L1, end.endBloco);
    if(id != -1){
        ucm->hitsL1++;
        atualizarLRU(ucm->L1, id, &ucm->relogio);
        ucm->tempoTotal +=1;
        ucm->L1->linhas[id].palavras[offset] = valor;
        ucm->L1->linhas[id].atualizado = 1; // marcado como sujo
        return;
    }
    ucm->missesL1++;

//se nao esta em L1, trazemos o bloco (leitura) depois escrevemos
    BlocoMemoria *bloco = ucm_ler(ucm, end);
    //bloco agora esta em L1, encontramos a linha
    int idl1 = buscarNaCache(ucm->L1, end.endBloco);
    if(idl1 != -1){
        ucm->L1->linhas[idl1].palavras[offset] = valor;
        ucm->L1->linhas[idl1].atualizado = 1;
    
    } else return;

}

void imprimirEstatisticas(UCM *ucm){
printf("\n========== ESTATÍSTICAS DA UCM ==========\n");
    int totalL1 = ucm->hitsL1 + ucm->missesL1;
    int totalL2 = ucm->hitsL2 + ucm->missesL2;
    int totalL3 = ucm->hitsL3 + ucm->missesL3;
    printf("L1 - Hits: %d, Misses: %d, Hit Rate: %.2f%%\n",
           ucm->hitsL1, ucm->missesL1,
           totalL1 ? (100.0 * ucm->hitsL1 / totalL1) : 0.0);
    printf("L2 - Hits: %d, Misses: %d, Hit Rate: %.2f%%\n",
           ucm->hitsL2, ucm->missesL2,
           totalL2 ? (100.0 * ucm->hitsL2 / totalL2) : 0.0);
    printf("L3 - Hits: %d, Misses: %d, Hit Rate: %.2f%%\n",
           ucm->hitsL3, ucm->missesL3,
           totalL3 ? (100.0 * ucm->hitsL3 / totalL3) : 0.0);
    printf("Tempo total estimado: %lld ciclos\n", ucm->tempoTotal);
    printf("===========================================\n");
}


