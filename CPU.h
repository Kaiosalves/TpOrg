#ifndef CPU_H
#define CPU_H
#include "Instrucao.h"
#include "RAM.h"


typedef struct CPU CPU;
//Aloca a CPU e registradores
CPU* alocaCPU();

//seta um vetor de instrucoes auxiliares
void setPrograma(CPU* cpu, Instrucao* programaAux);

//inicia a a CPU
void iniciarCPU(CPU* cpu, Ram* ram);

//liberar memoria
void liberaCPU(CPU* cpu);


#endif