#ifndef CPU_H
#define CPU_H

#include "Instrucao.h"
#include "UCM.h"
#include "BlocoMemoria.h"

typedef struct CPU {
    BlocoMemoria* registrador1;
    BlocoMemoria* registrador2;
    BlocoMemoria* registrador3;
    int PC;
    int opcode;
    Instrucao *programa;
    UCM *ucm;
} CPU;

// Aloca a CPU
CPU* alocaCPU();

// Seta o programa
void setPrograma(CPU* cpu, Instrucao* programaAux);

// Inicia a CPU
void iniciarCPU(CPU* cpu);

// Libera a CPU
void liberaCPU(CPU* cpu);

#endif