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

//invalida a linda na origem - pra ter politica de exclusividade
    origem->linhas[idOrig].endBloco = -1;
    origem->linhas[idOrig].atualizado = 0;
    for (int j = 0; j < 4; j++) {
        origem->linhas[idOrig].palavras[j] = 0;
    }
}

// Desce um bloco de um nível superior para o inferior
static void descerBloco(Cache *superior, int idSuperior, Cache *inferior, Ram *ram, int *relogio) {
    // Se o bloco no superior esta invlido, retorna
    if (superior->linhas[idSuperior].endBloco == -1) return;

    // Encontra linha para substituir no inferior (LRU)
    int idInf = encontrarLRU(inferior, superior->linhas[idSuperior].endBloco);
    // Se a linha no inferior estiver suja, escreve na RAM
    if (inferior->linhas[idInf].atualizado && inferior->linhas[idInf].endBloco != -1) {
        setBloco(ram, inferior->linhas[idInf].endBloco, inferior->linhas[idInf]);
    }
    // Copia o bloco do superior para o inferior
    inferior->linhas[idInf] = superior->linhas[idSuperior];
    // Atualiza LRU no inferior
    inferior->linhas[idInf].contador = (*relogio)++;

    // Invalida a linha no superior (pois o bloco agora está no inferior)
    superior->linhas[idSuperior].endBloco = -1;
    superior->linhas[idSuperior].atualizado = 0;
    for (int j = 0; j < 4; j++) superior->linhas[idSuperior].palavras[j] = 0;
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
    int endBloco = end.endBloco;
    int id;

//tenta na l1
    id = buscarNaCache(ucm->L1, endBloco);
    if(id != -1){
        ucm->hitsL1++;
        atualizarLRU(ucm->L1,id, &ucm->relogio);
        ucm->tempoTotal+=1;
        return &ucm->L1->linhas[id];
    }
    ucm->missesL1++;
//tenta na l2
    id = buscarNaCache(ucm->L2, endBloco);
    if(id != -1){
        ucm->hitsL2++;
        atualizarLRU(ucm->L2,id, &ucm->relogio);
        ucm->tempoTotal+=10;
        //sobe para de l2 para l1 - invalidando em l2
        subirBloco(ucm->L1, endBloco, ucm->L2, id, ucm->ram, &ucm->relogio);
        // busca em l1 e pega o indica
        int idl1 = buscarNaCache( ucm->L1, endBloco);
        return &ucm->L1->linhas[idl1];
    }
    ucm->missesL2++;
// tenta na l3
    id = buscarNaCache(ucm->L3, endBloco);
    if(id != -1){
        ucm->hitsL3++;
        atualizarLRU(ucm->L3,id, &ucm->relogio);
        ucm->tempoTotal+=50;
        //sobe para de l3 para l2 - invalidando em l3
        subirBloco(ucm->L2, endBloco, ucm->L3, id, ucm->ram, &ucm->relogio);
        // busca em l2 e pega o indice
        int idl2 = buscarNaCache( ucm->L2, endBloco);
        //sobe para de l2 para l1 - invalidando em l2
        subirBloco(ucm->L1, endBloco, ucm->L2, id, ucm->ram, &ucm->relogio);
        // busca em l1 e pega o indica
        int idl1 = buscarNaCache( ucm->L1, endBloco);
        return &ucm->L1->linhas[idl1];
    }
    ucm->missesL3++;
//nao achou em nenhuma, busca na RAM
    ucm->tempoTotal +=200;
    BlocoMemoria blocoRAM = getBloco(ucm->ram, endBloco);
    blocoRAM.atualizado = 0;
//coloca o bloco diretamente em L1
    int idl1 = encontrarLRU(ucm->L1, endBloco);
//se a linha em l1 estiver suja, desce p l2
    if(ucm->L1->linhas[idl1].atualizado && ucm->L1->linhas[idl1].endBloco != -1){
        descerBloco(ucm->L1, idl1, ucm->L2, ucm->ram, &ucm->relogio);
// se nao estava sujo, mas estava ocupado, somente esvazia a linha
    } else if(ucm->L1->linhas[idl1].endBloco != -1){
        ucm->L1->linhas[idl1].endBloco = -1;
    }
// coloca o bloco da RAM em l1
    ucm->L1->linhas[idl1] = blocoRAM;
    ucm->L1->linhas[idl1].contador = ucm->relogio++;
    ucm->L1->linhas[idl1].atualizado = 0;

    return &ucm->L1->linhas[idl1];
}

void ucm_escrever(UCM *ucm, Endereco end, int valor){
    int bloco = end.endBloco;
    int offset = end.endPalavra;
    int id;

// tenta na L1
    id = buscarNaCache(ucm->L1, bloco);
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
    BlocoMemoria *bloco_lido = ucm_ler(ucm, end);
    //bloco agora esta em L1, encontramos a linha
    int idl1 = buscarNaCache(ucm->L1, bloco);
    if(idl1 != -1){
        ucm->L1->linhas[idl1].palavras[offset] = valor;
        ucm->L1->linhas[idl1].atualizado = 1;
    } else {
    // Caso extremo: escreve direto na RAM
        BlocoMemoria blocoRAM = getBloco(ucm->ram, bloco);
        blocoRAM.palavras[offset] = valor;
        blocoRAM.atualizado = 1;
        setBloco(ucm->ram, bloco, blocoRAM);
    }

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


