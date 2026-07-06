#ifndef CPU_H
#define CPU_H

#include "Instrucao.h"
#include "UCM.h"


typedef struct CPU CPU;
//Aloca a CPU e registradores
CPU* alocaCPU();

//seta um vetor de instrucoes auxiliares
void setPrograma(CPU* cpu, Instrucao* programaAux);

void setUCM(CPU* cpu, UCM *ucm);

//inicia a a CPU
void iniciarCPU(CPU* cpu);

//liberar memoria
void liberaCPU(CPU* cpu);


#endif