#include <mpi.h>
#include <cstdlib>
#include <iostream>

#define NUM_PAIRS 10
#define MAX_VALUE 10000

using namespace std;

void generatePositive(int, int*);
int gcd(int, int);

int main(int argc, char** argv){
    int myid, np;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int *V = new int[NUM_PAIRS * 2];
    int *index = new int[np + 1];
    int *answer = new int[NUM_PAIRS];
    index[0] = 0;
    int avg_count = (NUM_PAIRS * 2) / np;
    for(int i = 1; i <= np; i++){
        index[i] = index[i - 1] + avg_count;
        if(i != np){
            avg_count = (NUM_PAIRS * 2 - index[i]) / (np - i);
        }
    }

    if(!myid){
        generatePositive(NUM_PAIRS * 2, V);
        for(int i = 1; i < np; i++){
            MPI_Send(V + index[i], index[i + 1] - index[i], MPI_INT, i, 99, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(V + index[myid], index[myid + 1] - index[myid], MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
    }

    for(int i = index[myid]; i < index[myid + 1]; i+=2){
        answer[i/2] = gcd(V[i], V[i+1]);
    }

    if(!myid){
        for(int i = 1; i < np; i++){
            MPI_Recv(answer + index[i] / 2, (index[i + 1] - index[i]) / 2, MPI_INT, i, 98, MPI_COMM_WORLD, &status);
        }
    } else {
        MPI_Send(answer + index[myid] / 2, (index[myid + 1] - index[myid]) / 2, MPI_INT, 0, 98, MPI_COMM_WORLD);
    }

    if(!myid){
        for(int i = 0; i < NUM_PAIRS; i++){
            cout << "GCD of " << V[i*2] << " and " << V[i*2 + 1] << " = " << answer[i] << endl;
        }
        // for(int i = 0; i < NUM_PAIRS; i++){
        //     cout << answer[i] << " ";
        // }
        // cout << endl;
    }

    MPI_Finalize();

    return 0;
}

void generatePositive(int N, int *V){
    for(int i = 0; i < N; i++){
        V[i] = rand() % MAX_VALUE + 1;
    }
}

int gcd(int A, int B){
    if(A <= 0 || B <= 0){
        cerr << "Positive Integer required." << endl;
        return 1;
    }
    if(A == 1 || B == 1){
        return 1;
    }
    while(A != B){
        if(A > B){
            A -= B;
        } else {
            B -= A;
        }
    }
    return A;
}