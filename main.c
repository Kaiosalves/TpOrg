#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "RAM.h"
#include "CPU.h"
#include "Instrucao.h"


//prototipo dos programas a serem executados
void programaAleatorio(Ram *ram, CPU *cpu, int qdeInstrucoes);
void programaMult(Ram *ram, CPU *cpu, int multiplicando, int multiplicador);
void programaDiv(Ram *ram, CPU *cpu, int dividendo, int divisor);
void programaFat(Ram *ram, CPU *cpu, int fat);
void programaSomaMatriz(Ram *ram, CPU *cpu, int card);

int main(){
//seed aleatoria
    srand(time(NULL));

//Iniciar a Ram e cpu
    CPU *cpu = alocaCPU();
    Ram *ram = alocarEstruturaRam();

//programas que podem ser executados(tirar o comentario para executar)

    // programaAleatorio(ram, cpu, 2000);
    // programaMult(ram, cpu, 15, 10);
    // programaDiv(ram, cpu, 175, 4);
    // programaFat(ram, cpu, 3);
    // programaSomaMatriz(ram, cpu, 3);

    printf("programa terminou!\n");

//desligar a maquina
    liberarRam(ram);
    free(ram);
    liberaCPU(cpu);


    return 0;
}

//programa aleatorio (simular  benchmark)

void programaAleatorio(Ram *ram, CPU *cpu, int qdeInstrucoes){
    Instrucao *Programa = (Instrucao*) malloc(qdeInstrucoes * sizeof(Instrucao));
    int tamanhoRAM = 1000;

    criarRamAleat(ram, tamanhoRAM);

    for(int i = 0; i< qdeInstrucoes - 1; i++){
        Programa[i].opcode = rand() % 2; 
        Programa[i].add1 = rand() % tamanhoRAM; 
        Programa[i].add2 = rand() % tamanhoRAM;
        Programa[i].add3 = rand() % tamanhoRAM;
    }

    Programa[qdeInstrucoes -1].opcode = -1; //halt

    setPrograma(cpu, Programa);
    iniciarCPU (cpu, ram);
    free(Programa);
}

// programa para multiplicar
void programaMult(Ram *ram, CPU *cpu, int multiplicando, int multiplicador){
    criarRam(ram, 2);
    setRegistrador1(cpu,multiplicando);

//copiar para RAM[1] o valor do multiplicando (reg1)

//cria o trecho 1 com 2 posicoes
    Instrucao trecho1[2];

    trecho1[0].opcode = 2;trecho1[0].add1 = 1;trecho1[0].add2 = 1;
    trecho1[1].opcode = -1;

    setPrograma(cpu, trecho1);
    iniciarCPU(cpu, ram);

//soma os valores

    for(int i = 0; i < multiplicador; i++) {
        Instrucao trecho2[2];
        trecho2[0].opcode = 0; trecho2[0].add1 = 0; trecho2[0].add2 = 1; trecho2[0].add3 = 0;
        trecho2[1].opcode = -1;
        
        setPrograma(cpu, trecho2);
        iniciarCPU(cpu, ram);
    }
//copia da RAM[0] para o reg1

    Instrucao trecho3[2];
    trecho3[0].opcode = 3; trecho3[0].add1 = 1; trecho3[0].add2 = 0; trecho3[0].add3 = -1;
    trecho3[1].opcode = -1;

    setPrograma(cpu, trecho3);
    iniciarCPU(cpu, ram);

    printf("O resultado da multiplicacao eh: %d\n", getRegistrador1(cpu));
}

void programaDiv(Ram *ram, CPU *cpu, int dividendo, int divisor){

//cria ram com 4 posicoes
    criarRamVazia(ram, 4);
//dividendo == registrador 1
    setRegistrador1(cpu,dividendo);

// copia dividendo e divisor para RAM - trecho1
    Instrucao trecho1[3];
    trecho1[0].opcode = 2; trecho1[0].add1 = 1; trecho1[0].add2 = 0;

//divisor == registrador 2
    setRegistrador2(cpu, divisor);
    trecho1[1].opcode = 2; trecho1[1].add1 = 2; trecho1[1].add2 = 1;

    trecho1[2].opcode = -1;

    setPrograma(cpu,trecho1);
    iniciarCPU(cpu, ram);

    setRegistrador1(cpu, 1);
    
// Salva a constante 1 na RAM[2] - trecho 2
    Instrucao trecho2[2];
    trecho2[0].opcode = 2; trecho2[0].add1 = 1; trecho2[0].add2 = 2;
    trecho2[1].opcode = -1; 
    
    setPrograma(cpu, trecho2);
    iniciarCPU(cpu, ram);

// "divide " - trecho 3
//Subtrai ram[0] - ram[1] e salva em ram[0] (1)
//soma o contador(ram[3]) com 1 e salva em ram[3] (2)
//atualiza o dividendo (da ram[0] para reg1) (3)
//reinicia o processo
    while(dividendo >= divisor){
        Instrucao trecho3[4];
        trecho3[0].opcode = 1; trecho3[0].add1 = 0; trecho3[0].add2 = 1; trecho3[0].add3 = 0; // (1)

        trecho3[1].opcode = 0; trecho3[1].add1 = 2; trecho3[1].add2 = 3; trecho3[1].add3 = 3; // (2)

        trecho3[2].opcode = 3; trecho3[2].add1 = 1; trecho3[2].add2 = 0; trecho3[2].add3 = -1; // (3)
        trecho3[3].opcode = -1;

        setPrograma(cpu, trecho3);
        iniciarCPU(cpu, ram);

        //atualiza dividendo
        dividendo = getRegistrador1(cpu);

    }

//copia de RAM[3] para CPU
    Instrucao trecho4[2];
    trecho4[0].opcode = 3; trecho4[0].add1 = 1; trecho4[0].add2 = 3; trecho4[0].add3 = -1;
    trecho4[1].opcode = -1;
    
    setPrograma(cpu, trecho4);
    iniciarCPU(cpu, ram);
    
    printf("O resultado da divisao eh: %d\n", getRegistrador1(cpu));
}

