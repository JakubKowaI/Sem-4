#include <iostream>
#include <array>
#include <vector>
#include <queue>
#include <unordered_map>
#include <iomanip>
#include <functional>
#include <unordered_set>
#include <map>
#include <set>
#include <chrono>
#include <random>
#include <algorithm>
#include <fstream>

using namespace std;

constexpr int n = 4;
using Matrix = array<array<int, n>, n>;
int states;
Matrix goal;

void printPuzzle(const Matrix& puzzle) {
    for(const auto& row : puzzle) {
        for(int val : row) {
            if(val == 0) cout << setw(3) << " ";
            else cout << setw(3) << val;
        }
        cout << endl;
    }
}

bool isSolvable(const Matrix& a) {
    int size=n;
    int inv = 0, zero_row = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (a[i][j] == 0) {
                zero_row = i;
                continue;
            }
            for (int m = i; m < size; m++) {
                for (int o = (m == i ? j + 1 : 0); o < size; o++) {
                    if (a[m][o] != 0 && a[i][j] > a[m][o]) inv++;
                }
            }
        }
    }
    if (size % 2 == 1) { // Odd grid (3x3, 5x5...)
        return inv % 2 == 0;
    } else { // Even grid (4x4, 6x6...)
        return ((size - 1 - zero_row) % 2 == 1) ? (inv % 2 == 1) : (inv % 2 == 0);
    }
}

Matrix createGoalState() {
    Matrix goal;
    int val = 1;
    for(auto& row : goal) {
        for(auto& cell : row) {
            cell = val++;
        }
    }
    goal[n-1][n-1] = 0; 
    return goal;
}

vector<Matrix> getNeighbors(const Matrix& board) {
    vector<Matrix> neighbors;
    int zero_i, zero_j;
    
    for(zero_i = 0; zero_i < n; zero_i++) {
        for(zero_j = 0; zero_j < n; zero_j++) {
            if(board[zero_i][zero_j] == 0) break;
        }
        if(zero_j < n && board[zero_i][zero_j] == 0) break;
    }
    
    const int moves[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    
    for(const auto& move : moves) {
        int new_i = zero_i + move[0];
        int new_j = zero_j + move[1];
        
        if(new_i >= 0 && new_i < n && new_j >= 0 && new_j < n) {
            Matrix new_board = board;
            swap(new_board[zero_i][zero_j], new_board[new_i][new_j]);
            neighbors.push_back(new_board);
        }
    }
    
    return neighbors;
}

Matrix generateSolvablePuzzle() {
    mt19937 gen(time(nullptr));
    Matrix puzzle = goal; 

    do {
        for (int i = 0; i < n*n-2; i++) {
            int j = gen()%((n*n)-2);
            int row1 = i/n, col1 = i%n;
            int row2 = j/n, col2 = j%n;
            swap(puzzle[row1][col1], puzzle[row2][col2]);
        }
    } while (!isSolvable(puzzle));
    
    return puzzle;
}

Matrix makeRandomMoves(int k) {
    mt19937 gen(random_device{}());
    Matrix current = goal;
    Matrix previous = goal;
    
    for (int i = 0; i < k; i++) {
        auto neighbors = getNeighbors(current);
        
        neighbors.erase(
            remove_if(neighbors.begin(), neighbors.end(), 
                     [&previous](const Matrix& m) { return m == previous; }),
            neighbors.end()
        );
        
        if (neighbors.empty()) break;
        
        previous = current;
        current = neighbors[uniform_int_distribution<>(0, neighbors.size()-1)(gen)];
    }
    
    return current;
}

int h1(const Matrix& a) {
    int distance = 0;
    unordered_map<int, pair<int, int>> goalPositions;
    
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            goalPositions[goal[i][j]] = {i, j};
        }
    }
    
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            int val = a[i][j];
            if(val != 0) {  
                auto goalPos = goalPositions[val];
                distance += abs(i - goalPos.first) + abs(j - goalPos.second);
            }
        }
    }
    return distance;
}

int h2(const Matrix& a){
    int counter =0;
    int it=1;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(a[i][j]!=it)counter++;
            it = (it + 1) % (n * n);
        }
    }
    return counter;
}

vector<Matrix> path(map<Matrix,Matrix> previous, Matrix& state){
    vector<Matrix> fullPath = {state};
    auto search = previous.find(state);
    while(search != previous.end()){
        state=search->second;
        fullPath.insert(fullPath.begin(),state);
        search = previous.find(state);
    } 
    return fullPath;
}

