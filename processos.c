#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h> 

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Uso: %s P (P = numero de elementos por segmento)\n", argv[0]);
        return 1;
    }

    int chunk_size = atoi(argv[1]);


    FILE *f1 = fopen("M1.txt", "r");
    FILE *f2 = fopen("M2.txt", "r");

    if (!f1 || !f2) {
        printf("Erro ao abrir os arquivos de entrada (M1.txt ou M2.txt)!\n");
        return 1;
    }

    int n1, m1, n2, m2;
    fscanf(f1, "%d %d", &n1, &m1);
    fscanf(f2, "%d %d", &n2, &m2);

    if (m1 != n2) {
        printf("As dimensoes das matrizes nao sao compativeis para multiplicacao!\n");
        fclose(f1);
        fclose(f2);
        return 1;
    }



    int **A = malloc(n1 * sizeof(int *));
    int **B = malloc(n2 * sizeof(int *));
    

    for (int i = 0; i < n1; i++) A[i] = malloc(m1 * sizeof(int));
    for (int i = 0; i < n2; i++) B[i] = malloc(m2 * sizeof(int));


    for (int i = 0; i < n1; i++)
        for (int j = 0; j < m1; j++)
            fscanf(f1, "%d", &A[i][j]);

    for (int i = 0; i < n2; i++)
        for (int j = 0; j < m2; j++)
            fscanf(f2, "%d", &B[i][j]);

    fclose(f1);
    fclose(f2);


    int total_elements = n1 * m2;

    int num_processes = (total_elements + chunk_size - 1) / chunk_size;


    clock_t start_global = clock();


    for (int p = 0; p < num_processes; p++) {
        pid_t pid = fork();

        if (pid == 0) { 

            clock_t start_time = clock();

    
            int startIdx = p * chunk_size;
            int endIdx = startIdx + chunk_size;
            if (endIdx > total_elements) endIdx = total_elements;

            char filename[64];
            sprintf(filename, "ResultadoProcessos_parte_%d.txt", p);

            FILE *fout = fopen(filename, "w");
            if (!fout) {
                perror("Erro ao criar arquivo de saida");
                exit(1);
            }

        
            fprintf(fout, "%d %d\n", n1, m2);

 
            for (int idx = startIdx; idx < endIdx; idx++) {
                int i = idx / m2;
                int j = idx % m2;
                
                int element_C = 0;
                for (int k = 0; k < m1; k++) {
                    element_C += A[i][k] * B[k][j];
                }
                
     
                fprintf(fout, "%d %d %d\n", i, j, element_C);
            }

 
            clock_t end_time = clock();
            double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

  
            fprintf(fout, "Tempo de calculo individual: %f segundos\n", time_spent);
            fclose(fout);

  
            for (int i = 0; i < n1; i++) free(A[i]);
            for (int i = 0; i < n2; i++) free(B[i]);
            free(A);
            free(B);
            
            exit(0); 
        }
    }


    for (int p = 0; p < num_processes; p++) {
        wait(NULL);
    }
    

    clock_t end_global = clock();

    for (int i = 0; i < n1; i++) free(A[i]);
    for (int i = 0; i < n2; i++) free(B[i]);
    free(A);
    free(B);

    printf("Multiplicacao concluida com %d processos (segmentos).\n", num_processes);
    printf("Resultado salvo em %d arquivos (ResultadoProcessos_parte_0.txt ate ResultadoProcessos_parte_%d.txt).\n", num_processes, num_processes - 1);
    printf("Tempo total de execucao (do fork ao ultimo wait): %f segundos\n", (double)(end_global - start_global) / CLOCKS_PER_SEC);

    return 0;
}
