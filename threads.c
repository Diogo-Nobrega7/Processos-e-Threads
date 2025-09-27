#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int **A, **B, **C;
int n1, m1, n2, m2;
int P; // número de threads

typedef struct {
    int start, end;
} ThreadData;

void *multiplica(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    for (int idx = data->start; idx < data->end; idx++) {
        int i = idx / m2;
        int j = idx % m2;
        C[i][j] = 0;
        for (int k = 0; k < m1; k++) {
            C[i][j] += A[i][k] * B[k][j];
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s P\n", argv[0]);
        return 1;
    }

    P = atoi(argv[1]);

    FILE *f1 = fopen("M1.txt", "r");
    FILE *f2 = fopen("M2.txt", "r");

    fscanf(f1, "%d %d", &n1, &m1);
    fscanf(f2, "%d %d", &n2, &m2);

    A = malloc(n1 * sizeof(int *));
    B = malloc(n2 * sizeof(int *));
    C = malloc(n1 * sizeof(int *));
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

    pthread_t threads[P];
    ThreadData data[P];
    int total = n1 * m2;
    int chunk = (total + P - 1) / P;

    clock_t start = clock();
    for (int t = 0; t < P; t++) {
        data[t].start = t * chunk;
        data[t].end = (t + 1) * chunk;
        if (data[t].end > total) data[t].end = total;
        pthread_create(&threads[t], NULL, multiplica, &data[t]);
    }

    for (int t = 0; t < P; t++) pthread_join(threads[t], NULL);
    clock_t end = clock();

    FILE *fout = fopen("ResultadoThreads.txt", "w");
    fprintf(fout, "%d %d\n", n1, m2);
    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < m2; j++) {
            fprintf(fout, "%d ", C[i][j]);
        }
        fprintf(fout, "\n");
    }
    fprintf(fout, "Tempo: %f segundos\n", (double)(end - start) / CLOCKS_PER_SEC);
    fclose(fout);

    printf("Resultado salvo em ResultadoThreads.txt\n");
    return 0;
}
