#include <iostream>
#include <array>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <iomanip>
#include <algorithm>
#include <functional>  // For greater<PuzzleState>

using namespace std;

constexpr int n = 4;
using Matrix = array<array<int, n>, n>;

struct PuzzleState {
    Matrix board;
    int g;  // Cost from start
    int h;  // Heuristic to goal
    int f;  // g + h
    vector<Matrix> path; // Solution path
    
    bool operator>(const PuzzleState& other) const {
        return f > other.f; // For min-heap
    }
};

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

int hammingDistance(const Matrix& board) {
    static const Matrix goal = createGoalState();
    int distance = 0;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(board[i][j] != 0 && board[i][j] != goal[i][j]) {
                distance++;
            }
        }
    }
    return distance;
}

vector<Matrix> getNeighbors(const Matrix& board) {
    vector<Matrix> neighbors;
    int zero_i, zero_j;
    
    // Find empty tile (0)
    for(zero_i = 0; zero_i < n; zero_i++) {
        for(zero_j = 0; zero_j < n; zero_j++) {
            if(board[zero_i][zero_j] == 0) break;
        }
        if(zero_j < n && board[zero_i][zero_j] == 0) break;
    }
    
    // Possible moves (up, down, left, right)
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

vector<Matrix> solvePuzzle(const Matrix& start) {
    const Matrix goal = createGoalState();
    priority_queue<PuzzleState, vector<PuzzleState>, greater<PuzzleState>> openSet;
    set<Matrix> closedSet;
    
    PuzzleState initialState;
    initialState.board = start;
    initialState.g = 0;
    initialState.h = hammingDistance(start);
    initialState.f = initialState.g + initialState.h;
    initialState.path = {start};
    
    openSet.push(initialState);
    closedSet.insert(start);
    
    while(!openSet.empty()) {
        PuzzleState current = openSet.top();
        openSet.pop();
        
        if(current.board == goal) {
            return current.path;
        }
        
        for(const Matrix& neighbor : getNeighbors(current.board)) {
            if(closedSet.find(neighbor) == closedSet.end()) {
                closedSet.insert(neighbor);
                
                PuzzleState neighborState;
                neighborState.board = neighbor;
                neighborState.g = current.g + 1;
                neighborState.h = hammingDistance(neighbor);
                neighborState.f = neighborState.g + neighborState.h;
                neighborState.path = current.path;
                neighborState.path.push_back(neighbor);
                
                openSet.push(neighborState);
            }
        }
    }
    
    return {}; // No solution found
}

int main() {
    Matrix start = {{
        {{6, 4, 7, 11}},
        {{9, 5, 1, 3}},
        {{13, 10, 8, 2}},
        {{14, 15, 12, 0}}
    }};
    
    cout << "Initial puzzle:" << endl;
    printPuzzle(start);
    
    cout << "\nSolving with A* (Hamming distance heuristic)..." << endl;
    auto solution = solvePuzzle(start);
    
    if(solution.empty()) {
        cout << "No solution found!" << endl;
    } else {
        cout << "\nSolution found in " << solution.size()-1 << " moves:" << endl;
        for(size_t i = 0; i < solution.size(); i++) {
            cout << "\nStep " << i << ":" << endl;
            printPuzzle(solution[i]);
        }
    }
    
    return 0;
}