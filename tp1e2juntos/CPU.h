#ifndef CPU_H
#define CPU_H
#include "Instrucao.h"
#include "RAM.h"


typedef struct CPU CPU;
//Aloca a CPU e registradores
CPU* alocaCPU();

//seta um vetor de instrucoes auxiliares
void setPrograma(CPU* cpu, Instrucao* programaAux);
BlocoMemoria* IniciarCache(int tamanho);
//inicia a a CPU
void iniciarCPU(CPU* cpu, Ram* ram, int TamCache1, int Tamcache2);

//liberar memoria
void liberaCPU(CPU* cpu);


#endif