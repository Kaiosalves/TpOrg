#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "RAM.h"
#include "CPU.h"
#include "UCM.h"
#include "Disco.h"
#include "Programa.h"

Instrucao* lerArquivoInstrucoes(char *nomeArquivo, int *qtdInstrucoes);

int main(int argc, char *argv[]) {
    // Parâmetros padrão
    int tamL1 = 16, tamL2 = 32, tamL3 = 64;
    int viasL2 = 4, viasL3 = 8;
    int tamRAM = 64;
    int tamDisco = 10000;
    char *instFile = "programa.txt";
    char *tratadorFile = "tratador_interrupcao.txt";


    if (argc >= 4) {
        tamL1 = atoi(argv[1]);
        tamL2 = atoi(argv[2]);
        tamL3 = atoi(argv[3]);
        if (argc >= 5) tamRAM = atoi(argv[4]);
        if (argc >= 6) instFile = argv[5];
        if (argc >= 7) tratadorFile = argv[6];
    }

    srand(time(NULL));

    //Inicializa o disco
    Disco disco;
    inicializarDisco(&disco, "disco.bin", tamDisco);

    //Inicializa a RAM
    Ram *ram = alocarEstruturaRam();
    criarRam(ram, &disco, tamRAM);

    //Inicializa as caches
    Cache L1, L2, L3;
    inicializarCache(&L1, 1, tamL1);
    inicializarCache(&L2, tamL2 / viasL2, viasL2);
    inicializarCache(&L3, tamL3 / viasL3, viasL3);

    //Inicializa a UCM
    UCM ucm;
    inicializarUCM(&ucm, &L1, &L2, &L3, ram);

    //Inicializa a CPU
    CPU *cpu = alocaCPU();
    cpu->ucm = &ucm;

    //Carrega o programa principal
    int qtInst;
    Instrucao *progInst = lerArquivoInstrucoes(instFile, &qtInst);
    Programa *p1 = criar_programa(progInst, qtInst, "Principal");
    set_programa(cpu, p1);

    //Carrega o tratador de interrupção
    int qtTrat;
    Instrucao *tratInst = lerArquivoInstrucoes(tratadorFile, &qtTrat);
    Programa *tratador = criar_programa(tratInst, qtTrat, "Tratador");
    set_tratador(cpu, tratador);

    //Executa a CPU
    executar_cpu(cpu, 1000000);

    // Estatísticas finais
    printf("\n--- ESTATÍSTICAS FINAIS ---\n");
    imprimirEstatisticas(&ucm);

    // 10. Libera recursos
    liberarRam(ram);
    free(ram);
    liberarCache(&L1);
    liberarCache(&L2);
    liberarCache(&L3);
    fecharDisco(&disco);
    liberaCPU(cpu);

    return 0;
}

Instrucao* lerArquivoInstrucoes(char *nomeArquivo, int *qtdInstrucoes) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo: %s\n", nomeArquivo);
        exit(1);
    }
    
    int linhas = 0;
    char ch;
    while (!feof(arquivo)) {
        ch = fgetc(arquivo);
        if (ch == '\n') linhas++;
    }
    rewind(arquivo);

    Instrucao *programa = (Instrucao*)malloc((linhas + 1) * sizeof(Instrucao));
    int i = 0, op, b1, p1, b2, p2, b3, p3;
    
    while (fscanf(arquivo, "%d:%d:%d:%d:%d:%d:%d\n", &op, &b1, &p1, &b2, &p2, &b3, &p3) == 7) {
        programa[i].opcode = op;
        programa[i].add1.endBloco = b1; programa[i].add1.endPalavra = p1 % 4;
        programa[i].add2.endBloco = b2; programa[i].add2.endPalavra = p2 % 4;
        programa[i].add3.endBloco = b3; programa[i].add3.endPalavra = p3 % 4;
        i++;
    }
    
    programa[i].opcode = -1;
    fclose(arquivo);
    
    *qtdInstrucoes = i + 1;
    return programa;
}
