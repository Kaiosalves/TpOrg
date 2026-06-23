#include <stdlib.h>
#include "aluno.h"
#include <stdbool.h>
#include <stdio.h>

#define ALLOCATION_EVENT 0
#define DEALLOCATION_EVENT 1

struct lista{
    long tamMax;
    Celula* cab;
};

//FUNCOES DAS ESTRUTURAS
Lista* criaLista(long tam_max){
    Lista* pTemp = (Lista*)malloc(sizeof(Lista));
    if(pTemp){
        pTemp->cab = (Celula*)malloc(sizeof(Celula));
        pTemp->tamMax = tam_max;
        if(!pTemp->cab){
            free(pTemp);
            pTemp = NULL;
        }

        pTemp->cab->data.inicio = 0;
        pTemp->cab->data.tam = 0;
        Celula *memory = (Celula*)malloc(sizeof(Celula));
        if(!memory){
            free(pTemp->cab);
            free(pTemp);
            return NULL;
        }
        memory->data.inicio = 0;
        memory->data.tam = tam_max;
        memory->prox = NULL;
        pTemp->cab->prox = memory;
    }
    return pTemp;
}

Processo* alocaProcessoVet(int tam){
    if(tam <= 0)
        return NULL;
    Processo *pTemp = (Processo*)malloc(sizeof(Processo)*tam);
    if(!pTemp)
        pTemp = NULL;
    return pTemp;
}

void liberaLista(Lista *pLista){
    if(!pLista)
        return;

    Celula *aux = pLista->cab;

    while(aux != NULL){
        Celula *prox = aux->prox;
        free(aux);
        aux = prox;
    }

    free(pLista);
}

//FUNCOES DE GERENCIAMENTO DE MEMÓRIA
RES alocaProcesso(Processo *proc, Lista* pMem, long *alocs, int idProcess){
    if(!proc || !pMem || !alocs)
        return ERRO;
    Celula *aux = pMem->cab->prox;
    Celula *ant = pMem->cab;
    while(aux != NULL && aux->data.tam < proc[idProcess].tam){
        ant = aux;      
        aux = aux->prox;    //caminhando na lista
    }
    if(!aux)
        return FULL; //toda e memória foi consumida e nenhum bloco foi liberado
    
    alocs[idProcess] = aux->data.inicio;
    //separando os espaços na memória

    //atualiza o inicio do bloco (alocando memória)
    aux->data.inicio = aux->data.inicio + proc[idProcess].tam;
    //atualiza o tamanho restante do bloco
    aux->data.tam = aux->data.tam - proc[idProcess].tam; 

    if(aux->data.tam == 0){ //Se toda a memoria do bloco foi consumida
        ant->prox = aux->prox;
        free(aux);
    }
    return ALOCOU;
}
RES desalocaProcesso(Processo *proc, Lista* pMem, long *alocs, int idProcess){
    if(!proc || !pMem || !alocs)
        return ERRO;
    Celula* aux = pMem->cab->prox;
    Celula* ant = pMem->cab;
    if(!aux){ //lista esta vazia ou seja memoria cheia
        Celula *novo = (Celula*)malloc(sizeof(Celula));
        if(!novo) return ERRO;
        novo->data.inicio = alocs[idProcess];
        novo->data.tam = proc[idProcess].tam;
        novo->prox = NULL;

        ant->prox = novo;
        

        return DESALOC;
    }
    while(alocs[idProcess] > aux->data.inicio && aux->prox){
        ant = aux;
        aux = aux->prox;
    }
    Celula *novo = (Celula*)malloc(sizeof(Celula));
    if(!novo) return ERRO;
    novo->prox = aux;
    novo->data.inicio = alocs[idProcess];
    novo->data.tam = proc[idProcess].tam;
    ant->prox = novo;
    return DESALOC;
}
bool defragMemory(Lista *pLista){
    if(!pLista || !pLista->cab->prox)
        return false;
    Celula* aux = pLista->cab->prox;
    
    while(aux && aux->prox){
        Celula* proximo = aux->prox;
        //Se aux termina onde o proximo começa
        if(aux->data.inicio + aux->data.tam == proximo->data.inicio){
            //Absorve o próximo bloco de memória
            aux->data.tam += proximo->data.tam;
            
            aux->prox = proximo->prox; //Pulamos o bloco absorvido
            free(proximo);//liberando sua memória
        }
        // mas e se o primeiro bloco nao precisar ser juntado???
        else{
            aux = aux->prox;
        }
    }
    
    return true;
}

