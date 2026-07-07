
#include "Instrucao.h"
#include "CPU.h"
#include "UCM.h"
#include <stdlib.h>
#include <stdio.h>

CPU* alocaCPU(){
    CPU *cpu = (CPU*) malloc(sizeof(CPU));
    cpu->registrador1 = NULL;
    cpu->registrador2 = NULL;
    cpu->registrador3 = NULL;
    cpu->PC = 0;
    cpu->opcode = 0x7FFFFFFF; // maior inteiro em hexadecimal
    cpu->programa = NULL;
    cpu->ucm = NULL;
    return cpu;

}

//seta um vetor de instrucoes auxiliares
void setPrograma(CPU* cpu, Instrucao* programaAux){
    cpu->programa = programaAux;
}
/**
 * @brief 
 * -1 --> Parada ;
 * 0 --> soma ;
 * 1 --> subtrai ;
 * 2 --> Registrador para RAM ;
 * 3 --> RAM para registrador ;
 * @param cpu 
 * @param ram 
 */
void iniciarCPU(CPU* cpu){
    if(!cpu->ucm){
        printf("ERRO: UCM NAO ASSOCIADA A CPU\n");
        return;
    }
    cpu->opcode =0;
    cpu->PC = 0;

    while (cpu->opcode != -1){
        Instrucao inst = cpu->programa[cpu->PC];
        cpu->opcode = inst.opcode;
        
        switch(cpu->opcode){
            case -1: // halt
                break;  

            case 0: { // SOMA
                // Lê os operandos
                BlocoMemoria *reg1 = ucm_ler(cpu->ucm, inst.add1);
                BlocoMemoria *reg2 = ucm_ler(cpu->ucm, inst.add2);
                int resultado = reg1->palavras[inst.add1.endPalavra] + reg2->palavras[inst.add2.endPalavra];
                // Escreve o resultado (write-back)
                ucm_escrever(cpu->ucm, inst.add3, resultado);
        // imprime resultado
                printf("Instrução de soma -> resultado %d escrito no bloco %d, palavra %d\n",resultado, inst.add3.endBloco, inst.add3.endPalavra);
                break;
            }

            case 1: { // SUBTRAÇÃO
                BlocoMemoria *reg1 = ucm_ler(cpu->ucm, inst.add1);
                BlocoMemoria *reg2 = ucm_ler(cpu->ucm, inst.add2);
                int resultado = reg1->palavras[inst.add1.endPalavra] - reg2->palavras[inst.add2.endPalavra];
                ucm_escrever(cpu->ucm, inst.add3, resultado);
            //imprime resultado
                printf("Instrução de subtração -> resultado %d escrito no bloco %d, palavra %d\n",resultado, inst.add3.endBloco, inst.add3.endPalavra);
                break;
            }

            default:
                printf("Instrução desconhecida\n");
                break;
        }
        cpu->PC++;
    }
}


//liberar memoria
void liberaCPU(CPU* cpu){
    if (cpu) {
        free(cpu);
    }
}