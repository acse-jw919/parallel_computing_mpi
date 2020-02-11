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

    bool shortest(int start, vector<int> follow, vector<int> des, vector<int> points, vector<int>& route, vector<int> ban){
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
                    if(des.size() > 1){
                        int current_index = 0;
                        for(int j = 0; j < new_route.size(); j++){
                            if(new_route[j] == des[current_index]){
                                current_index++;
                                if(current_index == des.size()) break;
                            }
                        }
                        if(current_index != des.size()) valid = false;
                    }
                    if(valid){
                        route.resize(0);
                        for(int j = 0; j < new_route.size() - 1; j++) {
                            route.push_back(new_route[j]);
                        }
                        p1 = true;
                        break;
                    }
                }
                if(nei[i] == start) continue;
                route_queue.push(new_route);
                flag_queue.push(new_flag);
            }
            if(p1) break;
            route_queue.pop();
            flag_queue.pop();
        }
        return p1;
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

    int loc(int v){
        for(int i = 0; i < 16; i++){
            if(this->p[i] == v){
                return i;
            }
        }
    }
};

void find_solution(Puzzle*);
void place(Puzzle*, int, int, vector<int>);

int main(int argc, char** argv){

    int p[16] = {15, 2, 1, 12, 8, 5, 6, 11, 4, 9, 10, 7, 3, 14, 13};
    Puzzle *puzzle = new Puzzle(p);
    find_solution(puzzle);
    return 0;
}

void place(Puzzle *puzzle, int v, int target, vector<int> ban){
    vector<int> route;
    vector<int> points;
    vector<int> follow;
    vector<int> des;

    if(puzzle->p[0] != v){
        int pos = -1;
        for(int i = 0; i < 16; i++){
            if(puzzle->p[i] == v){
                pos = i;
                break;
            }
        }
        points.push_back(puzzle->blank);
        points.push_back(target);
        des.push_back(target);
        puzzle->shortest(pos, follow, des, points, route, ban);
        puzzle->rotate(pos, target, route, route.size());
    }
}

void find_solution(Puzzle *puzzle){
    puzzle->printPuzzle();

    vector<int> route;
    vector<int> points;
    vector<int> follow;
    vector<int> des;
    vector<int> ban;

    place(puzzle, 1, 0, ban);
    puzzle->flag[0] = true;
    place(puzzle, 2, 1, ban);
    puzzle->flag[1] = true;
    puzzle->printPuzzle();

    int three = puzzle->loc(3);
    if(three != 10) place(puzzle, 3, 10, ban);
    int four = puzzle->loc(4);
    if(four != 9){
        ban.push_back(10);
        place(puzzle, 4, 9, ban);
    }

    ban.resize(0);
    points.push_back(puzzle->blank);
    points.push_back(2);
    points.push_back(3);
    follow.push_back(10);
    des.push_back(3);
    des.push_back(2);
    bool re = puzzle->shortest(9, follow, des, points, route, ban);
    if(re){
        puzzle->rotate(9, 3, route, route.size());
        puzzle->printPuzzle();
        puzzle->flag[2] = true;
        puzzle->flag[3] = true;
    } else {
        cout << "False" << endl;
        return;
    }

    ban.resize(0);
    place(puzzle, 5, 4, ban);
    puzzle->flag[4] = true;
    puzzle->printPuzzle();

    int thirteen = puzzle->loc(13);
    if(three != 10) place(puzzle, 13, 10, ban);
    puzzle->printPuzzle();
    int nine = puzzle->loc(9);
    if(nine != 14){
        ban.push_back(10);
        place(puzzle, 9, 14, ban);
    }
    puzzle->printPuzzle();

    ban.resize(0);
    points.resize(0);
    follow.resize(0);
    des.resize(0);
    route.resize(0);
    
    points.push_back(puzzle->blank);
    points.push_back(8);
    points.push_back(12);
    follow.push_back(14);
    des.push_back(12);
    des.push_back(8);
    re = puzzle->shortest(10, follow, des, points, route, ban);
    if(re){
        puzzle->rotate(10, 12, route, route.size());
        puzzle->printPuzzle();
        puzzle->flag[8] = true;
        puzzle->flag[12] = true;
    } else {
        cout << "False" << endl;
        return;
    }

    ban.resize(0);
    place(puzzle, 6, 5, ban);
    puzzle->flag[5] = true;
    puzzle->printPuzzle();

    int seven = puzzle->loc(7);
    int eight = puzzle->loc(8);
    vector<int> went;
    re = false;
    while(!re){
        vector<int> seven_neighbor = puzzle->neighbors(seven);
        for(int i = 0; i < seven_neighbor.size(); i++){
            ban.resize(0);
            points.resize(0);
            follow.resize(0);
            des.resize(0);
            route.resize(0);
            ban.push_back(seven);
            points.push_back(puzzle->blank);
            points.push_back(seven_neighbor[i]);
            des.push_back(seven_neighbor[i]);
            re = puzzle->shortest(eight, follow, des, points, route, ban);
            if(re) {
                puzzle->rotate(eight, seven_neighbor[i], route, route.size());
                puzzle->printPuzzle();
                break;
            }
        }
        if(!re){
            went.push_back(seven);
            int i = 0;
            for(; i < seven_neighbor.size(); i++){
                bool tmp = false;
                for(int j = 0; j < went.size(); j++){
                    if(seven_neighbor[i] == went[j]){
                        tmp = true;
                        break;
                    }
                }
                if(!tmp){
                    ban.resize(0);
                    place(puzzle, 7, seven_neighbor[i], ban);
                    seven = puzzle->loc(7);
                    eight = puzzle->loc(8);
                    break;
                }
            }
            if(i == seven_neighbor.size()){
                cout << "False" << endl;
                return;
            }
        }
    }

    seven = puzzle->loc(7);
    eight = puzzle->loc(8);

    ban.resize(0);
    points.resize(0);
    follow.resize(0);
    des.resize(0);
    route.resize(0);
    
    points.push_back(puzzle->blank);
    points.push_back(6);
    points.push_back(7);
    follow.push_back(seven);
    des.push_back(7);
    des.push_back(6);
    re = puzzle->shortest(eight, follow, des, points, route, ban);
    if(re){
        puzzle->rotate(eight, 7, route, route.size());
        puzzle->printPuzzle();
        puzzle->flag[6] = true;
        puzzle->flag[7] = true;
    } else {
        cout << "False" << endl;
        return;
    }

    ban.resize(0);
    place(puzzle, 10, 9, ban);
    puzzle->flag[9] = true;
    puzzle->printPuzzle();

}
