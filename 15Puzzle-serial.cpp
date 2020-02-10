#include <iostream>
#include <iomanip>
#include <queue>
#include <vector>

using namespace std;

class Puzzle{
public:
    int *p;
    int blank;
    Puzzle(){
        this->p = new int[16];
        this->blank = 15;
    }

    Puzzle(int p[16]){
        this->p = new int[16];
        for(int i = 0; i < 16; i++) this->p[i] = p[i];
        this->blank = 15;
    }

    int move(int dir, int prev){
        // if(!this->moveable(dir, prev)) return -1;
        this->p[this->blank] = this->p[this->blank + dir];
        this->blank += dir;
        this->p[this->blank] = 0;
        return this->blank - dir;
    }

    bool moveable(int dir, int prev){
        if(prev == this->blank + dir) return false;
        if(dir == 1){
            if(this->blank % 4 == 3) return false;
        } else if(dir == -1){
            if(this->blank % 4 == 0) return false;
        } else if(dir == 4){
            if(this->blank / 4 >= 3) return false;
        } else if(dir == -4){
            if(this->blank / 4 == 0) return false;
        } else {
            return false;
        }
        return true;
    }

    bool ordered(){
        if(blank != 15) return false;
        for(int i = 0; i < 15; i++){
            if(this->p[i] != i + 1) return false;
        }
        return true;
    }

    void printPuzzle(){
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                if(blank == i * 4 + j){
                    cout << "  " << " ";
                } else {
                    cout << setw(2) << this->p[i * 4 + j] << " ";
                }
            }
            cout << endl;
        }
        cout << endl;
    }
};

struct triplet{
    Puzzle *puzzle;
    int dir, prev, length;
    triplet(int d, int p, int l, Puzzle *pz){
        dir = d;
        prev = p;
        length = l;
        puzzle = pz;
    }
};

void find_solution(Puzzle*);
void ava_dir(Puzzle&, vector<int>&, int);

int main(int argc, char** argv){

    int p[16] = {15, 2, 1, 12, 8, 5, 6, 11, 4, 9, 10, 7, 3, 14, 13};
    Puzzle *puzzle = new Puzzle(p);
    find_solution(puzzle);
    return 0;
}

void find_solution(Puzzle *puzzle){
    queue<triplet> q;
    vector<int> tmp = vector<int>();
    if(puzzle->ordered()) return;
    ava_dir(*puzzle, tmp, -1);
    for(int i = 0; i < tmp.size(); i++){
        Puzzle *tp = new Puzzle();
        for(int j = 0; j < 16; j++) tp->p[j] = puzzle->p[j];
        tp->printPuzzle();
        triplet t(tmp[i], -1, 1, tp);
        q.push(t);
    }
    int iter_count = 0;
    while(q.size() > 0){
        triplet current = q.front();
        int prev = current.puzzle->move(current.dir, current.prev);
        iter_count++;
        if(iter_count % 1000000 == 0){
            cout << iter_count << endl;
            current.puzzle->printPuzzle();
        }
        if(current.puzzle->ordered()) {
            current.puzzle->printPuzzle();
            break;
        }
        tmp = vector<int>();
        ava_dir(*current.puzzle, tmp, prev);
        for(int dir : tmp){
            Puzzle *tp = new Puzzle();
            for(int i = 0; i < 16; i++) tp->p[i] = current.puzzle->p[i];
            tp->blank = current.puzzle->blank;
            triplet t(dir, prev, current.length + 1, tp);
            q.push(t);
        }
        q.pop();
        delete current.puzzle;
    }
}

void ava_dir(Puzzle& puzzle, vector<int>& result, int prev){
    int dirs[4] = {1, -1, 4, -4};
    for(int i = 0; i < 4; i++){
        bool re = puzzle.moveable(dirs[i], prev);
        if(re) result.push_back(dirs[i]);
    }
}