//ORDENAÇAO DE VETORES
RES comparaEvento(Evento e1, Evento e2){
    if(e1.tempo < e2.tempo)
        return ANTES;

    else if(e1.tempo == e2.tempo){
        if(e1.tipo > e2.tipo)
            return ANTES;
        else if(e1.tipo == e2.tipo)
            return IGUAL;
    }

    return DEPOIS;
}
//complexidade O(n)
void merge(Evento *events,int ini, int fim, int m){
    int leftsize = (m - ini) +1;
    int rightsize = fim - m;

    Evento *leftVector = (Evento*)malloc(sizeof(Evento)*leftsize);
    Evento *rightVector = (Evento*)malloc(sizeof(Evento)*rightsize);

    if(!leftVector || !rightVector)
        return;
    //copiando os valores para os subvetores
    int i, j;
    for(i = 0; i < leftsize; i++)
        leftVector[i] = events[i + ini];
    
    for(j = 0; j < rightsize; j++)
        rightVector[j] = events[j + m + 1];
    //Comparando os valores 
    int k = ini;
    i = 0; j = 0;
    while(i < leftsize && j < rightsize){
        RES comp = comparaEvento(leftVector[i], rightVector[j]);
        if(comp == ANTES || comp == IGUAL)
            events[k++] = leftVector[i++];
        else
            events[k++] = rightVector[j++];
        
    }
    //copiando o resto do vetor
    while(i < leftsize)
        events[k++] = leftVector[i++];
    while(j < rightsize)
        events[k++] = rightVector[j++];
    //liberando os vetores auxiliares
    free(leftVector);
    free(rightVector);
}
//Merge sort complexidade O(nlogn)
void ordenaEventos(Evento*v, int ini, int fim){
    if(ini < fim){
        int m = (ini + fim) / 2;
        ordenaEventos(v, ini, m);   //O(logn)
        ordenaEventos(v, m+1, fim);
        merge(v, ini, fim, m);      //O(n)
    }
    return;
}
long* first_fit(Processo *processos, int num_processos) {
   long *alocacoes = (long *)malloc(num_processos * sizeof(long));
    // verificacao necessaria para passar no valgrind, revisar
    for(int i = 0; i < num_processos; i++) alocacoes[i] = -1;
    Evento *eventos = (Evento*)malloc(sizeof(Evento) * num_processos * 2);
    long max_tam = 0;
    for (int i = 0; i < num_processos; ++i) {
        // Evento de alocacao
        eventos[2*i].tempo = processos[i].tempo_aloc;
        eventos[2*i].tipo = ALLOCATION_EVENT;
        eventos[2*i].processo = i;

        // Evento de desalocacao
        eventos[2*i+1].tempo = processos[i].tempo_desaloc;
        eventos[2*i+1].tipo = DEALLOCATION_EVENT;
        eventos[2*i+1].processo = i;

        max_tam += processos[i].tam;
    }
    // Insira aqui sua implementação
    ordenaEventos(eventos, 0, num_processos * 2 - 1);
    Lista* memory = criaLista(max_tam);
    int aux;
    for(int i = 0; i < num_processos*2; i++){
        aux = eventos[i].processo; //Qual processo está sendo modificado
        
        if(eventos[i].tipo == ALLOCATION_EVENT)
            if(alocaProcesso(processos, memory, alocacoes, aux) == FULL){
                printf("Memória Cheia!\n");
            }
                
        else{
            desalocaProcesso(processos, memory, alocacoes, aux);
            defragMemory(memory);
        }
    }

    free(eventos);
    liberaLista(memory);
    
    return alocacoes;
}