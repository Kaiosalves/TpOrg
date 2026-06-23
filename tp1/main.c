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
void programaExp(CPU *cpu,Ram *ram, int base, int exp);
void programaCifra(Ram *ram, CPU *cpu, int chave, int tam, char *msg);
void normaVetor(CPU *cpu, Ram *ram, int *vetor, int tamVetor);
void programaTrocar(Ram *ram, CPU *cpu, int posA, int posB, int posTemp);
void programaBubbleSort(Ram *ram, CPU *cpu, int tamVetor);
void programaMDC(Ram *ram, CPU *cpu, int a, int b);
int main(){
//seed aleatoria
    srand(time(NULL));

    //Iniciar a Ram e cpu
    CPU *cpu = alocaCPU();
    Ram *ram = alocarEstruturaRam();
    
    //programas que podem ser executados(tirar o comentario para executar)
    //programaAleatorio(ram, cpu, 2000);
    //programaMult(ram, cpu, 15, 10);
    //programaDiv(ram, cpu, 175, 4);
    //programaFat(ram, cpu, 3);
    //programaSomaMatriz(ram, cpu, 3);
    //programaExp(cpu, ram, 8, 3);
    char msg[] = "olaJoubert";
    programaCifra(ram, cpu, 3, 5, msg);
    int v[5] = {6,7,5,2,12};
    normaVetor(cpu, ram, v, 5);
    programaBubbleSort(ram, cpu, 50);
    //programaMDC(ram, cpu, 12, 8);
    
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
            setRegistrador1(cpu, matriz2[i][j]);
            
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
    int deslocamento = card * card; //aponta pro inicio da matriz2
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
void programaExp(CPU *cpu,Ram *ram, int base, int exp){
    if (exp == 0) {
        printf("%d^%d = 1\n", base, exp);
        return;
    }
    setRegistrador1(cpu, base);
    
// Criamos uma estrutura de RAM temporária para a exponenciacao guardar o acumulado

    criarRam(ram, 1); 
//copia do reg 1 para ram [0]
    Instrucao trechoSalva[2];
    trechoSalva[0].opcode = 2; 
    trechoSalva[0].add1 = 1;   
    trechoSalva[0].add2 = 0;   
    trechoSalva[1].opcode = -1;
    setPrograma(cpu, trechoSalva);
    iniciarCPU(cpu, ram);
//faz os tramites da multiplicacao "guardando o resultado"
    for(int i = 0; i < exp - 1; i++){
        Instrucao trechoLeitura[2];
        trechoLeitura[0].opcode = 3; 
        trechoLeitura[0].add1 = 1;   
        trechoLeitura[0].add2 = 0;  
        trechoLeitura[1].opcode = -1;
        setPrograma(cpu, trechoLeitura);
        iniciarCPU(cpu, ram);
        
        int acumuladoAtual = getRegistrador1(cpu);

//multiplica deixando o resultado no reg1
        programaMult(ram, cpu, acumuladoAtual, base);

        int resultadoMultiplicacao = getRegistrador1(cpu);

        criarRam(ram, 1); 
        setRegistrador1(cpu, resultadoMultiplicacao);
        setPrograma(cpu, trechoSalva);
        iniciarCPU(cpu, ram);
    }

//passa da Ram[0] para o reg1
    Instrucao trechoFinal[2];
    trechoFinal[0].opcode = 3; 
    trechoFinal[0].add1 = 1;  
    trechoFinal[1].opcode = -1;
    setPrograma(cpu, trechoFinal);
    iniciarCPU(cpu, ram);

    printf("\n%d^%d = %d \n", base, exp, getRegistrador1(cpu));
}
void programaCifra(Ram *ram, CPU *cpu, int chave, int tam, char *msg){
    criarRamVazia(ram, tam + 1); // uma pra cada letra + 1 pra chave

//trecho 1: passar chave de criptografia para Ram[0]
    setRegistrador1 (cpu, chave);
    Instrucao trechoChave[2];
    trechoChave[0].opcode = 2; trechoChave[0].add1 = 1; trechoChave[0].add2 = 0;
    trechoChave[1].opcode = -1;
    setPrograma(cpu, trechoChave);
    iniciarCPU(cpu,ram);

//trecho 2: passa mensagem para a Ram
    for(int i = 0; i <tam; i++){
        setRegistrador1(cpu, msg[i]);
        
        Instrucao trechoMsg[2];
        trechoMsg[0].opcode = 2; trechoMsg[0].add1 = 1; trechoMsg[0].add2 = i +1;
        trechoMsg[1].opcode = -1;

        setPrograma(cpu, trechoMsg);
        iniciarCPU(cpu, ram);
    }
    printf("Mensagem encriptada pela CPU:\n");
    char mensagemFinal[tam + 1];
// trecho 3: Soma da RAM[i] + RAM[0] e subscreve em RAM[i]
    for(int i = 0; i <tam; i++){
        int pos = i + 1;
        
        Instrucao trechoEnc[2];
        trechoEnc[0].opcode = 0; 
        trechoEnc[0].add1 = pos; 
        trechoEnc[0].add2 = 0; 
        trechoEnc[0].add3 = pos;
        trechoEnc[1].opcode = -1;

        setPrograma(cpu, trechoEnc);
        iniciarCPU(cpu, ram);

//trecho 4: passar da Ram para o Registrador

        Instrucao trechoReg [2];
        trechoReg[0].opcode = 3; trechoReg[0].add1 = 1; trechoReg[0].add2 = pos; trechoReg[0].add3 = -1;
        trechoReg[1].opcode = -1;

        setPrograma(cpu, trechoReg);
        iniciarCPU(cpu, ram);

// converter denovo para char e imprimir

        int encriptado = getRegistrador1(cpu);
        mensagemFinal[i] = (char)encriptado;
    }
    mensagemFinal[tam] = '\0'; 
    printf("Mensagem encriptada pela CPU: %s\n\n", mensagemFinal);
    printf("\n");
}
void normaVetor(CPU *cpu, Ram *ram, int *vetor, int tamVetor){
    criarRamVazia(ram, tamVetor+4);
    
    setRegistrador1(cpu, 0);//valor inicial da soma
    Instrucao trechoSet0[2];
    trechoSet0[0].opcode = 2;
    trechoSet0[0].add1 = 1;
    trechoSet0[0].add2 = 0;
    trechoSet0[1].opcode = -1;
    setPrograma(cpu, trechoSet0);
    iniciarCPU(cpu, ram);
    
    //Essa parte só copia o vetor para as posições de 2, 3... a n;
    for(int i = 0; i < tamVetor; i++){
        setRegistrador1(cpu, vetor[i]);

        Instrucao trechoCopia[2];
        trechoCopia[0].opcode = 2;
        trechoCopia[0].add1 = 1;
        trechoCopia[0].add2 = i+2;
        trechoCopia[1].opcode = -1;//parada

        setPrograma(cpu, trechoCopia);
        iniciarCPU(cpu, ram);
    }
    
    for(int i = 0; i < tamVetor; i++){
        //Quadrado do termo armazenado em reg1 
        int valorTermo = getDado(ram,i+2);
        if(valorTermo == 0)
            continue;

        for(int j = 0; j < valorTermo; j++){
            Instrucao trechoSomaQuadrado[2];
            trechoSomaQuadrado[0].opcode = 0;   //soma
            trechoSomaQuadrado[0].add1 = 0;     //valor anterior para somar
            trechoSomaQuadrado[0].add2 = i+2;   //posicão atual do vetor p somar
            trechoSomaQuadrado[0].add3 = 0;     // Resultado da soma
            trechoSomaQuadrado[1].opcode = -1;  // parada

            setPrograma(cpu, trechoSomaQuadrado);
            iniciarCPU(cpu, ram);
        }
        
        
    }
    //TEOREMA DE NICÔMACO
    int posResRaiz = tamVetor +1;
    int posConst1 = tamVetor +3;
    int posConst2 = tamVetor +2;
    setDado(ram, 1, 1);         // primeiro número impar
    setDado(ram, posConst1, 1); //Constante 1
    setDado(ram, posConst2, 2); //constante 2
    setDado(ram, posResRaiz, 0);
    //enquanto o resultado do quadrado for maior que o impar atual
    while(getDado(ram, 0) >= getDado(ram, 1)){
        Instrucao trechoRaiz[4];

        // Essa parte subtrai o impar atual no resultado da soma dos quadrados
        trechoRaiz[0].opcode = 1; // subtrai
        trechoRaiz[0].add1 = 0;
        trechoRaiz[0].add2 = 1;
        trechoRaiz[0].add3 = 0;

        // Essa parte incrementa o contador em +1
        trechoRaiz[1].opcode = 0; //soma
        //Valor para ser incrementado no contador
        trechoRaiz[1].add1 = posConst1; 
        trechoRaiz[1].add2 = posResRaiz; //posição do contador
        trechoRaiz[1].add3 = posResRaiz;

        //Soma 2 na posição 1 da ram para pular para o proximo impar
        trechoRaiz[2].opcode = 0;//soma
        trechoRaiz[2].add1 = 1;
        trechoRaiz[2].add2 = posConst2;
        trechoRaiz[2].add3 = 1;

        trechoRaiz[3].opcode = -1;//parada do programa
        
        setPrograma(cpu, trechoRaiz);
        iniciarCPU(cpu, ram);
    }

    Instrucao trechoMove[2];
    trechoMove[0].opcode = 3; // mover da ram para o registrador
    trechoMove[0].add1 = 1;
    trechoMove[0].add2 = posResRaiz;

    trechoMove[1].opcode = -1;//halt

    setPrograma(cpu, trechoMove);
    iniciarCPU(cpu, ram);


    printf("\nResultado da norma aproximada de V: %d\n", getRegistrador1(cpu));
    // O CÁLCULO DA RAIZ SE BASEIA EM SUBTRAIR O RESULTADO DO QUADRADO PELO PROXIMO INTEIRO IMPAR E CONTAR A QUANTIDADE DE SUBTRAÇÕES, O NÚMERO TOTAL SERÁ O RESULTADO APROXIMADO DA RAIZ!!! SÓ TE DA A RAIZ EXATA QUANDO O NÚMERO TEM UM RAIZ EXATA
}

void programaTrocar(Ram *ram, CPU *cpu, int posA, int posB, int posTemp){
    // passo 1: temp = A
    Instrucao trecho1[2]; // copia ram[posA] para o registrador 1
    trecho1[0].opcode = 3; trecho1[0].add1 = 1; trecho1[0].add2 = posA;
    trecho1[1].opcode = -1;
    setPrograma(cpu, trecho1);
    iniciarCPU(cpu, ram);

    Instrucao trecho2[2]; // copia registrador 1 para ram[posTemp]
    trecho2[0].opcode = 2; trecho2[0].add1 = 1; trecho2[0].add2 = posTemp;
    trecho2[1].opcode = -1;
    setPrograma(cpu, trecho2);
    iniciarCPU(cpu, ram);

    // passo 2: A = B
    Instrucao trecho3[2]; // copia ram[posB] para o registrador 1
    trecho3[0].opcode = 3; trecho3[0].add1 = 1; trecho3[0].add2 = posB;
    trecho3[1].opcode = -1;
    setPrograma(cpu, trecho3);
    iniciarCPU(cpu, ram);

    Instrucao trecho4[2]; // copia registrador 1 para ram[posA]
    trecho4[0].opcode = 2; trecho4[0].add1 = 1; trecho4[0].add2 = posA;
    trecho4[1].opcode = -1;
    setPrograma(cpu, trecho4);
    iniciarCPU(cpu, ram);

    // passo 3: B = temp
    Instrucao trecho5[2]; // copia ram[posTemp] para o registrador 1
    trecho5[0].opcode = 3; trecho5[0].add1 = 1; trecho5[0].add2 = posTemp;
    trecho5[1].opcode = -1;
    setPrograma(cpu, trecho5);
    iniciarCPU(cpu, ram);

    Instrucao trecho6[2]; // copia registrador 1 para ram[posB]
    trecho6[0].opcode = 2; trecho6[0].add1 = 1; trecho6[0].add2 = posB;
    trecho6[1].opcode = -1;
    setPrograma(cpu, trecho6);
    iniciarCPU(cpu, ram);
}

void programaBubbleSort(Ram *ram, CPU *cpu, int tamVetor){
    int *vetor = (int*) malloc(tamVetor * sizeof(int));
    
    // preenche o vetor com números aleatórios entre 0 e 99
    for(int i = 0; i < tamVetor; i++){
        vetor[i] = rand() % 100;
    }

    // imprime o vetor antes
    printf("Vetor antes do bubble sort:\n");
    for(int i = 0; i < tamVetor; i++){
        printf("%d ", vetor[i]);
    }
    printf("\n");

    // cria RAM com espaço para o vetor + 2 posições auxiliares
    // posição tamVetor = resultado da comparação
    // posição tamVetor + 1 = temp para a troca
    criarRamVazia(ram, tamVetor + 2);
    int posTemp = tamVetor + 1;

    // copia o vetor para a RAM
    for(int i = 0; i < tamVetor; i++){
        setRegistrador1(cpu, vetor[i]);

        Instrucao trechoCopia[2];
        trechoCopia[0].opcode = 2; trechoCopia[0].add1 = 1; trechoCopia[0].add2 = i;
        trechoCopia[1].opcode = -1;

        setPrograma(cpu, trechoCopia);
        iniciarCPU(cpu, ram);
    }

    // bubble sort do maior para o menor
    for(int i = 0; i < tamVetor - 1; i++){
        for(int j = 0; j < tamVetor - 1 - i; j++){

            Instrucao trechoComp[2];
            trechoComp[0].opcode = 1; trechoComp[0].add1 = j; trechoComp[0].add2 = j + 1; trechoComp[0].add3 = tamVetor;
            trechoComp[1].opcode = -1;

            setPrograma(cpu, trechoComp);
            iniciarCPU(cpu, ram);

            int resultado = getRegistrador1(cpu);
            if(resultado < 0){
                programaTrocar(ram, cpu, j, j+1, posTemp);
            }
        }
    }

    // imprime o vetor depois
    printf("Vetor depois do bubble sort:\n");
    for(int i = 0; i < tamVetor; i++){
        Instrucao trechoLer[2];
        trechoLer[0].opcode = 3; trechoLer[0].add1 = 1; trechoLer[0].add2 = i;
        trechoLer[1].opcode = -1;

        setPrograma(cpu, trechoLer);
        iniciarCPU(cpu, ram);

        printf("%d ", getRegistrador1(cpu));
    }
    printf("\n");
    free(vetor);
}

void programaMDC(Ram *ram, CPU *cpu, int a, int b){
    // cria RAM com 3 posições
    // RAM[0] = a
    // RAM[1] = b
    // RAM[2] = não utilizada (reserva)
    criarRamVazia(ram, 3);

    // copia a para RAM[0]
    setRegistrador1(cpu, a);
    Instrucao trechoA[2];
    trechoA[0].opcode = 2; trechoA[0].add1 = 1; trechoA[0].add2 = 0;
    trechoA[1].opcode = -1;
    setPrograma(cpu, trechoA);
    iniciarCPU(cpu, ram);

    // copia b para RAM[1]
    setRegistrador1(cpu, b);
    Instrucao trechoB[2];
    trechoB[0].opcode = 2; trechoB[0].add1 = 1; trechoB[0].add2 = 1;
    trechoB[1].opcode = -1;
    setPrograma(cpu, trechoB);
    iniciarCPU(cpu, ram);

    // algoritmo de euclides
    // enquanto a e b forem diferentes, subtrai o menor do maior
    while(getDado(ram, 0) != getDado(ram, 1)){

        // se a > b, subtrai b de a e salva em RAM[0]
        if(getDado(ram, 0) > getDado(ram, 1)){
            Instrucao trechoSub[2];
            trechoSub[0].opcode = 1; trechoSub[0].add1 = 0; trechoSub[0].add2 = 1; trechoSub[0].add3 = 0;
            trechoSub[1].opcode = -1;
            setPrograma(cpu, trechoSub);
            iniciarCPU(cpu, ram);

        // se b > a, subtrai a de b e salva em RAM[1]
        } else {
            Instrucao trechoSub[2];
            trechoSub[0].opcode = 1; trechoSub[0].add1 = 1; trechoSub[0].add2 = 0; trechoSub[0].add3 = 1;
            trechoSub[1].opcode = -1;
            setPrograma(cpu, trechoSub);
            iniciarCPU(cpu, ram);
        }
    }

    // quando RAM[0] == RAM[1] o MDC foi encontrado
    // copia o resultado de RAM[0] para o registrador 1
    Instrucao trechoRes[2];
    trechoRes[0].opcode = 3; trechoRes[0].add1 = 1; trechoRes[0].add2 = 0;
    trechoRes[1].opcode = -1;
    setPrograma(cpu, trechoRes);
    iniciarCPU(cpu, ram);

    printf("MDC(%d, %d) = %d\n", a, b, getRegistrador1(cpu));
}
