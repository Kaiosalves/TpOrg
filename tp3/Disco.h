#ifndef DISCO_H
#define DISCO_H

#include <stdio.h>
#include <time.h>
#include "BlocoMemoria.h"


typedef struct {
    FILE *arquivo;
    int numBlocos;
    int tamanhoBloco;
}Disco;

void inicializarDisco(Disco *d, const char *nome, int numBlocos);

BlocoMemoria lerBlocoDisco(Disco *disco, int endBloco);

void escreverBlocoDisco(Disco *disco, BlocoMemoria* bloco);

void fecharDisco(Disco *disco);

#endif