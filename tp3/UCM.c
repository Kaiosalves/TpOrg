#include "UCM.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constantes de tempo 
#define TEMPO_L1 1
#define TEMPO_L2 10
#define TEMPO_L3 50
#define TEMPO_RAM 100
#define TEMPO_DISCO 100000

// Desce um bloco de um nível superior para o inferior
static void descerBloco(Cache *superior, int idSuperior, Cache *inferior, Cache *proximo, Ram *ram, int *relogio) {
    if (superior->linhas[idSuperior].endBloco == -1) return;
    
    int endBloco = superior->linhas[idSuperior].endBloco;
    int idInf = encontrarLRU(inferior, endBloco);
    
    // Se a linha no inferior estiver ocupada, desce para o próximo nível
    if (inferior->linhas[idInf].endBloco != -1) {
        if (proximo != NULL) {// se esta na l2
            // cascata para o próximo nível
            descerBloco(inferior, idInf, proximo, NULL, ram, relogio);
        } else {
            // Último nível: escreve na RAM se sujo e invalida
            if (inferior->linhas[idInf].atualizado) {
                ram_escrever_bloco(ram, &inferior->linhas[idInf]);
            }
            inferior->linhas[idInf].endBloco = -1;
            inferior->linhas[idInf].atualizado = 0;
            memset(inferior->linhas[idInf].palavras, 0, sizeof(int)*4);
        }
    }
    
    // Agora coloca o bloco do superior no inferior
    inferior->linhas[idInf] = superior->linhas[idSuperior];
    inferior->linhas[idInf].contador = (*relogio)++;
    
    // Invalida o bloco no superior (exclusividade)
    superior->linhas[idSuperior].endBloco = -1;
    superior->linhas[idSuperior].atualizado = 0;
    memset(superior->linhas[idSuperior].palavras, 0, sizeof(int)*4);
}



static void promover_para_L1(UCM *ucm, int endBloco, Cache *origem, int idOrig) {
    int idL1 = encontrarLRU(ucm->L1, endBloco);
    if (ucm->L1->linhas[idL1].endBloco != -1) {
        descerBloco(ucm->L1, idL1, ucm->L2, ucm->L3, ucm->ram, &ucm->relogio);
    }

    ucm->L1->linhas[idL1] = origem->linhas[idOrig];
    ucm->L1->linhas[idL1].contador = ucm->relogio++;

// invalida na origem - politica de exclusividade
    origem->linhas[idOrig].endBloco = -1;
    origem->linhas[idOrig].atualizado = 0;
    memset(origem->linhas[idOrig].palavras, 0, sizeof(int)*4);

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
    ucm->hitsRAM = ucm->missesRAM = 0;
    ucm->acessosDisco = 0;
    ucm->tempoTotal = 0;
}





