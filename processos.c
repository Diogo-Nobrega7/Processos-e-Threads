#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s P\n", argv[0]);
        return 1;
    }

    int P = atoi(argv[1]);
    FILE *f1 = fopen("M1.txt", "r");
    FILE *f2 = fopen("M2.txt", "r");

    if (!f1 || !f2) {
        printf("Erro ao abrir os arquivos de entrada!\n");
        return 1;
    }

    int n1, m1, n2, m2;
    fscanf(f1, "%d %d", &n1, &m1);
    fscanf(f2, "%d %d", &n2, &m2);

    int **A = malloc(n1 * sizeof(int *));
    int **B = malloc(n2 * sizeof(int *));
    int **C = malloc(n1 * sizeof(int *));
    for (int i = 0; i < n1; i++) A[i] = malloc(m1 * sizeof(int));
    for (int i = 0; i < n2; i++) B[i] = malloc(m2 * sizeof(int));
    for (int i = 0; i < n1; i++) C[i] = malloc(m2 * sizeof(int));

    for (int i = 0; i < n1; i++)
        for (int j = 0; j < m1; j++)
            fscanf(f1, "%d", &A[i][j]);

    for (int i = 0; i < n2; i++)
        for (int j = 0; j < m2; j++)
            fscanf(f2, "%d", &B[i][j]);

    fclose(f1);
    fclose(f2);

    int total = n1 * m2;
    int chunk = (total + P - 1) / P;

    clock_t start = clock();

    for (int p = 0; p < P; p++) {
        pid_t pid = fork();
        if (pid == 0) {
            int startIdx = p * chunk;
            int endIdx = (p + 1) * chunk;
            if (endIdx > total) endIdx = total;

            for (int idx = startIdx; idx < endIdx; idx++) {
                int i = idx / m2;
                int j = idx % m2;
                C[i][j] = 0;
                for (int k = 0; k < m1; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
            exit(0);
        }
    }

    for (int p = 0; p < P; p++) wait(NULL);

    clock_t end = clock();

    FILE *fout = fopen("ResultadoProcessos.txt", "w");
    fprintf(fout, "%d %d\n", n1, m2);
    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < m2; j++) {
            fprintf(fout, "%d ", C[i][j]);
        }
        fprintf(fout, "\n");
    }
    fprintf(fout, "Tempo: %f segundos\n", (double)(end - start) / CLOCKS_PER_SEC);
    fclose(fout);

    printf("Resultado salvo em ResultadoProcessos.txt\n");

    for (int i = 0; i < n1; i++) free(A[i]);
    for (int i = 0; i < n2; i++) free(B[i]);
    for (int i = 0; i < n1; i++) free(C[i]);
    free(A);
    free(B);
    free(C);

    return 0;
}
