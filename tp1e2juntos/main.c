#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "RAM.h"
#include "CPU.h"
#include "Instrucao.h"

Instrucao* lerArquivoInstrucoes(char *nomeArquivo, int *qtdInstrucoes);

int main(){
    srand(time(NULL));

    CPU *cpu = alocaCPU();
    Ram *ram = alocarEstruturaRam();

    int TamanhoRAM = 1000;
    criarRamAleat(ram, TamanhoRAM);

    //leitura do arquivo.txt
    int qtIntrucoes = 0;
    Instrucao *programaDoGerador = lerArquivoInstrucoes("instructions2.txt", &qtIntrucoes);

    setPrograma(cpu, programaDoGerador);

    printf("CPU INICIANDO...\n\n");
    iniciarCPU(cpu, ram, 16, 32);

    printf("FIM");

    free(programaDoGerador);
    liberarRam(ram);
    free(ram);
    liberaCPU(cpu);

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