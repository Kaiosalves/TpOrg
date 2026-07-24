#include "Programa.h"
#include <stdlib.h>
#include <string.h>




Programa* criar_programa(Instrucao *instrucoes, int tamanho, const char *nome) {
    Programa *p = (Programa*)malloc(sizeof(Programa));
    p->instrucoes = instrucoes;
    p->tamanho = tamanho;
    p->PC = 0;
    p->estado = EXECUTANDO;
    strcpy(p->nome, nome);
    // Inicializa registradores vazios
    for (int i = 0; i < 4; i++) {
        p->reg1.palavras[i] = 0;
        p->reg2.palavras[i] = 0;
        p->reg3.palavras[i] = 0;
    }
    p->reg1.endBloco = -1;
    p->reg2.endBloco = -1;
    p->reg3.endBloco = -1;
    return p;
}

void liberar_programa(Programa *p) {
    if (p) {
        if (p->instrucoes) free(p->instrucoes);
        free(p);
    }
}