#include <iostream>
#include <iomanip>
#include <queue>
#include <vector>

using namespace std;

class Puzzle{
public:
    int *p;
    int blank;
    bool *flag;

    Puzzle(int p[16]){
        this->p = new int[16];
        this->flag = new bool[16];
        for(int i = 0; i < 16; i++) {
            this->p[i] = p[i];
            this->flag[i] = false;
        }
        this->blank = 15;
        this->p[this->blank] = 0;
    }

    void rotate(int start, int target, vector<int> route, int length){
        int offset = 0;
        int *tv = new int[length];
        tv[0] = this->p[route[0]];
        for(int i = 1; i < length; i++){
            if(route[i] == target){
                offset = i;
            }
            tv[i] = this->p[route[i]];
        }
        for(int i = 0; i < length; i++){
            int t_index = route[(i + offset) % length];
            this->p[t_index] = tv[i];
            if(tv[i] == 0){
                this->blank = t_index;
            }
        }
        delete[] tv;
    }

    bool shortest(int start, vector<int> follow, vector<int> points, vector<int>& route, vector<int> ban){
        queue<vector<int> > route_queue;
        queue<bool*> flag_queue;
        vector<int> route_init;
        bool flag_init[points.size()];
        for(int i = 0; i < points.size(); i++) flag_init[i] = false;
        route_init.push_back(start);
        for(int i = 0; i < follow.size(); i++){
            route_init.push_back(follow[i]);
        }
        route_queue.push(route_init);
        flag_queue.push(flag_init);
        bool p1 = false;
        while(route_queue.size() > 0){
            vector<int> current_route = route_queue.front();
            bool *current_flag = flag_queue.front();
            int end = current_route[current_route.size() - 1];
            vector<int> nei = this->neighbors(end);
            for(int i = 0; i < nei.size(); i++){
                if(nei[i] != start){
                    bool passed = false;
                    for(int j = 0; j < current_route.size(); j++){
                        if(current_route[j] == nei[i]){
                            passed = true;
                            break;
                        }
                    }
                    if(passed) continue;
                    for(int j = 0; j < ban.size(); j++){
                        if(ban[j] == nei[i]){
                            passed = true;
                            break;
                        }
                    }
                    if(passed) continue;
                }
                bool *new_flag = new bool[points.size()];
                for(int j = 0; j < points.size(); j++) {
                    new_flag[j] = current_flag[j];
                    if(nei[i] == points[j]) new_flag[j] = true;
                }
                vector<int> new_route;
                for(int j = 0; j < current_route.size(); j++) new_route.push_back(current_route[j]);
                new_route.push_back(nei[i]);
                if(new_route.size() > points.size() && nei[i] == start){
                    bool valid = true;
                    for(int j = 0; j < points.size(); j++) valid = valid && new_flag[j];
                    if(valid){
                        route.resize(0);
                        for(int j = 0; j < new_route.size() - 1; j++) {
                            route.push_back(new_route[j]);
                        }
                        p1 = true;
                        break;
                    }
                }
                route_queue.push(new_route);
                flag_queue.push(new_flag);
            }
            if(p1) break;
            route_queue.pop();
            flag_queue.pop();
        }
        return false;
    }

    vector<int> neighbors(int point){
        int dirs[4] = {-1, 1, -4, 4};
        vector<int> result;
        for(int i = 0; i < 4; i++){
            int target = dirs[i] + point;
            if(target / 4 != point / 4 && target % 4 != point % 4){
                continue;
            }
            if(target < 0 || target > 15 || this->flag[target]){
                continue;
            }
            result.push_back(target);
        }
        return result;
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

void find_solution(Puzzle*);

int main(int argc, char** argv){

    int p[16] = {15, 2, 1, 12, 8, 5, 6, 11, 4, 9, 10, 7, 3, 14, 13};
    Puzzle *puzzle = new Puzzle(p);
    find_solution(puzzle);
    return 0;
}

void find_solution(Puzzle *puzzle){
    puzzle->printPuzzle();
    vector<int> route;
    vector<int> points;
    vector<int> ban;
    vector<int> follow;
    points.push_back(15);
    points.push_back(0);
    puzzle->shortest(2, follow, points, route, ban);
    puzzle->rotate(2, 0, route, route.size());
    puzzle->printPuzzle();
    puzzle->flag[0] = true;

    route.resize(0);
    points.resize(0);
    ban.resize(0);
    follow.resize(0);

    points.push_back(puzzle->blank);
    points.push_back(1);
    puzzle->shortest(4, follow, points, route, ban);
    puzzle->rotate(4, 1, route, route.size());
    puzzle->printPuzzle();
    puzzle->flag[1] = true;

    route.resize(0);
    points.resize(0);
    ban.resize(0);
    follow.resize(0);

    points.push_back(puzzle->blank);
    points.push_back(9);
    puzzle->shortest(12, follow, points, route, ban);
    puzzle->rotate(12, 9, route, route.size());
    puzzle->printPuzzle();

    route.resize(0);
    points.resize(0);
    ban.resize(0);
    follow.resize(0);

    points.push_back(puzzle->blank);
    points.push_back(3);
    follow.push_back(9);
    puzzle->shortest(5, follow, points, route, ban);
    puzzle->rotate(5, 3, route, route.size());
    puzzle->printPuzzle();


}
