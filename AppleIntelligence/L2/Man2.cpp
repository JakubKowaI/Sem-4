#include <iostream>
#include <array>
#include <vector>
#include <queue>
#include <unordered_map>
#include <iomanip>
#include <functional>
#include <unordered_set>

using namespace std;

constexpr int n = 4;
using Matrix = array<array<int, n>, n>;

void printState(const Matrix& puzzle) {
    for(const auto& row : puzzle) {
        for(int val : row) {
            if(val == 0) cout << setw(3) << " ";
            else cout << setw(3) << val;
        }
        cout << endl;
    }
}

struct PuzzleState {
    Matrix board;
    int g;  // cost from start
    int h;  // heuristic to goal
    int last_move; // Stores last move direction (0-3)
    vector<Matrix> path; // Stores the complete path
    
    bool operator>(const PuzzleState& other) const {
        return (g + h) > (other.g + other.h);
    }
};

struct MatrixHash {
    size_t operator()(const Matrix& m) const {
        size_t h = 0;
        for (const auto& row : m) {
            for (int val : row) {
                h = h * 31 + val;
            }
        }
        return h;
    }
};

Matrix createGoal() {
    Matrix goal;
    int val = n*n-1;
    for(auto& row : goal) {
        for(auto& cell : row) {
            cell = val--;
        }
    }
    goal[n-1][n-1] = 0;
    return goal;
}

int manhattanDistance(const Matrix& board) {
    static const Matrix goal = createGoal();
    int distance = 0;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            int val = board[i][j];
            if(val != 0) {
                int goal_i = (val-1)/n;
                int goal_j = (val-1)%n;
                distance += abs(i-goal_i) + abs(j-goal_j);
            }
        }
    }
    return distance;
}

int manhattanDistance2(const Matrix& board) {
    static const Matrix goal = createGoal();
    int distance = 0;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            int val = board[i][j];
            if(val != 0) {
                int goal_i = (val-1)/n;
                int goal_j = (val-1)%n;
                // Weighted but still admissible (divide by max possible weight)
                distance += (abs(i-goal_i) + abs(j-goal_j)) * (val / (n*n));
            }
        }
    }
    return distance;
}



vector<Matrix> solvePuzzle(const Matrix& start) {
    const Matrix goal = createGoal();
    priority_queue<PuzzleState, vector<PuzzleState>, greater<PuzzleState>> openSet;
    unordered_set<Matrix, MatrixHash> closedSet;
    
    // Possible moves: up, down, left, right
    const int moves[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    const string move_names[4] = {"Up", "Down", "Left", "Right"};
    
    PuzzleState initialState;
    initialState.board = start;
    initialState.g = 0;
    initialState.h = manhattanDistance2(start);
    initialState.last_move = -1;
    initialState.path = {start};
    
    openSet.push(initialState);
    closedSet.insert(start);
    
    while(!openSet.empty()) {
        PuzzleState current = openSet.top();
        openSet.pop();
        
        if(current.h == 0) {
            return current.path;
        }
        
        // Find empty tile
        int zero_i, zero_j;
        for(zero_i = 0; zero_i < n; zero_i++) {
            for(zero_j = 0; zero_j < n; zero_j++) {
                if(current.board[zero_i][zero_j] == 0) goto found;
            }
        }
        found:
        
        for(int k = 0; k < 4; k++) {
            // Don't undo last move (0-1 are opposites, 2-3 are opposites)
            if((current.last_move == 0 && k == 1) || 
               (current.last_move == 1 && k == 0) ||
               (current.last_move == 2 && k == 3) || 
               (current.last_move == 3 && k == 2)) {
                continue;
            }
            
            int new_i = zero_i + moves[k][0];
            int new_j = zero_j + moves[k][1];
            
            if(new_i >= 0 && new_i < n && new_j >= 0 && new_j < n) {
                Matrix new_board = current.board;
                swap(new_board[zero_i][zero_j], new_board[new_i][new_j]);
                
                if(closedSet.find(new_board) == closedSet.end()) {
                    closedSet.insert(new_board);
                    
                    PuzzleState neighbor;
                    neighbor.board = new_board;
                    neighbor.g = current.g + 1;
                    neighbor.h = manhattanDistance2(new_board);
                    neighbor.last_move = k;
                    neighbor.path = current.path;
                    neighbor.path.push_back(new_board);
                    
                    openSet.push(neighbor);
                }
            }
        }
    }
    
    return {};
}

int main() {
    Matrix start1 = {{
        {{1, 4, 2, 3}},
        {{13, 6, 7, 8}},
        {{5, 10, 11, 12}},
        {{9, 14, 15, 0}}
    }};
    
    Matrix start = {{
        {{10, 12, 1, 2}},
        {{11, 14, 13, 8}},
        {{15, 3, 5, 4}},
        {{6, 9, 7, 0}}
    }};

    cout << "Initial state:" << endl;
    printState(start);
    
    auto solution = solvePuzzle(start);
    if(!solution.empty()) {
        cout << "\nSolution found in " << solution.size()-1 << " moves!" << endl;
        for(size_t i = 0; i < solution.size(); i++) {
            cout << "\nStep " << i << ":" << endl;
            printState(solution[i]);
        }
    } else {
        cout << "\nNo solution found!" << endl;
    }
    
    return 0;
}