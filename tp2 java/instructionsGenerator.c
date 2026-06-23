//Gerador de instruções feito por Luísa Calegari, Thiago Cecote e Josué Vila Real
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N_INST 10000 //numero de instrucoes
#define N_FOR 5     //numero de instrucoes que vao se repetir
#define N_MEM 1000  //numero da memoria para endereco do bloco
#define N_PROB 50   //probabilidade de repeticao
#define N_OPCODE 4  //numero maximo de opcodes
#define N_WORD 4    //numero de palavras no bloco

int rep[5][7];
int inst[10000][7];

void instGenerator();

int main() {
    instGenerator();
    return 0;
}

void instGenerator() {
    FILE *arquivo = fopen("instructions2.txt", "w");
    int random;
    int instruc[8] = {6, 6, 6, 6, 6, 6, 6, 6};
    srand(time(NULL));

    for (int i = 0; i < N_FOR; i++) {
        random = rand() % N_OPCODE;
        rep[i][0] = random;
        for (int j = 1; j <= (instruc[random]); j += 2) {
            rep[i][j] = rand() % N_MEM;
            rep[i][j + 1] = rand() % N_WORD;
        }
    }

    for (int i = 0; i < N_INST;) {
        random = (rand() % 100) + 1;
        if (random <= N_PROB && i + N_FOR < N_INST) {
            for (int j = 0; j < N_FOR; j++) {
                for (int k = 0; k < 7; k++) {
                    inst[i + j][k] = rep[j][k];
                }
            }
            i += N_FOR;
        } else {
            random = rand() % N_OPCODE;
            inst[i][0] = random;
            for (int j = 1; j <= instruc[random]; j += 2) {
                inst[i][j] = rand() % N_MEM;
                inst[i][j + 1] = rand() % N_WORD;
            }
            i++;
        }
    }
    for (int i = 0; i < N_INST; i++) {
        for (int k = 0; k < 6; k++) {
            fprintf(arquivo, "%d:", inst[i][k]);
        }
        fprintf(arquivo, "%d\n", inst[i][6]);
    }

    fclose(arquivo);
}
