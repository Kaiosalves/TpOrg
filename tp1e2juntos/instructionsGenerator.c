
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N_INST 10000
#define N_MEM 1000
#define N_OPCODE 2
#define N_WORD 4

int main(int argc, char *argv[]) {
    int N_PROB = 50;
    int N_FOR = 5;
    if (argc >= 3) {
        N_PROB = atoi(argv[1]);
        N_FOR = atoi(argv[2]);
    }
    if (N_PROB < 0 || N_PROB > 100) N_PROB = 50;
    if (N_FOR < 1) N_FOR = 5;

    char nomeArquivo[100];
    sprintf(nomeArquivo, "instructions_p%d_f%d.txt", N_PROB, N_FOR);

    FILE *arquivo = fopen(nomeArquivo, "w");
    if (!arquivo) { perror("fopen"); return 1; }

    int random;
    int instruc[8] = {6, 6, 6, 6, 6, 6, 6, 6};
    srand(time(NULL));

    int rep[5][7]; // N_FOR máximo 5, mas se for maior, adaptamos
    int maxFor = (N_FOR > 5) ? 5 : N_FOR; // limitamos a 5 para simplificar, mas pode aumentar
    for (int i = 0; i < maxFor; i++) {
        random = rand() % N_OPCODE;
        rep[i][0] = random;
        for (int j = 1; j <= instruc[random]; j += 2) {
            rep[i][j] = rand() % N_MEM;
            rep[i][j + 1] = rand() % N_WORD;
        }
    }

    int inst[N_INST][7];
    for (int i = 0; i < N_INST;) {
        random = (rand() % 100) + 1;
        if (random <= N_PROB && i + N_FOR < N_INST) {
            for (int j = 0; j < N_FOR; j++) {
                for (int k = 0; k < 7; k++) {
                    inst[i + j][k] = rep[j % maxFor][k];
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
    printf("Gerado %s\n", nomeArquivo);
    return 0;
}