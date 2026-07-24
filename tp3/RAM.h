#ifndef RAM_H
#define RAM_H
#include "BlocoMemoria.h"
#include "Disco.h"


typedef struct RAM Ram;

Ram* alocarEstruturaRam();
//cria Ram base
void criarRam(Ram *r, Disco *disco, int tamanho);
//libera a memoria ram
void liberarRam(Ram *r);

// Busca um bloco na RAM; se não estiver, carrega do disco (substituição direta).
// Retorna o bloco e preenche veio_do_disco (1 se leu do disco, 0 se já estava).
BlocoMemoria* ram_get_bloco(Ram *r, int endBloco, int *veio_do_disco);


void ram_escrever_bloco(Ram *r, BlocoMemoria *bloco);

//imprimir conteudo da Ram
void imprimirConteudoRam(Ram* r);

#endif