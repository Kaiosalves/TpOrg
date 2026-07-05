
#include "Instrucao.h"
#include "RAM.h"
#include "CPU.h"
#include "MMU.h"
#include <stdlib.h>
#include <stdio.h>

struct CPU{
    BlocoMemoria* registrador1;
    BlocoMemoria* registrador2;
    BlocoMemoria* registrador3;

    int PC; // program counter
    int opcode;
    Instrucao *programa;

    int custo;
    int missc1;
    int hitc1;
    int missc2;
    int hitc2;

    BlocoMemoria *cache1;
    int tamCache1;
    BlocoMemoria *cache2;
    int tamCache2;
};

CPU* alocaCPU(){
    CPU *cpu = (CPU*) malloc(sizeof(CPU));
    cpu->registrador1 = NULL;
    cpu->registrador2 = NULL;
    cpu->registrador3 = NULL;
    cpu->PC = 0;
    cpu->opcode = 0x7FFFFFFF; // maior inteiro em hexadecimal
    cpu->programa = NULL;
    cpu->custo = 0;
    cpu->missc1 = 0;
    cpu->hitc1 = 0;
    cpu->missc2 = 0;
    cpu->hitc2 = 0;
    
    cpu->cache1 = NULL;
    cpu->cache2 = NULL;
    cpu->tamCache1 = 0;
    cpu->tamCache2 = 0;
    return cpu;

}

//seta um vetor de instrucoes auxiliares
void setPrograma(CPU* cpu, Instrucao* programaAux){
    cpu->programa = programaAux;
}

