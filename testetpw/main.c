#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "RAM.h"
#include "CPU.h"
#include "Instrucao.h"
#include "UCM.h"

Instrucao* lerArquivoInstrucoes(char *nomeArquivo, int *qtdInstrucoes);

int main(int argc, char *argv[]) {
    // Parâmetros padrão
    int tamL1 = 16;
    int tamL2 = 64;
    int tamL3 = 256;
    int viasL2 = 4;
    int viasL3 = 8;
    char *instFile = "instructions2.txt";

    // Se receber argumentos: ./tp2 <tamL1> <tamL2> <tamL3> [arquivo_instrucoes]
    if (argc >= 4) {
        tamL1 = atoi(argv[1]);
        tamL2 = atoi(argv[2]);
        tamL3 = atoi(argv[3]);
        if (argc >= 5) instFile = argv[4];
    }

    srand(time(NULL));

    // Cria RAM
    Ram *ram = alocarEstruturaRam();
    criarRamAleat(ram, 1000);

    // Inicializa caches
    Cache L1, L2, L3;
    inicializarCache(&L1, 1, tamL1);                // L1 totalmente associativa
    inicializarCache(&L2, tamL2 / viasL2, viasL2);  // L2 set-associativa
    inicializarCache(&L3, tamL3 / viasL3, viasL3);  // L3 set-associativa

    // UCM
    UCM ucm;
    inicializarUCM(&ucm, &L1, &L2, &L3, ram);

    // Lê instruções
    int qtInstrucoes;
    Instrucao *programa = lerArquivoInstrucoes(instFile, &qtInstrucoes);

    // CPU
    CPU *cpu = alocaCPU();
    setPrograma(cpu, programa);
    cpu->ucm = &ucm;

    iniciarCPU(cpu);

    // Imprime estatísticas no formato parseável
    int total = ucm.hitsL1 + ucm.missesL1 + ucm.hitsL2 + ucm.missesL2 + ucm.hitsL3 + ucm.missesL3 + 1; // +1 para evitar divisão por zero
    // Mas vamos usar totais por nível
    int totL1 = ucm.hitsL1 + ucm.missesL1;
    int totL2 = ucm.hitsL2 + ucm.missesL2;
    int totL3 = ucm.hitsL3 + ucm.missesL3;
    // Acesso à RAM quando miss em L3
    int ramAcessos = ucm.missesL3; // cada miss em L3 gera um acesso à RAM
    // Total de acessos = totL1 (ou totL2, tanto faz, pois todos os acessos passam por L1)
    int totalAcessos = totL1;

    // Formato: HIT_STATS:l1_hits,l1_misses,l2_hits,l2_misses,l3_hits,l3_misses,ram_acessos,total_acessos
    printf("HIT_STATS:%d,%d,%d,%d,%d,%d,%d,%d\n",
           ucm.hitsL1, ucm.missesL1,
           ucm.hitsL2, ucm.missesL2,
           ucm.hitsL3, ucm.missesL3,
           ramAcessos, totalAcessos);

    // Também imprime a tabela bonita (opcional)
    imprimirEstatisticas(&ucm);

    // Libera
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