void programaFat(Ram *ram, CPU *cpu, int fat){

    int j =1;

    for(int i = 1; i <=fat; i++){
        programaMult(ram, cpu, i, j);
    //copia da ram para cpu o parcial
        Instrucao trecho1[2];
        trecho1[0].opcode =3; trecho1[0].add1 = 1; trecho1[0].add2 =0;
        trecho1[1].opcode = -1;

        setPrograma(cpu, trecho1);
        iniciarCPU(cpu, ram);

    //atualiza j, com a parcial da multiplicacao
        j = getRegistrador1(cpu);
    }
    //copia da ram p/ cpu o final
    Instrucao trecho2[2];
    trecho2[0].opcode = 3; trecho2[0].add1 = 1; trecho2[0].add2 = 0; trecho2[0].add3 = -1;
    trecho2[1].opcode = -1;
    
    setPrograma(cpu, trecho2);
    iniciarCPU(cpu, ram);

    printf("O resultado do fatorial eh: %d\n", getRegistrador1(cpu));
}

void programaSomaMatriz(Ram *ram, CPU *cpu, int card) {
    int matriz1 [card][card];
    int matriz2 [card][card];

    //preenche com valores aleatorios a matriz
    for(int i = 0; i < card; i++) {
        for(int j = 0; j < card; j++) {
            matriz1[i][j] = rand() % 100;
            matriz2[i][j] = rand() % 100;
        }
    }
    criarRamVazia(ram, card * card * 3);
    int fimDaLInha = 0; // flag que percorre as "matrizes" na ram

    for (int i = 0; i < card; i++) {
        for(int j = 0; j < card; j++) {                
            // Copia o valor da matriz 1 para o Registrador 1
            setRegistrador1(cpu, matriz1[i][j]);
            
            Instrucao trecho1[2];
            // Copia da CPU para a RAM na posição atual de FimDaLinha
            trecho1[0].opcode = 2; trecho1[0].add1 = 1; trecho1[0].add2 = fimDaLInha;
            trecho1[1].opcode = -1;
            
            setPrograma(cpu, trecho1);
            iniciarCPU(cpu, ram);
            
            fimDaLInha++; 
        }           
    }
    for (int i = 0; i < card; i++) {
        for(int j = 0; j < card; j++) {                
            // Copia o valor da matriz 2 para o Registrador 1
            setRegistrador1(cpu, matriz1[i][j]);
            
            Instrucao trecho2[2];
            // Copia da CPU para a RAM na posição atual de FimDaLinha
            trecho2[0].opcode = 2; trecho2[0].add1 = 1; trecho2[0].add2 = fimDaLInha;
            trecho2[1].opcode = -1;
            
            setPrograma(cpu, trecho2);
            iniciarCPU(cpu, ram);
            
            fimDaLInha++; 
        }           
    }

    fimDaLInha = 0;
    int deslocamento = card * card;
    for (int i = 0; i < card; i++) {
        for(int j = 0; j < card; j++) {
            Instrucao trecho3[2];

            //soma o termo n de uma com o termo n da outra e armazena no termo n da terceira
            trecho3[0].opcode = 0; 
            trecho3[0].add1 = fimDaLInha; 
            trecho3[0].add2 = fimDaLInha + deslocamento; 
            trecho3[0].add3 = fimDaLInha + (2 * deslocamento);
            
            trecho3[1].opcode = -1;

            setPrograma(cpu, trecho3);
            iniciarCPU(cpu,ram);

            fimDaLInha++;
        }
    }

}
void FibProgram(Ram *ram, CPU *cpu, int n){
    criarRamVazia(ram, 3);
    setDado(ram, 0, 0);
    setDado(ram, 1, 1);

    if(n == 0){
        printf("Fibo = %d\n", getDado(ram, 0));
        return;
    }

    if(n == 1){
        printf("Fibo = %d\n", getDado(ram, 1));
        return;
    }

    Instrucao soma[2];
    soma[0].opcode = 0;
    soma[0].add1 = 0;
    soma[0].add2 = 1;
    soma[0].add3 = 2;
    soma[1].opcode = -1;


    int temp1, temp2;
    for(int i = 2; i <= n; i++){
        setPrograma(cpu, soma);
        iniciarCPU(cpu, ram);

        temp1 = getDado(ram, 1);
        temp2 = getDado(ram, 2);
        

        setDado(ram, 0, temp1);
        setDado(ram, 1, temp2);


    }
    printf("fibo = %d\n", getDado(ram, 1));
}
void expProgram(CPU *cpu,Ram *ram, int base, int exp){
    int res = 1;
    for(int i = 0; i < exp; i++){
        //resultado *= base; 
        programaMult(ram, cpu, res, base);
        res = getRegistrador1(cpu);
    }
    printf("%d^%d = %d\n", base, exp, res);
}