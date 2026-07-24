#include "Disco.h"
#include <stdlib.h>
#include <string.h>

// Inicializa o Disco lendo arquivo gerado
void inicializarDisco(Disco *d, const char *nome, int numBlocos) {
    d->numBlocos = numBlocos;
    d->tamanhoBloco = 4 * sizeof(int);
    d->arquivo = fopen(nome, "rb+");
    if (!d->arquivo) {
        d->arquivo = fopen(nome, "wb+");
        if (!d->arquivo) {
            perror("Erro ao criar disco");
            exit(1);
        }
        // Preenche todo o disco com zeros
        int zero[4] = {0,0,0,0};
        for (int i = 0; i < numBlocos; i++) {
            fwrite(zero, sizeof(int), 4, d->arquivo);
        }

        // Tenta carregar dados do arquivo texto
        FILE *dados = fopen("disco_dados.txt", "r");
        if (dados) {
            int bloco, p0, p1, p2, p3;
            while (fscanf(dados, "%d %d %d %d %d", &bloco, &p0, &p1, &p2, &p3) == 5) {
                if (bloco >= 0 && bloco < numBlocos) {
                    long pos = (long)bloco * d->tamanhoBloco;
                    fseek(d->arquivo, pos, SEEK_SET);
                    int vals[4] = {p0, p1, p2, p3};
                    fwrite(vals, sizeof(int), 4, d->arquivo);
                }
            }
            fclose(dados);
        } else {
            // Se não houver arquivo texto, gera aleatório
            srand(time(NULL));
            int bloco[4];
            for (int i = 0; i < numBlocos; i++) {
                for (int j = 0; j < 4; j++) bloco[j] = rand() % 1000;
                fwrite(bloco, sizeof(int), 4, d->arquivo);
            }
        }
        fflush(d->arquivo);
    }
}

BlocoMemoria lerBlocoDisco(Disco *disco, int endBloco){
//valida se o bloco consegue esta dentro do espaco
    if (endBloco < 0 || endBloco >= disco->numBlocos) {
        fprintf(stderr, "Bloco inválido: %d\n", endBloco);
        exit(1);
    }
//acha posicao movendo o ponteiro a partir do inicio
    long pos = (long)endBloco * disco->tamanhoBloco;
    fseek(disco->arquivo, pos, SEEK_SET);

//Seta os parametros restantes que nao ficam no arquivo
    BlocoMemoria bloco;
    bloco.endBloco = endBloco;
    bloco.atualizado = 0;
    bloco.custo = 0;
    bloco.cacheHit = 5; // veio do disco
//le o bloco do arquivo propriamente dito
    fread(bloco.palavras, sizeof(int), 4, disco->arquivo);


    return bloco;
}


void escreverBlocoDisco(Disco *disco, BlocoMemoria* bloco){
//VERIFICACAO SE ESTA NO ESPACO ADEQUADO
    if (bloco->endBloco < 0 || bloco->endBloco >= disco->numBlocos) return;
//acha posicao movendo o ponteiro a partir do inicio
    long pos = (long)bloco->endBloco * disco->tamanhoBloco;
    fseek(disco->arquivo, pos, SEEK_SET);
//escreve no bloco
    fwrite(bloco->palavras, sizeof(int), 4, disco->arquivo);
    fflush(disco->arquivo);
}

void fecharDisco(Disco *disco){
    if(disco->arquivo)
        fclose(disco->arquivo);
}
