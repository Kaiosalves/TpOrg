#ifndef CPU_H
#define CPU_H

#include "Instrucao.h"
#include "UCM.h"
#include "Programa.h"

#define PROB_INTERRUPCAO 0.5  // probabilidad de acontecer uma interrupcao

typedef struct CPU {
    Programa *programa; //programa principal
    Programa *tratador; //prog especial p tratamento de interrup
    UCM *ucm;
    int ciclos_totais; // numeros de ciclos
} CPU;

// Aloca a CPU
CPU* alocaCPU();

void set_programa(CPU *cpu, Programa *p);

void set_tratador(CPU *cpu, Programa *p);

void executar_cpu(CPU *cpu, int ciclos_max);

// Libera a CPU
void liberaCPU(CPU* cpu);

#endif