#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <unordered_set>
#include <map>
#include <set>

using namespace std;

constexpr int n = 4;
using Matrix = array<array<int, n>, n>;
int states=0;
Matrix goal;

void printPuzzle(const Matrix& puzzle) {
    for (const auto& row : puzzle) {
        for (int val : row) {
            if (val == 0) cout << setw(3) << " ";
            else cout << setw(3) << val;
        }
        cout << endl;
}
}

Matrix createGoalState() {
    Matrix goal;
    int val = 1;
    for (auto& row : goal) {
        for (auto& cell : row) {
            cell = val++;
        }
    }
    goal[n - 1][n - 1] = 0;
    return goal;
}

vector<Matrix> getNeighbors(const Matrix& board) {
    vector<Matrix> neighbors;
    int zero_i, zero_j;

    for (zero_i = 0; zero_i < n; zero_i++) {
        for (zero_j = 0; zero_j < n; zero_j++) {
            if (board[zero_i][zero_j] == 0) break;
        }
        if (zero_j < n && board[zero_i][zero_j] == 0) break;
    }

    const int moves[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

    for (const auto& move : moves) {
        int new_i = zero_i + move[0];
        int new_j = zero_j + move[1];

        if (new_i >= 0 && new_i < n && new_j >= 0 && new_j < n) {
            Matrix new_board = board;
            swap(new_board[zero_i][zero_j], new_board[new_i][new_j]);
            neighbors.push_back(new_board);
        }
    }

    return neighbors;
}

int h1(const Matrix& a) {
    int distance = 0;
    unordered_map<int, pair<int, int>> goalPositions;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            goalPositions[goal[i][j]] = { i, j };
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int val = a[i][j];
            if (val != 0) {
                auto goalPos = goalPositions[val];
                distance += abs(i - goalPos.first) + abs(j - goalPos.second);
            }
        }
    }
    return distance;
}

vector<Matrix> path(map<Matrix, Matrix> previous, Matrix& state) {
    vector<Matrix> fullPath = { state };
    auto search = previous.find(state);
    while (search != previous.end()) {
        state = search->second;
        fullPath.insert(fullPath.begin(), state);
        search = previous.find(state);
    }
    return fullPath;
}

vector<Matrix> solve(const Matrix& start) {
    multimap<int, pair<Matrix, int>> possible;  // Store <f, <Matrix, g>>
    unordered_set<string> visited; // To keep track of visited states

    possible.insert({ h1(start), { start, 0 } });  // Start with f = h1(start) and g = 0
    visited.insert(to_string(start[0][0]) + to_string(start[0][1]) + to_string(start[0][2]) + to_string(start[0][3]) +
        to_string(start[1][0]) + to_string(start[1][1]) + to_string(start[1][2]) + to_string(start[1][3]) +
        to_string(start[2][0]) + to_string(start[2][1]) + to_string(start[2][2]) + to_string(start[2][3]) +
        to_string(start[3][0]) + to_string(start[3][1]) + to_string(start[3][2]) + to_string(start[3][3]));  // Serialize the matrix for visited check

    map<Matrix, Matrix> previous;
    map<Matrix, int> gCost;
    gCost.insert({ start, 0 });

    Matrix current;

    while (!possible.empty()) {
        current = possible.begin()->second.first;  // Extract Matrix from the pair

        states++;

        if (current == goal) {
            return path(previous, current);
        }

        possible.erase(possible.begin());

        auto neighbors = getNeighbors(current);

        for (const Matrix& neighbor : neighbors) {
            // Serialize neighbor matrix
            string neighbor_str = to_string(neighbor[0][0]) + to_string(neighbor[0][1]) + to_string(neighbor[0][2]) + to_string(neighbor[0][3]) +
                to_string(neighbor[1][0]) + to_string(neighbor[1][1]) + to_string(neighbor[1][2]) + to_string(neighbor[1][3]) +
                to_string(neighbor[2][0]) + to_string(neighbor[2][1]) + to_string(neighbor[2][2]) + to_string(neighbor[2][3]) +
                to_string(neighbor[3][0]) + to_string(neighbor[3][1]) + to_string(neighbor[3][2]) + to_string(neighbor[3][3]);

            if (visited.find(neighbor_str) == visited.end()) {
                visited.insert(neighbor_str);

                int tentative_g = gCost[current] + 1;  // Cost to reach neighbor

                if (gCost.find(neighbor) == gCost.end() || tentative_g < gCost[neighbor]) {
                    gCost[neighbor] = tentative_g;
                    previous[neighbor] = current;

                    int f = tentative_g + h1(neighbor);  // f = g + h
                    possible.insert({ f, { neighbor, tentative_g } });  // Insert with new f and g values
                }
            }
        }
    }

    return {};  // No solution found
}

int main() {
    goal = createGoalState();

    Matrix start = {{
        {{10, 12, 1, 2}},
        {{11, 14, 13, 8}},
        {{15, 3, 5, 4}},
        {{6, 9, 7, 0}}
    }};

    cout << "Initial state:" << endl;
    printPuzzle(start);

    auto solution = solve(start);
    if (!solution.empty()) {
        cout << "\nSolution found in " << solution.size() - 1 << " moves!" << endl;
        for (size_t i = 0; i < solution.size(); i++) {
            cout << "\nStep " << i << ":" << endl;
            printPuzzle(solution[i]);
        }
        cout<<"\nChecked "<<states<<" states"<<endl;
    }
    else {
        cout << "\nNo solution found!" << endl;
    }

    return 0;
}
