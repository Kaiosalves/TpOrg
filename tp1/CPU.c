
#include "Instrucao.h"
#include "RAM.h"
#include "CPU.h"
#include <stdlib.h>
#include <stdio.h>

struct CPU{
    int registrador1;
    int registrador2;
    int PC; // program counter
    int opcode;
    Instrucao *programa;
};

CPU* alocaCPU(){
    CPU *cpu = (CPU*) malloc(sizeof(CPU));
    cpu->registrador1 = 0;
    cpu->registrador2 = 0;
    cpu->PC = 0;
    cpu->opcode = 0x7FFFFFFF; // maior inteiro em hexadecimal
    cpu->programa = NULL;
    return cpu;

}

//seta um vetor de instrucoes auxiliares
void setPrograma(CPU* cpu, Instrucao* programaAux){
    cpu->programa = programaAux;
}

//passar e pegar valor dos registradores
void setRegistrador1(CPU *cpu, int valor) {
    cpu->registrador1 = valor;
}

int getRegistrador1(CPU *cpu) {
    return cpu->registrador1;
}

void setRegistrador2(CPU *cpu, int valor) {
    cpu->registrador2 = valor;
}

int getRegistrador2(CPU *cpu) {
    return cpu->registrador2;
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
void iniciarCPU(CPU* cpu, Ram* ram){
    cpu->opcode = 0x7FFFFFFF;
    cpu->PC = 0;

    while (cpu->opcode != -1){
        Instrucao inst = cpu->programa[cpu->PC];
        cpu->opcode = inst.opcode;
        
        switch(cpu->opcode){
            case -1:
                break;  

            case 0:
                cpu->registrador1 = getDado(ram, inst.add1);
                cpu->registrador2 = getDado(ram, inst.add2);
                cpu->registrador1 += cpu->registrador2;
                setDado(ram,inst.add3, cpu->registrador1);
                printf("Soma: Ram[%d], tem valor %d\n", inst.add3, cpu->registrador1);
                break;

            case 1: 
                cpu->registrador1 = getDado(ram, inst.add1);
                cpu->registrador2 = getDado(ram, inst.add2);
                cpu->registrador1 -= cpu->registrador2;
                setDado(ram,inst.add3, cpu->registrador1);
                printf("Subtracao: Ram[%d], tem valor %d \n", inst.add3, cpu->registrador1);
                break;

            case 2:
                if(inst.add1 == 1){
                    setDado(ram, inst.add2,cpu->registrador1);
                    printf("Copiado do Registrador para a Ram, na posicao %d de valor: %d\n", inst.add2, cpu->registrador1);
                }
                else if(inst.add1 == 2){
                    setDado(ram, inst.add2,cpu->registrador2);
                    printf("Copiado do Registrador para a Ram, na posicao %d de valor: %d\n", inst.add2, cpu->registrador2);
                break;
                }

            case 3:
                if(inst.add1 == 1){
                    cpu->registrador1 = getDado(ram, inst.add2);
                    printf("Copiado da Ram para o registrador 1, o valor: %d\n",cpu->registrador1);

                    }else if (inst.add1 == 2){
                        cpu->registrador2 = getDado(ram, inst.add2);
                        printf("Copiado da Ram para o registrador 2, o valor: %d\n",cpu->registrador2);
                    }
                break;
            }
        cpu->PC++;
        }
    
    }



//liberar memoria
void liberaCPU(CPU* cpu){
    free(cpu);
}