BlocoMemoria* IniciarCache(int tamanho){
    BlocoMemoria* umaCache = (BlocoMemoria*) malloc(tamanho * sizeof(BlocoMemoria));
    for(int i = 0; i < tamanho; i++){
        umaCache[i].endBloco = -1;
        umaCache[i].atualizado = 0;
        umaCache[i].custo = 0;
        umaCache[i].cacheHit = 0;
        for(int j = 0; j < 4; j++){
            umaCache[i].palavras[j] = 0;
        }
    }
    return umaCache;
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
void iniciarCPU(CPU* cpu, Ram* ram, int TamCache1, int Tamcache2){
    cpu->cache1 = IniciarCache(TamCache1);
    cpu->tamCache1 = TamCache1;
    cpu->cache2 = IniciarCache(Tamcache2);
    cpu->tamCache2 = Tamcache2;
    cpu->opcode =0;
    cpu->PC = 0;

    while (cpu->opcode != -1){
        Instrucao inst = cpu->programa[cpu->PC];
        cpu->opcode = inst.opcode;
        
        switch(cpu->opcode){
            case -1: // halt
                break;  

            case 0: //op soma - busca nas memorias e tras pra cache 1
                cpu->registrador1 = buscarNasMemorias(inst.add1, ram, cpu->cache1, cpu->tamCache1, cpu->cache2, cpu->tamCache2);
                cpu->registrador2 = buscarNasMemorias(inst.add2, ram, cpu->cache1, cpu->tamCache1, cpu->cache2, cpu->tamCache2);
                cpu->registrador3 = buscarNasMemorias(inst.add3, ram, cpu->cache1, cpu->tamCache1, cpu->cache2, cpu->tamCache2);
                //contadores de hit e miss
                //contador do reg1
                if(cpu->registrador1->cacheHit == 1) cpu->hitc1++;
                else if (cpu->registrador1->cacheHit == 2) { 
                    cpu->missc1++; 
                    cpu->hitc2++; }
                else if (cpu->registrador1->cacheHit == 3) { 
                    cpu->missc1++; 
                    cpu->missc2++; 
                }

                //contador p reg2
                if (cpu->registrador2->cacheHit == 1) cpu->hitc1++;
                else if (cpu->registrador2->cacheHit == 2) { 
                    cpu->missc1++; 
                    cpu->hitc2++;
                }
                else if (cpu->registrador2->cacheHit == 3) { 
                    cpu->missc1++; 
                    cpu->missc2++; 
                }
                
                //contadores p reg3
                if (cpu->registrador3->cacheHit == 1) cpu->hitc1++;
                else if (cpu->registrador3->cacheHit == 2) { 
                    cpu->missc1++; 
                    cpu->hitc2++; 
                }
                else if (cpu->registrador3->cacheHit == 3) { 
                    cpu->missc1++; 
                    cpu->missc2++; 
                }
                //soma das palavras
                cpu->registrador3->palavras[inst.add3.endPalavra] = 
                cpu->registrador1->palavras[inst.add1.endPalavra] + 
                cpu->registrador2->palavras[inst.add2.endPalavra];

                //modifica o dado, entao ativa a flag atualizado
                cpu->registrador3->atualizado = 1;

                //soma os custos acumulados
                cpu->custo += cpu->registrador1->custo + cpu->registrador2->custo + cpu->registrador3->custo;

                //imprimindo resultados
                printf("Instucao de soma -> RAM posicao %d com conteudo na cache 1 %d\n", 
                    inst.add3.endBloco, cpu->registrador3->palavras[inst.add3.endPalavra]);
                printf("Custo ate o momento: %d\n", cpu->custo);
                printf("Ate o momento: ... Hit C1: %d Miss C1: %d\n", cpu->hitc1, cpu->missc1);
                printf("Ate o momento: ... Hit C2: %d Miss C2: %d\n\n", cpu->hitc2, cpu->missc2);

                break;

            case 1: //op subtracao, busca nas memorias e tras pra cache 1
                cpu->registrador1 = buscarNasMemorias(inst.add1, ram, cpu->cache1, cpu->tamCache1, cpu->cache2, cpu->tamCache2);
                cpu->registrador2 = buscarNasMemorias(inst.add2, ram, cpu->cache1, cpu->tamCache1, cpu->cache2, cpu->tamCache2);
                cpu->registrador3 = buscarNasMemorias(inst.add3, ram, cpu->cache1, cpu->tamCache1, cpu->cache2, cpu->tamCache2);
                //contadores de hit e miss
                //contador do reg1
                if(cpu->registrador1->cacheHit == 1) cpu->hitc1++;
                else if (cpu->registrador1->cacheHit == 2) { 
                    cpu->missc1++; 
                    cpu->hitc2++; }
                else if (cpu->registrador1->cacheHit == 3) { 
                    cpu->missc1++; 
                    cpu->missc2++; 
                }

                //contador p reg2
                if (cpu->registrador2->cacheHit == 1) cpu->hitc1++;
                else if (cpu->registrador2->cacheHit == 2) { 
                    cpu->missc1++; 
                    cpu->hitc2++;
                }
                else if (cpu->registrador2->cacheHit == 3) { 
                    cpu->missc1++; 
                    cpu->missc2++; 
                }
                
                //contadores p reg3
                if (cpu->registrador3->cacheHit == 1) cpu->hitc1++;
                else if (cpu->registrador3->cacheHit == 2) { 
                    cpu->missc1++; 
                    cpu->hitc2++; 
                }
                else if (cpu->registrador3->cacheHit == 3) { 
                    cpu->missc1++; 
                    cpu->missc2++; 
                }

                //soma das palavras
                cpu->registrador3->palavras[inst.add3.endPalavra] = 
                cpu->registrador1->palavras[inst.add1.endPalavra] - 
                cpu->registrador2->palavras[inst.add2.endPalavra];

                //modifica o dado, entao ativa a flag atualizado
                cpu->registrador3->atualizado = 1;

                //soma os custos acumulados
                cpu->custo += cpu->registrador1->custo + cpu->registrador2->custo + cpu->registrador3->custo;

                //imprimindo resultados
                printf("Instucao de subtracao -> RAM posicao %d com conteudo na cache 1 %d\n", 
                    inst.add3.endBloco, cpu->registrador3->palavras[inst.add3.endPalavra]);
                printf("Custo ate o momento: %d\n", cpu->custo);
                printf("Ate o momento: ... Hit C1: %d Miss C1: %d\n", cpu->hitc1, cpu->missc1);
                printf("Ate o momento: ... Hit C2: %d Miss C2: %d\n\n", cpu->hitc2, cpu->missc2);

                break;
        }
        cpu->PC++;
    }
}


//liberar memoria
void liberaCPU(CPU* cpu){
    if(cpu!= NULL){
        if(cpu->cache1 != NULL) free(cpu->cache1);
        if(cpu->cache2 != NULL) free (cpu->cache2);
        free(cpu);
    }
}