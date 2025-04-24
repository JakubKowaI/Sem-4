#include <iostream>
#include <array>
#include <vector>
#include <queue>
#include <unordered_map>
#include <iomanip>
#include <functional>
#include <limits>

using namespace std;

constexpr int n = 4;
constexpr int MAX_DEPTH = 50;
using Matrix = array<array<int, n>, n>;

// State representation with additional A* data
struct PuzzleState {
    Matrix board;
    int g;  // cost from start
    int h;  // heuristic to goal
    int f;  // g + h
    vector<Matrix> path; // path to this state
    
    bool operator>(const PuzzleState& other) const {
        return f > other.f;
    }
};

// For hashing the board state
struct MatrixHash {
    size_t operator()(const Matrix& m) const {
        size_t hash = 0;
        for (const auto& row : m) {
            for (int val : row) {
                hash ^= hash << 13;
                hash ^= hash >> 7;
                hash ^= hash << 17;
                hash += val;
            }
        }
        return hash;
    }
};

Matrix perfectPuzzle() {
    Matrix T;
    int t = n*n - 1;
    for(auto& row : T) {
        for(auto& val : row) {
            val = t--;
        }
    }
    return T;
}

void printState(const Matrix& T) {
    for(const auto& row : T) {
        for(int val : row) {
            if(val == 0) cout << setw(3) << " ";
            else cout << setw(3) << val;
        }
        cout << endl;
    }
}

// Manhattan distance heuristic
int manhattanDistance(const Matrix& a, const Matrix& goal) {
    int distance = 0;
    unordered_map<int, pair<int, int>> goalPositions;
    
    // Create a map of value to position in goal state
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            goalPositions[goal[i][j]] = {i, j};
        }
    }
    
    // Calculate Manhattan distance for each tile
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            int val = a[i][j];
            if(val != 0) {  // Don't count the empty tile
                auto goalPos = goalPositions[val];
                distance += abs(i - goalPos.first) + abs(j - goalPos.second);
            }
        }
    }
    return distance;
}

vector<Matrix> getNeighbors(const Matrix& T) {
    vector<Matrix> neighbors;
    
    // Find the empty tile (0)
    int zero_i = -1, zero_j = -1;
    for(int i = 0; i < n && zero_i == -1; ++i) {
        for(int j = 0; j < n; ++j) {
            if(T[i][j] == 0) {
                zero_i = i;
                zero_j = j;
                break;
            }
        }
    }
    
    // Possible moves: up, down, left, right
    const int moves[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    
    for(const auto& move : moves) {
        int new_i = zero_i + move[0];
        int new_j = zero_j + move[1];
        
        if(new_i >= 0 && new_i < n && new_j >= 0 && new_j < n) {
            Matrix new_state = T;
            swap(new_state[zero_i][zero_j], new_state[new_i][new_j]);
            neighbors.push_back(new_state);
        }
    }
    
    return neighbors;
}

vector<Matrix> aStar(const Matrix& start, const Matrix& goal) {
    priority_queue<PuzzleState, vector<PuzzleState>, greater<PuzzleState>> openSet;
    unordered_map<Matrix, int, MatrixHash> gScores;
    
    // Initial state
    PuzzleState initialState;
    initialState.board = start;
    initialState.g = 0;
    initialState.h = manhattanDistance(start, goal);
    initialState.f = initialState.g + initialState.h;
    initialState.path = {start};
    
    openSet.push(initialState);
    gScores[start] = 0;
    
    while(!openSet.empty()) {
        PuzzleState current = openSet.top();
        openSet.pop();
        
        // Check if we've reached the goal
        if(current.board == goal) {
            return current.path;
        }
        
        // If we found a better path already, skip
        if(current.g > gScores[current.board]) {
            continue;
        }
        
        if(current.g > MAX_DEPTH) {
            continue; // Skip paths that are too long
        }
        
        // Explore neighbors
        for(const Matrix& neighbor : getNeighbors(current.board)) {
            int tentative_g = current.g + 1; // Each move costs 1
            
            // If this path to neighbor is better
            if(gScores.find(neighbor) == gScores.end() || tentative_g < gScores[neighbor]) {
                gScores[neighbor] = tentative_g;
                
                PuzzleState newState;
                newState.board = neighbor;
                newState.g = tentative_g;
                newState.h = manhattanDistance(neighbor, goal);
                newState.f = newState.g + newState.h;
                newState.path = current.path;
                newState.path.push_back(neighbor);
                
                openSet.push(newState);
            }
        }
    }
    
    return {}; // No solution found (shouldn't happen for solvable puzzles)
}

int main() {
    Matrix start = {{
        {{1, 2, 3, 4}},
        {{5, 6, 7, 8}},
        {{9, 10, 11, 12}},
        {{13, 14, 15, 0}}
    }};
    
    Matrix goal = perfectPuzzle();
    
    cout << "Initial state:" << endl;
    printState(start);
    
    cout << "\nGoal state:" << endl;
    printState(goal);
    
    cout << "\nSolving with A*..." << endl;
    auto solutionPath = aStar(start, goal);
    
    if(solutionPath.empty()) {
        cout << "No solution found!" << endl;
    } else {
        cout << "\nSolution found in " << solutionPath.size()-1 << " moves:" << endl;
        for(size_t i = 0; i < solutionPath.size(); i++) {
            cout << "\nStep " << i << ":" << endl;
            printState(solutionPath[i]);
        }
    }
    
    return 0;
}