#ifndef PROGRAMA_H
#define PROGRAMA_H

#include "Instrucao.h"
#include "BlocoMemoria.h"

typedef enum {
    EXECUTANDO,
    FINALIZADO
} EstadoPrograma;

typedef struct {
    Instrucao *instrucoes;
    int tamanho;
    int PC;
    BlocoMemoria reg1, reg2, reg3;  // registradores do programa
    EstadoPrograma estado;
    char nome[20];
} Programa;

Programa* criar_programa(Instrucao *instrucoes, int tamanho, const char *nome);
void liberar_programa(Programa *p);

#endif