#ifndef RAM_H
#define RAM_H
#include "BlocoMemoria.h"

typedef struct RAM Ram;

Ram* alocarEstruturaRam();
//cria Ram base
void criarRam (Ram *r, int tamanho);

//cria ram preenchida com 0
void criarRamVazia (Ram *r, int tamanho);

//cria ram e preenche com numeros aleatorios
void criarRamAleat  (Ram *r, int tamanho);

//libera a memoria ram
void liberarRam(Ram *r);

//funcoes de passar e pegar dados
void setBloco(Ram *r, int endereco, BlocoMemoria bloco);

BlocoMemoria getBloco( Ram *r, int endereco);

//imprimir conteudo da Ram
void imprimirConteudoRam(Ram* r);

#endif