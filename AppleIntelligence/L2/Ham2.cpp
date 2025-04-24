#include <iostream>
#include <array>
#include <vector>
#include <set>
#include <unordered_set>
#include <iomanip>
#include <algorithm>

using namespace std;

constexpr int n = 4;
using Matrix = array<array<int, n>, n>;

Matrix createGoalState() {
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

void printPuzzle(const Matrix& puzzle) {
    for(const auto& row : puzzle) {
        for(int val : row) {
            if(val == 0) cout << setw(3) << " ";
            else cout << setw(3) << val;
        }
        cout << endl;
    }
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

bool solveDFS(const Matrix& current, const Matrix& goal, 
             vector<Matrix>& path, set<Matrix>& visited, 
             int depth = 0, int max_depth = 20) {
    if(depth > max_depth) return false;
    
    if(current == goal) {
        path.push_back(current);
        return true;
    }
    
    visited.insert(current);
    
    for(const Matrix& neighbor : getNeighbors(current)) {
        if(visited.find(neighbor) == visited.end()) {
            if(solveDFS(neighbor, goal, path, visited, depth+1, max_depth)) {
                path.push_back(current);
                return true;
            }
        }
    }
    
    visited.erase(current);
    return false;
}

vector<Matrix> solvePuzzle(const Matrix& start) {
    const Matrix goal = createGoalState();
    set<Matrix> visited;
    vector<Matrix> solution;
    
    // Try with increasing depth limits
    for(int depth = 1; depth <= 30; depth++) {
        visited.clear();
        solution.clear();
        if(solveDFS(start, goal, solution, visited, 0, depth)) {
            reverse(solution.begin(), solution.end());
            return solution;
        }
    }
    
    return {};
}

int main() {
    Matrix start = {{
        {{1, 2, 3, 4}},
        {{5, 6, 7, 8}},
        {{9, 10, 11, 12}},
        {{13, 14, 15, 0}}
    }};
    
    cout << "Initial puzzle:" << endl;
    printPuzzle(start);
    
    cout << "\nSolving..." << endl;
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