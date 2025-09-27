#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Uso: %s n1 m1 n2 m2\n", argv[0]);
        return 1;
    }

    int n1 = atoi(argv[1]);
    int m1 = atoi(argv[2]);
    int n2 = atoi(argv[3]);
    int m2 = atoi(argv[4]);

    if (m1 != n2) {
        printf("As dimensoes nao sao compativeis para multiplicacao!\n");
        return 1;
    }

    srand(time(NULL));

    FILE *f1 = fopen("M1.txt", "w");
    FILE *f2 = fopen("M2.txt", "w");

    fprintf(f1, "%d %d\n", n1, m1);
    fprintf(f2, "%d %d\n", n2, m2);

    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < m1; j++) {
            fprintf(f1, "%d ", rand() % 10);
        }
        fprintf(f1, "\n");
    }

    for (int i = 0; i < n2; i++) {
        for (int j = 0; j < m2; j++) {
            fprintf(f2, "%d ", rand() % 10);
        }
        fprintf(f2, "\n");
    }

    fclose(f1);
    fclose(f2);
    printf("Arquivos M1.txt e M2.txt gerados com sucesso!\n");
    return 0;
}
