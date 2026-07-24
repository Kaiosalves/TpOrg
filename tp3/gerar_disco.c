#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int numBlocos = 10000; // pode ser passado como argumento
    if (argc >= 2) numBlocos = atoi(argv[1]);

    srand(time(NULL));
    FILE *f = fopen("disco_dados.txt", "w");
    if (!f) {
        perror("Erro ao criar disco_dados.txt");
        return 1;
    }

    for (int i = 0; i < numBlocos; i++) {
        fprintf(f, "%d %d %d %d %d\n",
                i,
                rand() % 1000,
                rand() % 1000,
                rand() % 1000,
                rand() % 1000);
    }

    fclose(f);
    printf("Arquivo disco_dados.txt gerado com %d blocos.\n", numBlocos);
    return 0;
}