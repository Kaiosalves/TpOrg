#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "RAM.h"
#include "CPU.h"
#include "Instrucao.h"
#include "UCM.h"

Instrucao* lerArquivoInstrucoes(char *nomeArquivo, int *qtdInstrucoes);

int main(){
    // Cria RAM
    Ram *ram = alocarEstruturaRam();
    int tamanhoRAM = 1000;
    criarRamAleat(ram, tamanhoRAM);

    // Definição das caches
    int tamL1 = 16;          // ex: 16 vias, 1 conjunto
    int tamL2 = 64;          // ex: 4 vias, 16 conjuntos
    int tamL3 = 256;         // ex: 8 vias, 32 conjuntos

    Cache L1, L2, L3;
    inicializarCache(&L1, 1, tamL1);                // L1: 1 via
    inicializarCache(&L2, tamL2 / 4, 4);            // L2: 4 vias
    inicializarCache(&L3, tamL3 / 8, 8);            // L3: 8 vias

    // Inicializa UCM
    UCM ucm;
    inicializarUCM(&ucm, &L1, &L2, &L3, ram);

    // Lê instruções
    int qtInstrucoes;
    Instrucao *programa = lerArquivoInstrucoes("instructions2.txt", &qtInstrucoes);

    // Cria CPU e associa programa e UCM
    CPU *cpu = alocaCPU();
    setPrograma(cpu, programa);
    setUCM(cpu, &ucm);

    printf("CPU INICIANDO...\n\n");
    iniciarCPU(cpu);

    // Imprime estatísticas
    imprimirEstatisticas(&ucm);

    // Libera memória
    free(programa);
    liberarRam(ram);
    free(ram);
    liberarCache(&L1);
    liberarCache(&L2);
    liberarCache(&L3);
    liberaCPU(cpu);

    return 0;
}






Instrucao* lerArquivoInstrucoes(char *nomeArquivo, int *qtdInstrucoes){
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de instrucoes: %s\n", nomeArquivo);
        exit(1);
    }

    int linhas = 0;
    char ch;
    while(!feof(arquivo)) {
        ch = fgetc(arquivo);
        if(ch == '\n') {
            linhas++;
        }
    }
    
    int tamanhoVetor = linhas + 1;
    
    Instrucao *programa = (Instrucao*) malloc(tamanhoVetor * sizeof(Instrucao));
    if (programa == NULL) {
        printf("Erro de memoria ao alocar o programa.\n");
        exit(1);
    }

    rewind(arquivo);

    int i = 0;
    int op, b1, p1, b2, p2, b3, p3;

    // Lendo o formato: opcode:bloco1:palavra1:bloco2:palavra2:bloco3:palavra3
    while (fscanf(arquivo, "%d:%d:%d:%d:%d:%d:%d\n", &op, &b1, &p1, &b2, &p2, &b3, &p3) == 7) {
        programa[i].opcode = op;
        
        programa[i].add1.endBloco = b1;
        programa[i].add1.endPalavra = p1;
        
        programa[i].add2.endBloco = b2;
        programa[i].add2.endPalavra = p2;
        
        programa[i].add3.endBloco = b3;
        programa[i].add3.endPalavra = p3;
        
        i++;
    }

    // Inserindo manualmente a última instrução de parada (Halt)
    programa[i].opcode = -1;
    programa[i].add1.endBloco = -1;  programa[i].add1.endPalavra = -1;
    programa[i].add2.endBloco = -1;  programa[i].add2.endPalavra = -1;
    programa[i].add3.endBloco = -1;  programa[i].add3.endPalavra = -1;

    fclose(arquivo);
    
    *qtdInstrucoes = i + 1; 
    return programa;
}