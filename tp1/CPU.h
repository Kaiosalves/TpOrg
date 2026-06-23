#ifndef CPU_H
#define CPU_H
#include "Instrucao.h"
#include "RAM.h"


typedef struct CPU CPU;
//Aloca a CPU e registradores
CPU* alocaCPU();

//seta um vetor de instrucoes auxiliares
void setPrograma(CPU* cpu, Instrucao* programaAux);

//passagem de valor do registrador1
void setRegistrador1(CPU *cpu, int valor);
int getRegistrador1(CPU *cpu);

//passagem de valor do registrador1
void setRegistrador2(CPU *cpu, int valor);
int getRegistrador2(CPU *cpu);

//inicia a a CPU
void iniciarCPU(CPU* cpu, Ram* ram);



//liberar memoria
void liberaCPU(CPU* cpu);


#endif