BlocoMemoria* ucm_ler(UCM *ucm, Endereco end, int *veio_do_disco){
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
// Tenta L2 
    id = buscarNaCache(ucm->L2, endBloco);
    if (id != -1) {
        ucm->hitsL2++;
        atualizarLRU(ucm->L2, id, &ucm->relogio);
        ucm->tempoTotal += TEMPO_L2;
    // Promove de L2 para L1, invalidando em L2
        promover_para_L1(ucm, endBloco, ucm->L2, id);
        int idL1 = buscarNaCache(ucm->L1, endBloco);
        *veio_do_disco = 0;
        return &ucm->L1->linhas[idL1];
    }
    ucm->missesL2++;
//Tenta L3
    id = buscarNaCache(ucm->L3, endBloco);
    if (id != -1) {
        ucm->hitsL3++;
        atualizarLRU(ucm->L3, id, &ucm->relogio);
        ucm->tempoTotal += TEMPO_L3;
// Move de L3 para L2 
        int idL2 = encontrarLRU(ucm->L2, endBloco);
        if (ucm->L2->linhas[idL2].endBloco != -1) {
    // Desce o bloco removido de L2 para L3 (cascata, próximo = NULL porque L3 é o último)
        descerBloco(ucm->L2, idL2, ucm->L3, NULL, ucm->ram, &ucm->relogio);
    }
// Copia o bloco de L3 para L2
        ucm->L2->linhas[idL2] = ucm->L3->linhas[id];
        ucm->L2->linhas[idL2].contador = ucm->relogio++;
        ucm->L2->linhas[idL2].atualizado = 0;

// Invalida em L3
        ucm->L3->linhas[id].endBloco = -1;
        ucm->L3->linhas[id].atualizado = 0;
        memset(ucm->L3->linhas[id].palavras, 0, sizeof(int)*4);

// promove de l2 para l1
        int idL2novo = buscarNaCache(ucm->L2, endBloco);
        promover_para_L1(ucm, endBloco, ucm->L2, idL2novo);
//retorna info de l1
        int idL1 = buscarNaCache(ucm->L1, endBloco);

        *veio_do_disco = 0;
        return &ucm->L1->linhas[idL1];
    }
    ucm->missesL3++;

// Miss em l3: busca na RAM
    int veio_disco = 0;
    BlocoMemoria *blocoRAM = ram_get_bloco(ucm->ram, endBloco, &veio_disco);
     if (blocoRAM == NULL) {
        // Caso raro: se a RAM falhar, retorna NULL para o chamador tratar
        *veio_do_disco = 0;
        return NULL;
     }
    if (veio_disco) {
        ucm->missesRAM++;
        ucm->acessosDisco++;
        ucm->tempoTotal += TEMPO_DISCO;
        *veio_do_disco = 1;
    } else {
        ucm->hitsRAM++;
        ucm->tempoTotal += TEMPO_RAM;
        *veio_do_disco = 0;
    }
//Agora o bloco esta na RAM. copia Para l3, l2 l1
    BlocoMemoria blocoCopia = *blocoRAM;
    blocoCopia.atualizado = 0;

// PASSO 1: Coloca o bloco da RAM na L3 (substituindo LRU)
    int idL3 = encontrarLRU(ucm->L3, endBloco);
// Se a linha em L3 estiver ocupada, trata write-back
    if (ucm->L3->linhas[idL3].atualizado && ucm->L3->linhas[idL3].endBloco != -1) {
        ram_escrever_bloco(ucm->ram, &ucm->L3->linhas[idL3]);
    }
    ucm->L3->linhas[idL3] = blocoCopia;
    ucm->L3->linhas[idL3].contador = ucm->relogio++;
    ucm->L3->linhas[idL3].atualizado = 0;


// PASSO 2: Sobe da L3 para a L2 (exclusividade: invalida L3)
    int idL2 = encontrarLRU(ucm->L2, endBloco);
// Se a linha em L2 estiver ocupada, desce para L3 (cascata)
    if (ucm->L2->linhas[idL2].endBloco != -1) {
        descerBloco(ucm->L2, idL2, ucm->L3, NULL, ucm->ram, &ucm->relogio);
    }
// Copia o bloco da L3 para L2
    ucm->L2->linhas[idL2] = ucm->L3->linhas[idL3];
    ucm->L2->linhas[idL2].contador = ucm->relogio++;
    ucm->L2->linhas[idL2].atualizado = 0;

// Invalida na L3 (exclusividade)
    ucm->L3->linhas[idL3].endBloco = -1;
    ucm->L3->linhas[idL3].atualizado = 0;
    memset(ucm->L3->linhas[idL3].palavras, 0, sizeof(int)*4);


//Sobe da L2 para a L1

    promover_para_L1(ucm, endBloco, ucm->L2, idL2);
// Retorna o bloco que agora está em L1
    int idL1 = buscarNaCache(ucm->L1, endBloco);
    return &ucm->L1->linhas[idL1];
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
        ucm->tempoTotal +=TEMPO_L1;
        ucm->L1->linhas[id].palavras[offset] = valor;
        ucm->L1->linhas[id].atualizado = 1; // marcado como sujo
        return;
    }

//se nao esta em L1, trazemos o bloco (leitura) depois escrevemos
    int veio_disco = 0;
    BlocoMemoria *bloco_lido = ucm_ler(ucm, end, &veio_disco);
    if (bloco_lido == NULL ) return;
    //bloco agora esta em L1, encontramos a linha
    int idl1 = buscarNaCache(ucm->L1, bloco);
    if(idl1 != -1){
        ucm->L1->linhas[idl1].palavras[offset] = valor;
        ucm->L1->linhas[idl1].atualizado = 1;
    } else {
    // Fallback: escreve direto na RAM
        BlocoMemoria blocoRAM = {0};
        blocoRAM.endBloco = bloco;
        blocoRAM.palavras[offset] = valor;
        blocoRAM.atualizado = 1;
        ram_escrever_bloco(ucm->ram, &blocoRAM);
    }

}

void imprimirEstatisticas(UCM *ucm){
    printf("\n---------- ESTATÍSTICAS DA UCM -----------\n");
    int totalL1 = ucm->hitsL1 + ucm->missesL1;
    int totalL2 = ucm->hitsL2 + ucm->missesL2;
    int totalL3 = ucm->hitsL3 + ucm->missesL3;
    int totalRAM = ucm->hitsRAM + ucm->missesRAM;
    int totalAcessos = totalL1;

    printf("L1 - Hits: %d, Misses: %d, Taxa: %.2f%%\n",
           ucm->hitsL1, ucm->missesL1,
           totalL1 ? (100.0 * ucm->hitsL1 / totalL1) : 0.0);
    printf("L2 - Hits: %d, Misses: %d, Taxa: %.2f%%\n",
           ucm->hitsL2, ucm->missesL2,
           totalL2 ? (100.0 * ucm->hitsL2 / totalL2) : 0.0);
    printf("L3 - Hits: %d, Misses: %d, Taxa: %.2f%%\n",
           ucm->hitsL3, ucm->missesL3,
           totalL3 ? (100.0 * ucm->hitsL3 / totalL3) : 0.0);
    printf("RAM - Hits: %d, Misses: %d, Taxa: %.2f%%\n",
           ucm->hitsRAM, ucm->missesRAM,
           totalRAM ? (100.0 * ucm->hitsRAM / totalRAM) : 0.0);
    printf("Acessos ao disco: %d (%.2f%% do total)\n",
           ucm->acessosDisco,
           totalAcessos ? (100.0 * ucm->acessosDisco / totalAcessos) : 0.0);
    printf("Tempo total estimado: %lld ciclos\n", ucm->tempoTotal);
}


