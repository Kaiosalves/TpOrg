
#include "CPU.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

CPU* alocaCPU() {
    CPU *cpu = (CPU*)malloc(sizeof(CPU));
    cpu->programa = NULL;
    cpu->tratador = NULL;
    cpu->ucm = NULL;
    cpu->ciclos_totais = 0;
    return cpu;
}
//seta o programa a ser exec
void set_programa(CPU *cpu, Programa *p){
    cpu->programa = p;
}
//define que vai executar o prog quando tiver interrupcao
void set_tratador(CPU *cpu, Programa *p){
    cpu->tratador = p;
}

/**
 * @brief 
 * executa uma instucao de um programa
 * @param cpu 
 * @param p 
 * @return int == 1 se terminou e == 0 se foi concluida a instrucao
 */
static int executar_instrucao(CPU *cpu, Programa *p) {
//recebe a instrucao atual, pelo contado PC
    Instrucao inst = p->instrucoes[p->PC];
    p->PC++;
//HALT: se opcode eh -1, finalizado e retorna 1 
    if (inst.opcode == -1) {
        p->estado = FINALIZADO;
        return 1;
    }

    int op = inst.opcode;

    if (op == 2) {
    // Instrução de interrupção explícita do TP2 – agora é ignorada (NOP)
    return 0;
}
//soma == 0
//sub == 1
    if (op == 0 || op == 1) {
    int veio_disco1 = 0, veio_disco2 = 0;
    BlocoMemoria *reg1 = ucm_ler(cpu->ucm, inst.add1, &veio_disco1);
    BlocoMemoria *reg2 = ucm_ler(cpu->ucm, inst.add2, &veio_disco2);
    
    // Se algum bloco veio do disco
    if (veio_disco1 || veio_disco2) 
       printf("Acesso a disco \n");
    
//soma ou subtrai endereco 1 com 2
    int resultado;
    if (op == 0) {
        resultado = reg1->palavras[inst.add1.endPalavra] + reg2->palavras[inst.add2.endPalavra];
    } else {
        resultado = reg1->palavras[inst.add1.endPalavra] - reg2->palavras[inst.add2.endPalavra];
    }
    //escreve no endereco 3
    ucm_escrever(cpu->ucm, inst.add3, resultado);
    
    if (op == 0) {
        printf("[%s] %s -> %d\n", p->nome, "SOMA", resultado);
    } else {
        printf("[%s] %s -> %d\n", p->nome, "SUB", resultado);
    }
    
    return 0;
}
//instrucao desconhecida

    printf("Instrucao Desconhecida\n");
    return 0;
}


//executa o tratador (ou outro programa)
static void executar_subprograma(CPU *cpu, Programa *p) {
    if (p == NULL) {
        printf("ERRO: executar_subprograma com programa NULL\n");
        return;
    }
    int terminou = 0;
    while (!terminou && p->estado != FINALIZADO) {
        terminou = executar_instrucao(cpu, p);
    }
}


void executar_cpu(CPU *cpu, int ciclos_max){
    srand(time(NULL));
    int ciclos = 0;
    Programa* p = cpu->programa;

    if(!p){
        printf("ERRO: NAO FOI CARREGADO UM PROGRAMA\n");
        return;
    }
        
    printf("_____PROGRAMA INICIADO_____\n");

    while(ciclos < ciclos_max && p->estado != FINALIZADO){
        int terminou = executar_instrucao(cpu, p);
        if(terminou){
            printf("HALT: PROGRAMA FINALIZOU\n");
            break;
        }

        // Verifica interrupcao probabilistica - apos cada instrucao
        if (p != cpu->tratador && p->estado != FINALIZADO) {
            if ((rand() % 100) < PROB_INTERRUPCAO) {
            printf("\n INTERRUPÇÃO OCORREU!\n");
            printf("--- EXECUTANDO TRATADOR ---\n");
        // Verifica se o tratador já terminou e o reinicia
                if (cpu->tratador->estado == FINALIZADO) {
                cpu->tratador->estado = EXECUTANDO;
                cpu->tratador->PC = 0;
        }
            executar_subprograma(cpu, cpu->tratador);
            printf("--- FIM DO TRATADOR, retornando a %s (PC=%d) ---\n\n", p->nome, p->PC);
    }
}

        ciclos++;
        cpu->ciclos_totais++;
    }

    printf("\n--- EXECUÇÃO CONCLUÍDA ---\n");
    printf("Ciclos executados: %d\n", ciclos);
}
//liberar memoria
void liberaCPU(CPU *cpu) {
    if (cpu) {
        if (cpu->programa) liberar_programa(cpu->programa);
        if (cpu->tratador) liberar_programa(cpu->tratador);
        free(cpu);
    }
}