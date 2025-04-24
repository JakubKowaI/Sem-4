#include <iostream>
#include <array>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <iomanip>
#include <map>
#include <unordered_map>

using namespace std;

constexpr int n = 4;
using Matrix = array<array<int, n>, n>;
int states;
int maxDistance = 60;

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

void printPuzzle(const Matrix& puzzle) {
    for(const auto& row : puzzle) {
        for(int val : row) {
            if(val == 0) cout << setw(3) << " ";
            else cout << setw(3) << val;
        }
        cout << endl;
    }
}

bool isSolvable(const Matrix& a){
    int inv=0;
    for(int i =0; i<n;i++){
        for(int j=0;j<n;j++){
            if(a[i][j]==0)continue;
            for(int m =i; m<n;m++){
                for(int o=j;o<n;o++){
                    if(a[i][j]>a[m][o]&&a[m][o]!=0)inv++;
                }
            }
        }
    }
    return inv%2;
}

int h1(const Matrix& a, const Matrix& goal) {
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

int h2(const Matrix& a, const Matrix& goal) {
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
                distance += (abs(i - goalPos.first) + abs(j - goalPos.second))*((n*n)-val);
            }
        }
    }
    return distance;
}

int h3(const Matrix& a, const Matrix& goal){
    int distance =0;

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(a[i][j]!=goal[i][j])distance++;
        }
    }
    return distance;
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

vector<Matrix> solve(const Matrix& state, int distance,set<Matrix> visited) {
    const Matrix goal = createGoalState();
    states++;


    if(state == goal) {
        cout<<"Bullseye "<< distance<<endl;
        maxDistance=distance;
        return {state};
    }
    
    if(visited.find(state) != visited.end() || distance > maxDistance) {
        return {};
    }
    
    visited.insert(state);
    auto neighbors = getNeighbors(state);
    multimap<int, Matrix> hoodMap;
    
    for(const Matrix& neighbor : neighbors) {
        hoodMap.insert({h1(neighbor, goal), neighbor});
    }
    
    vector<Matrix> res = {};
    for (const auto& [dist, neighbor] : hoodMap) {
        vector<Matrix> path = solve(neighbor, distance + 1,visited);
        if(!path.empty()) {
            if(res.empty()||path.size()+1<=res.size()){
                path.insert(path.begin(), state);
                res = path;
            }
        }
    }
    
    return res;  
}

int main() {
    // Matrix start = {{
    //     {{1, 2, 3, 4}},
    //     {{5, 6, 7, 8}},
    //     {{9, 10, 15, 11}},
    //     {{13, 14, 12, 0}}
    // }};

    Matrix start = {{
        {{10, 12, 1, 2}},
        {{11, 14, 13, 8}},
        {{15, 3, 5, 4}},
        {{6, 9, 7, 0}}
    }};

    // Matrix start2 = {{
    //     {{4, 5, 6}},
    //     {{0, 8, 1}},
    //     {{7, 3, 2}}
    // }};
    


    cout << "Initial puzzle:" << endl;
    printPuzzle(start);

    cout <<endl<< "Goal puzzle:" << endl;
    printPuzzle(createGoalState());
    
    cout << "\nSolving..." << endl;
    //auto solution = solvePuzzle(start);

    set<Matrix> visited{};
    states=0;
    if(isSolvable(start)){
        cout<<"Not solvable!"<<endl;
        return -1;
    }
    auto solution = solve(start,0,visited);
    
    
    if(solution.empty()) {
        cout << "No solution found!" << endl;
    } else {
        cout << "\nSolution found in " << solution.size()-1 << " moves:" << endl;
        for(size_t i = 0; i < solution.size(); i++) {
            cout << "\nStep " << i << ":" << endl;
            printPuzzle(solution[i]);
        }
        cout<<"\nStates visited: "<<states<<endl;
    }
    
    return 0;
}