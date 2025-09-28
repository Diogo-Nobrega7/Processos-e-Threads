#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

int **A, **B, **C;
int n1, m1, n2, m2;
int P; 

typedef struct {
    int id;
    int start, end;
} ThreadData;

void *multiplica(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    clock_t start_time = clock(); 

    for (int idx = data->start; idx < data->end; idx++) {
        int i = idx / m2;
        int j = idx % m2;
        C[i][j] = 0;
        for (int k = 0; k < m1; k++) {
            C[i][j] += A[i][k] * B[k][j];
        }
    }

    clock_t end_time = clock(); 

  
    char filename[64];
    sprintf(filename, "ResultadoThreads_%d.txt", data->id);

    FILE *fout = fopen(filename, "w");
    if (!fout) {
        perror("Erro ao criar arquivo de saída");
        pthread_exit(NULL);
    }

 
    fprintf(fout, "%d %d\n", n1, m2);


    for (int idx = data->start; idx < data->end; idx++) {
        int i = idx / m2;
        int j = idx % m2;
        fprintf(fout, "%d %d %d\n", i, j, C[i][j]); 
        
    }

    double tempo = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    fprintf(fout, "Tempo thread %d: %f segundos\n", data->id, tempo);

    fclose(fout);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s P\n", argv[0]);
        return 1;
    }

    P = atoi(argv[1]); 

    FILE *f1 = fopen("M1.txt", "r");
    FILE *f2 = fopen("M2.txt", "r");

    if (!f1 || !f2) {
        printf("Erro ao abrir arquivos M1.txt ou M2.txt\n");
        return 1;
    }

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

    int total = n1 * m2;
    int num_threads = (total + P - 1) / P; 

    pthread_t threads[num_threads];
    ThreadData data[num_threads];

    for (int t = 0; t < num_threads; t++) {
        data[t].id = t;
        data[t].start = t * P;
        data[t].end = (t + 1) * P;
        if (data[t].end > total) data[t].end = total;
        pthread_create(&threads[t], NULL, multiplica, &data[t]);
    }

    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    printf("%d arquivos de saída gerados (ResultadoThreads_X.txt)\n", num_threads);

    for (int i = 0; i < n1; i++) free(A[i]);
    for (int i = 0; i < n2; i++) free(B[i]);
    for (int i = 0; i < n1; i++) free(C[i]);
    free(A); free(B); free(C);

    return 0;
}


