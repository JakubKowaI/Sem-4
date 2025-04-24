#include <iostream>
#include <array>
#include <vector>
#include <iomanip>

using namespace std;

constexpr int n = 4;
using Matrix = array<array<int, n>, n>;  // Type alias for 4x4 array

Matrix perfectPuzzle() {
    Matrix T;
    int t = n*n - 1;
    
    for(auto& row : T) {  // Range-based for loop
        for(auto& val : row) {
            val = t--;
        }
    }
    return T;
}


void printState(const Matrix& T) {
    for(const auto& row : T) {
        for(int val : row) {
                cout << setw(3) << val;
        }
        cout << endl;
    }
}

bool areEqual(const Matrix& a, const Matrix& b) {
    return a == b;  // std::array has built-in comparison
}

int h1(const Matrix& a, const Matrix& b) {
    int c = 0;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(a[i][j] == b[i][j]) c++;   
        }
    }
    return c; // Returns count of numbers in correct positions
}

vector<Matrix> f1(const Matrix& T){
    vector<Matrix> G;
    vector<Matrix> neighbors;

    if(T == perfectPuzzle()){
        G.push_back(T);
        return G;
    }

    // Find the position of the empty tile (0)
    int zero_i = -1, zero_j = -1;
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            if(T[i][j] == 0) {
                zero_i = i;
                zero_j = j;
                break;
            }
        }
        if(zero_i != -1) break;
    }
    
    // Generate all possible moves
    const int moves[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};  // Up, Down, Left, Right
    
    for(const auto& move : moves) {
        int new_i = zero_i + move[0];
        int new_j = zero_j + move[1];
        
        // Check if the move is valid
        if(new_i >= 0 && new_i < n && new_j >= 0 && new_j < n) {
            // Create a new state by swapping the empty tile with its neighbor
            Matrix new_state = T;
            swap(new_state[zero_i][zero_j], new_state[new_i][new_j]);
            neighbors.push_back(new_state);
        }
    }
    int min = 0;
    for(Matrix temp : neighbors){
        vector<Matrix> tt = f1(temp);
        if(tt.size()<=min){
            G=tt;
        }
    }
    
    return G;
}

int main() {
    auto T = perfectPuzzle();
    printState(T);

    Matrix state = {{
        {{15, 14, 13, 12}},
        {{11, 10, 9, 8}},
        {{7, 6, 5, 4}},
        {{3, 2, 1, 0}}
    }};

    Matrix puzzle = {{
        {{1, 2, 3, 4}},
        {{5, 6, 7, 8}},
        {{9, 10, 11, 12}},
        {{13, 14, 15, 0}}
    }};
    
    cout << "Are equal: " << areEqual(T, puzzle) << endl;
    cout << "Correct positions: " << h1(T, puzzle) << endl;
    printState(f1(puzzle).back());
    return 0;
}