vector<Matrix> solve(const Matrix& start){
    //priority_queue<node, vector<node>, CompareF> possible;
    states=1;
    
    multimap<int,Matrix> possible;

    possible.insert({h1(start),start});

    map<Matrix,Matrix> previous;

    map<Matrix, int> gCost;
    
    gCost.insert({start,0});

    map<Matrix,int> fCost;

    fCost.insert({start,h1(start)});

    Matrix current;

    set<Matrix> visited;

    while(!possible.empty()){
        current=possible.begin()->second;
        states++;
        

        if(current==goal){
            return path(previous,current);
        }

        
        
        possible.erase(possible.begin());
        if(visited.find(current)!=visited.end())continue;
        visited.insert(current);

        auto neighbors = getNeighbors(current);
        
        for(const Matrix& neighbor : neighbors) {
            if(visited.find(neighbor)!=visited.end())continue;
            if(gCost.find(neighbor)==gCost.end()){
                gCost[neighbor]=gCost[current]+1;
                fCost[neighbor] = gCost[neighbor] + h1(neighbor);
                //cout<<states<<endl;
            }
            if(gCost[current]+1<=gCost[neighbor]){
                    previous[neighbor]=current;
                    gCost[neighbor]=gCost[current]+1;
                    fCost[neighbor] = gCost[neighbor] + h1(neighbor);
                    
                    
                    possible.insert({gCost[current]+1+h1(neighbor),neighbor});
            }
        }
    }
    return {};
}

vector<Matrix> solve2(const Matrix& start){
    //priority_queue<node, vector<node>, CompareF> possible;
    states=1;
    
    multimap<int,Matrix> possible;

    possible.insert({h2(start),start});

    map<Matrix,Matrix> previous;

    map<Matrix, int> gCost;
    
    gCost.insert({start,0});

    map<Matrix,int> fCost;

    fCost.insert({start,h2(start)});

    Matrix current;

    set<Matrix> visited;

    while(!possible.empty()){
        current=possible.begin()->second;
        states++;
        
        if(current==goal){
            return path(previous,current);
        }
        
        possible.erase(possible.begin());

        visited.insert(current);

        auto neighbors = getNeighbors(current);
        
        for(const Matrix& neighbor : neighbors) {
            if(visited.find(neighbor)!=visited.end())continue;
            if(gCost.find(neighbor)==gCost.end()){
                gCost[neighbor]=gCost[current]+1;
                fCost[neighbor] = gCost[neighbor] + h2(neighbor);
                //cout<<states<<endl;
            }
            if(gCost[current]+1<=gCost[neighbor]){
                    previous[neighbor]=current;
                    gCost[neighbor]=gCost[current]+1;
                    fCost[neighbor] = gCost[neighbor] + h2(neighbor);
                    
                    
                    possible.insert({gCost[current]+1+h2(neighbor),neighbor});
            }
        }
    }
    return {};
}


void results(vector<Matrix> solution){
    
    if(!solution.empty()) {
        cout << "\nSolution found in " << solution.size()-1 << " moves!" << endl;
        for(size_t i = 0; i < solution.size(); i++) {
            cout << "\nStep " << i << ":" << endl;
            printPuzzle(solution[i]);
        }
        cout << "\nStates visited: " << states << endl;
    } else {
        cout << "\nNo solution found!" << endl;
    }
}

int main() {
    goal = createGoalState();
    const vector<int> move_counts = {5, 10, 15, 20, 25, 30};

    ofstream output("results.csv");
    output << "heurystyka,liczba_stanow,dlugosc_rozwiazania\n";
for(int i=0;i<50;i++){
    for (int moves : move_counts) {
        Matrix puzzle = makeRandomMoves(moves);
        cout << "\nTesting puzzle with " << moves << " random moves:\n";
        printPuzzle(puzzle);

        if (!isSolvable(puzzle)) {
            cout << "Not solvable!" << endl;
            continue;
        }

        // --- solve z h1 ---
        cout << "Solving with h1:\n";
        auto start_time = chrono::steady_clock::now();
        vector<Matrix> path1 = solve(puzzle);
        auto end_time = chrono::steady_clock::now();
        int visited1 = states;
        int steps1 = path1.empty() ? -1 : static_cast<int>(path1.size()) - 1;

        output << "h1," << visited1 << "," << steps1 << "\n";

        cout << "Elapsed time: " 
             << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count()
             << " ms\n";

        // --- solve2 z h2 ---
        cout << "Solving with h2:\n";
        start_time = chrono::steady_clock::now();
        vector<Matrix> path2 = solve2(puzzle);
        end_time = chrono::steady_clock::now();
        int visited2 = states;
        int steps2 = path2.empty() ? -1 : static_cast<int>(path2.size()) - 1;

        output << "h2," << visited2 << "," << steps2 << "\n";

        cout << "Elapsed time: " 
             << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count()
             << " ms\n";
    }
}
    output.close();
    cout << "\nWyniki zapisane do pliku 'results.csv'.\n";
    return 0;
}