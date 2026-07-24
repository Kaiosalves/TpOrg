#include "RAM.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

struct RAM{
    int tamanho;
    BlocoMemoria *blocos;
    Disco *disco;
};

//estrutura base de ram
Ram* alocarEstruturaRam() {
    return (Ram*)malloc(sizeof(struct RAM));
}
//cria Ram vazia
void criarRam(Ram *r, Disco *disco, int tamanho) {
    r->tamanho = tamanho;
    r->disco = disco;
    r->blocos = (BlocoMemoria*)malloc(tamanho * sizeof(BlocoMemoria));
    for(int i = 0; i < tamanho; i++)
    {
        r->blocos[i].endBloco = i;

        r->blocos[i].atualizado = 0;
        r->blocos[i].custo = 0;
        r->blocos[i].cacheHit = 0;

        memset(r->blocos[i].palavras,0,sizeof(int) * 4);
    }
}

//libera a memoria ram
void liberarRam(Ram *r){
    if(r->blocos)
        free(r->blocos);
}

BlocoMemoria* ram_get_bloco(Ram *r, int endBloco, int *veio_do_disco){
    int pos = endBloco % r->tamanho;
    BlocoMemoria *bloco = &r->blocos[pos];
//deu hit na RAM
     if (bloco->endBloco == endBloco) {
        *veio_do_disco = 0;
        bloco->cacheHit = 4; // hit na RAM
        return bloco;
    }

// Miss: substituir (FLAG DIRTY)
    if (bloco->atualizado && bloco->endBloco != -1) {
        escreverBlocoDisco(r->disco, bloco);
    }

// le do disco
    BlocoMemoria novo = lerBlocoDisco(r->disco, endBloco);
    novo.cacheHit = 5; // veio do disco
    novo.atualizado = 0;
    *bloco = novo;
    *veio_do_disco = 1;

    return bloco;
}


void ram_escrever_bloco(Ram *r, BlocoMemoria *bloco){
//acha posicao
    int pos = bloco->endBloco % r->tamanho;
//se achou a posicao, escreve e atualiza flag
    if (r->blocos[pos].endBloco == bloco->endBloco) {
        r->blocos[pos] = *bloco;
        r->blocos[pos].atualizado = 1;
    //se nao achou a posicao
    } else {
        // Insere (substitui) no disco e atualiza
        if (r->blocos[pos].atualizado && r->blocos[pos].endBloco != -1) {
            escreverBlocoDisco(r->disco, &r->blocos[pos]);
        }
        r->blocos[pos] = *bloco;
        r->blocos[pos].atualizado = 1;
    }
}

//imprimir conteudo da Ram
void imprimirConteudoRam(Ram* r) {
    printf("Conteúdo da RAM (direto, tam %d)\n", r->tamanho);
    for (int i = 0; i < r->tamanho; i++) {
        if (r->blocos[i].endBloco != -1) {
            printf("Pos %d -> Bloco %d: [%d %d %d %d]\n", i, r->blocos[i].endBloco,
                   r->blocos[i].palavras[0], r->blocos[i].palavras[1],
                   r->blocos[i].palavras[2], r->blocos[i].palavras[3]);
        }
    }
}