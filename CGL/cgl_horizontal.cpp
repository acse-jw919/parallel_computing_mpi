#include <mpi.h>
#include <iostream>
#include <fstream>
#include <ctime>

// #define M 1200
// #define N 1200
#define ITER_TIME 100
#define PRINT_RESULT false

using namespace std;

void print_data(int, int, bool*);
bool* readData(int, int, string, int&, int&);
void evolve(int, int, bool*, bool);

int dirx[8] = {-1, 0, 0, 1, -1, -1, 1, 1};
int diry[8] = {0, -1, 1, 0, -1, 1, -1, 1};

int main(int argc, char** argv){
    clock_t startTime;
    MPI_Status status_head, status_tail;
    MPI_Request req_head, req_tail;
    int myid, np;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int m, n;
    bool *data = readData(myid, np, "data/2078_source.txt", m, n);
    if(data == nullptr){
        cout << "Cannot read file into processor " << myid << endl;
        return -1;
    }

    startTime = clock();
    for(int i = 0; i < ITER_TIME; i++){
        if(myid < np - 1) MPI_Isend(&data[(m - 2) * n], n, MPI_CXX_BOOL, myid + 1, 0, MPI_COMM_WORLD, &req_tail);
        if(myid > 0) MPI_Isend(&data[n], n, MPI_CXX_BOOL, myid - 1, 1, MPI_COMM_WORLD, &req_head);

        if(myid > 0) MPI_Recv(&data[0], n, MPI_CXX_BOOL, myid - 1, 0, MPI_COMM_WORLD, &status_head);
        else for(int j = 0; j < n; j++) data[j] = false;
        if(myid < np - 1) MPI_Recv(&data[(m - 1) * n], n, MPI_CXX_BOOL, myid + 1, 1, MPI_COMM_WORLD, &status_tail);
        else for(int j = 0; j < n; j++) data[(m - 1) * n + j] = false;
        evolve(m, n, data, false);
    }
    double time_elapsed = (double)(clock() - startTime) / CLOCKS_PER_SEC;

    if(!myid){
        if(PRINT_RESULT) print_data(m, n, data);
        if(np > 1){
            MPI_Send(&time_elapsed, 1, MPI_DOUBLE, 1, 2, MPI_COMM_WORLD);
        }
    } else {
        double tmp;
        MPI_Recv(&tmp, 1, MPI_DOUBLE, myid - 1, 2, MPI_COMM_WORLD, &status_head);
        tmp += time_elapsed;
        if(PRINT_RESULT) print_data(m, n, data);
        if(myid != np - 1){
            MPI_Send(&tmp, 1, MPI_DOUBLE, myid + 1, 2, MPI_COMM_WORLD);
        } else {
            cout << endl << tmp << " seconds" << endl;
        }
    }
    MPI_Finalize();
    return 0;
}

bool* readData(int myid, int np, string filePath, int& m, int& n){
    ifstream dataFile(filePath, ios::in);
    if(!dataFile){
        return nullptr;
    }
    int counter = 0, total_m, start;
    if(!dataFile.eof()){
        dataFile >> total_m >> n;
        m = total_m / np;
        start = myid * m;
        if(myid < total_m % np) {
            m++;
            start += myid;
        } else {
            start += total_m % np;
        }
    } else {
        return nullptr;
    }
    m += 2;
    bool *data = new bool[(m) * n];
    bool tmp;
    while(!dataFile.eof()){
        if(counter < start){
            for(int i = 0; i < n; i++) dataFile >> tmp;
        } else {
            for(int i = 0; i < n; i++) dataFile >> data[(counter - start + 1) * n + i];
        }
        counter++;
        if(counter == start + m - 2) break;
    }
    return data;
}

void print_data(int m, int n, bool *data){
    for(int i = 1; i < m - 1; i++){
        for(int j = 0; j < n; j++){
            cout << data[i * n + j] << " ";
        }
        cout << endl;
    }
    // cout << endl;
}

void evolve(int m, int n, bool *lmap, bool periodic){
    bool *tmp = new bool[m*n];
    for(int i = 1; i < m - 1; i++){
        for(int j = 0; j < n; j++){
            int cx = i, cy = j;
            int cindex = i * n + j;
            int alive = 0;
            // cout << "[" << cx << "," << cy << "]" << endl;
            for(int k  = 0; k < 8; k++){
                int tindex, tx, ty; 
                if(!periodic){
                    tindex = (cx + dirx[k]) * n + (cy + diry[k]);
                    tx = tindex / n;
                    ty = tindex % n;
                    if(tindex < 0 || tindex > m*n) continue;
                    if(k < 4 && tx != cx && ty != cy) continue;
                    if(k >= 4 && (abs(tx - cx) + abs(ty - cy) != 2)) continue;
                } else {
                    tx = (cx + dirx[k] + m) % m;
                    ty = (cy + diry[k] + n) % n;
                    tindex = tx * n + ty;
                }
                // cout << "(" << tx << "," << ty << ") ";
                if(lmap[tindex]) alive++;
                // if(alive > 3) break;
            }
            // cout << endl << endl;
            if(lmap[cindex] && (alive < 2 || alive > 3)) tmp[cindex] = false;
            else if(!lmap[cindex] && alive == 3) tmp[cindex] = true;
            else tmp[cindex] = lmap[cindex];
        }
    }
    for(int i = 1; i < m - 1; i++){
        for(int j = 0; j < n; j++){
            lmap[i * n + j] = tmp[i * n + j];
        }
    }
    delete[] tmp;
}