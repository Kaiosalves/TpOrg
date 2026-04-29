#ifndef RAM_H
#define RAM_H

typedef struct RAM Ram;

//cria Ram base
void criarRam (Ram *r, int tamanho);

//cria ram preenchida com 0
void criarRamVazia (Ram *r, int tamanho);

//cria ram e preenche com numeros aleatorios
void criarRamAleat  (Ram *r, int tamanho);

//libera a memoria ram
void liberarRam(Ram *r);

//funcoes de passar e pegar dados
int setDado(Ram *r, int endereco, int conteudo);

int getDado(Ram *r, int endereco);

//imprimir conteudo da Ram
void imprimirConteudoRam(Ram* r);

#endif