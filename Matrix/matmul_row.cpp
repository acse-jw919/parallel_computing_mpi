#include <mpi.h>
#include <iostream>
#include <ctime>

using namespace std;

void generateMatrix(int, int, double*);
void printMatrix(int, int, double*);

int main(int argc, char** argv){
    clock_t startTime, endTime;

    MPI_Status status;
    int myid, np;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int A_m = 1000, B_n = 1000, A_mp = 0;
    int A_n = 1000, B_m = A_n;
    double* B = new double[B_m * B_n];
    double* A = new double[A_m * A_n];

    if(A_m % np != 0){
        A_mp = A_m % np;
        A_m += A_mp;
    }
    int nrows = A_m / np;
    double* A_row = new double[nrows * A_n];

    if(!myid){
        generateMatrix(A_m, A_n, A);
        generateMatrix(B_m, B_n, B);
        // printMatrix(A_m - A_mp, A_n, A);
        // printMatrix(B_m, B_n, B);
        startTime = clock();
    }
    MPI_Bcast(B, B_m * B_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, nrows * A_n, MPI_DOUBLE, A_row, nrows * A_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double* tmp_result = new double[nrows * B_n];
    for(int i = 0; i < nrows; i++){
        for(int j = 0; j < B_n; j++){
            tmp_result[i * B_n + j] = 0;
            for(int k = 0; k < A_n; k++){
                tmp_result[i * B_n + j] += A_row[i * A_n + k] * B[k * B_n + j];
            }
        }
    }

    double* result = new double[A_m * B_n];
    MPI_Gather(tmp_result, nrows*B_n, MPI_DOUBLE, result, nrows*B_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if(!myid){
        endTime = clock();
        cout << (double) (endTime - startTime) / CLOCKS_PER_SEC << " seconds\n";
        // printMatrix(A_m - A_mp, B_n, result);
    }

    MPI_Finalize();

    return 0;
}

void printMatrix(int nrows, int ncols, double *matrix){
    for(int i = 0; i < nrows; i++){
        for(int j = 0; j < ncols; j++){
            cout << matrix[i * ncols + j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void generateMatrix(int nrows, int ncols, double *matrix){
    for(int i = 0; i < nrows; i++){
        for(int j = 0; j < ncols; j++){
            matrix[i * ncols + j] = rand() % 10;
        }
    }
}