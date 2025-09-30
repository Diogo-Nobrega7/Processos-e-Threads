#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

int **A, **B, **C;
int n1, m1, n2, m2;

int P; 

typedef struct {
    int startIdx, endIdx;
} ThreadData;

void *multiplica(void *arg) {
    ThreadData *data = (ThreadData *)arg;

   
    for (int idx = data->startIdx; idx < data->endIdx; idx++) {
        int i = idx / m2;
        int j = idx % m2;
        
        C[i][j] = 0;
        for (int k = 0; k < m1; k++) {

            C[i][j] += A[i][k] * B[k][j];
        }
    }
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("Uso: %s P (P = numero de threads)\n", argv[0]);
        return 1;
    }

    P = atoi(argv[1]); 
    if (P <= 0) {
        printf("O numero de threads (P) deve ser maior que zero.\n");
        return 1;
    }
    
    
    FILE *f1 = fopen("M1.txt", "r");
    FILE *f2 = fopen("M2.txt", "r");

    if (!f1 || !f2) {
        printf("Erro ao abrir arquivos M1.txt ou M2.txt\n");
        return 1;
    }

    fscanf(f1, "%d %d", &n1, &m1);
    fscanf(f2, "%d %d", &n2, &m2);

    if (m1 != n2) {
        printf("As dimensoes das matrizes nao sao compativeis para multiplicacao!\n");
        fclose(f1);
        fclose(f2);
        return 1;
    }


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
    int chunk = (total + P - 1) / P; 

    pthread_t *threads = malloc(P * sizeof(pthread_t));
    ThreadData *data = malloc(P * sizeof(ThreadData));
    
   
    clock_t start = clock();

    for (int t = 0; t < P; t++) {
        data[t].startIdx = t * chunk;
        data[t].endIdx = (t + 1) * chunk;
        if (data[t].endIdx > total) data[t].endIdx = total;
        
        if (pthread_create(&threads[t], NULL, multiplica, &data[t]) != 0) {
             perror("Erro ao criar thread");
             
             return 1;
        }
    }


    for (int t = 0; t < P; t++) {
        pthread_join(threads[t], NULL);
    }

    clock_t end = clock();
    double tempo_total = (double)(end - start) / CLOCKS_PER_SEC;


    
    FILE *fout = fopen("ResultadoThreads.txt", "w");
    if (!fout) {
        perror("Erro ao criar arquivo de saida final");
        return 1;
    }

    fprintf(fout, "%d %d\n", n1, m2);
    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < m2; j++) {
            fprintf(fout, "%d ", C[i][j]);
        }
        fprintf(fout, "\n");
    }
    fprintf(fout, "Tempo: %f segundos\n", tempo_total);
    fclose(fout);

    printf("Resultado salvo em ResultadoThreads.txt\n");
    printf("Tempo total de execucao com %d threads: %f segundos\n", P, tempo_total);

    
    for (int i = 0; i < n1; i++) free(A[i]);
    for (int i = 0; i < n2; i++) free(B[i]);
    for (int i = 0; i < n1; i++) free(C[i]);
    free(A); free(B); free(C);
    free(threads);
    free(data);

    return 0;
}



