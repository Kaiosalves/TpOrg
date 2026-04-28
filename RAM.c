#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct 
{
int tamanho;
int *memoria
}Ram;

//cria Ram base
void criarRam (Ram *r, int tamanho){
    r->tamanho = tamanho;
    r->memoria = (int*) malloc(tamanho * sizeof(int));
}
//cria ram preenchida com 0
void criarRamVazia (Ram *r, int tamanho){
    r->tamanho = tamanho;
    r->memoria = (int*) calloc(tamanho, sizeof(int));
}
//cria ram e preenche com numeros aleatorios
void criarRamAleat  (Ram *r, int tamanho){
    criarRam(r, tamanho);
    srand(time(NULL));
    for(int i = 0; i < tamanho; i++){
        r->memoria[i] = rand();
    }
}
//libera a memoria ram
void liberarRam(Ram *r){
    free(r->memoria);
}
//funcoes de passar e pegar dados
int setDado(Ram *r, int endereco, int conteudo){
    return r->memoria[endereco] = conteudo;
}
int getDado(Ram *r, int endereco){
    return r->memoria[endereco];
}
//imprimir conteudo da Ram
void Imprimir(Ram* r){
    printf("Conteudo da RAM\n");
        for(int i = 0; i < r->tamanho; i++){
            printf("%d,", r->memoria[i]);
    }
    printf("\n");
}