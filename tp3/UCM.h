#ifndef UCM_H
#define UCM_H

#include "cache.h"
#include "RAM.h"
#include "Endereco.h"

typedef struct {
    Cache *L1;
    Cache *L2;
    Cache *L3;
    Ram *ram;
    int relogio;          // contador global para LRU
    // Estatísticas
    int hitsL1, missesL1;
    int hitsL2, missesL2;
    int hitsL3, missesL3;
    int hitsRAM, missesRAM;
    int acessosDisco;
    long long tempoTotal; // soma dos custos
} UCM;
void inicializarUCM(UCM *ucm, Cache *L1, Cache *L2, Cache *L3, Ram *ram);

// Leitura de uma palavra da memória 
BlocoMemoria* ucm_ler(UCM *ucm, Endereco end, int *veio_do_disco);

// Escrita de uma palavra (write-back)
void ucm_escrever(UCM *ucm, Endereco end, int valor);

// Imprime as estatísticas 
void imprimirEstatisticas(UCM *ucm);



#endif