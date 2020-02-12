#include <iostream>
#include <cmath>
#include <ctime>

#define M 5
#define N 5
#define ITER_TIME 9

using namespace std;

void generateByValue(bool*);
void generate(int, bool*);
void print_map(int, int, bool*);
void evolve(int, int, bool*, bool);

int dirx[8] = {-1, 0, 0, 1, -1, -1, 1, 1};
int diry[8] = {0, -1, 1, 0, -1, 1, -1, 1};

int main(int argc, char** argv){

    bool *init_map = new bool[M*N];
    // generate(M*N, init_map);
    generateByValue(init_map);
    print_map(M, N, init_map);

    clock_t start = clock();
    for(int i = 0; i < ITER_TIME; i++){
        // cout << "ITER: " << i + 1 << endl;
        evolve(M, N, init_map, true);
        // print_map(M, N, init_map);
    }
    print_map(M, N, init_map);
    clock_t end = clock();

    cout << (double) (end - start) / CLOCKS_PER_SEC << " seconds" << endl;

    return 0;
}

void generate(int size, bool *result){
    for(int i = 0; i < size; i++){
        result[i] = rand() % 2;
    }
}

void generateByValue(bool *result){
    bool tmp[M*N] = {
        0, 0, 1, 0, 0,
        0, 1, 1, 0, 1,
        0, 1, 0, 0, 0,
        0, 1, 1, 0, 0,
        1, 1, 1, 0, 1
    };
    for(int i = 0; i < M*N; i++){
        result[i] = tmp[i];
    }
}


void print_map(int m, int n, bool *lmap){
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            cout << lmap[i * n + j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void evolve(int m, int n, bool *lmap, bool periodic){
    bool *tmp = new bool[m*n];
    for(int i = 0; i < m; i++){
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
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            lmap[i * n + j] = tmp[i * n + j];
        }
    }
}