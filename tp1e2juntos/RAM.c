#include "RAM.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>



struct RAM{
    int tamanho;
    BlocoMemoria *memoria;
};

//estrutura base de ram
Ram* alocarEstruturaRam() {
    return (Ram*)malloc(sizeof(struct RAM));
}
//cria Ram base
void criarRam (Ram *r, int tamanho){
    r->tamanho = tamanho;
    r->memoria = (BlocoMemoria*)malloc(tamanho * sizeof(BlocoMemoria));
    for(int i = 0; i < tamanho; i++)
    {
        r->memoria[i].endBloco = i;

        r->memoria[i].atualizado = 0;
        r->memoria[i].custo = 0;
        r->memoria[i].cacheHit = 0;
        r->memoria[i].frequencia = 0;

        for(int j = 0; j < 4; j++)
        {
            r->memoria[i].palavras[j] = 0;
        }
    }
}

//cria ram preenchida com 0
void criarRamVazia (Ram *r, int tamanho){
   criarRam(r, tamanho);
}

//cria ram e preenche com numeros aleatorios
void criarRamAleat(Ram *r, int tamanho){
    criarRam(r, tamanho);
    srand(time(NULL));
    for(int i = 0; i < tamanho; i++){
        for(int j = 0; j < 4; j++){
            r->memoria[i].palavras[j] = rand() %1000;

        }
    }
}
//libera a memoria ram
void liberarRam(Ram *r){
    free(r->memoria);
}



//funcoes de passar e pegar dados
int setDado(Ram *r, int endereco, int conteudo){
    return r->memoria[endereco].palavras[0] = conteudo;
}
int getDado(Ram *r, int endereco){
    return r->memoria[endereco].palavras[0];
}

void setBloco(Ram *r, int endereco, BlocoMemoria bloco){
    r->memoria[endereco] = bloco;
}

BlocoMemoria getBloco( Ram *r, int endereco){
    return r->memoria[endereco];
}

//imprimir conteudo da Ram
void ImprimirConteudoRam(Ram* r){
    printf("Conteudo da RAM\n");
        for(int i = 0; i < r->tamanho; i++){
            printf("Bloco %d -> [%d %d %d %d]\n",i,

            r->memoria[i].palavras[0],

            r->memoria[i].palavras[1],

            r->memoria[i].palavras[2],

            r->memoria[i].palavras[3]
        );
    }
    printf("\n");
}