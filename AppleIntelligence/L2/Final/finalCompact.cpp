#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <cstdint>

using namespace std;

struct State {
    uint64_t board;
    int g;
    int h;
    uint64_t parent;

    State(uint64_t b, int g_cost, int h_cost, uint64_t p)
        : board(b), g(g_cost), h(h_cost), parent(p) {}

    int f() const {
        return g + h;
    }
};

struct CompareState {
    bool operator()(const State& a, const State& b) const {
        return a.f() > b.f();
    }
};

bool isSolvable(uint64_t board) {
    vector<int> tiles;
    int blank_row = 0;

    for (int i = 0; i < 16; ++i) {
        int val = (board >> (i * 4)) & 0xF;
        if (val == 0) {
            blank_row = i / 4;
        } else {
            tiles.push_back(val);
        }
    }

    int inversions = 0;
    for (size_t i = 0; i < tiles.size(); ++i) {
        for (size_t j = i + 1; j < tiles.size(); ++j) {
            if (tiles[i] > tiles[j]) {
                ++inversions;
            }
        }
    }

    int blank_row_from_bottom = 3 - blank_row; // 0-based index
    return (inversions + blank_row_from_bottom) % 2 == 0;
}

int manhattanDistance(uint64_t board) {
    int dist = 0;
    for (int i = 0; i < 16; ++i) {
        int val = (board >> (i * 4)) & 0xF;
        if (val == 0) continue;
        int targetRow = (val - 1) / 4;
        int targetCol = (val - 1) % 4;
        int row = i / 4;
        int col = i % 4;
        dist += abs(row - targetRow) + abs(col - targetCol);
    }
    return dist;
}

vector<pair<uint64_t, int>> getNeighbors(uint64_t board) {
    vector<pair<uint64_t, int>> neighbors;
    int zero_pos = 0;
    for (int i = 0; i < 16; ++i) {
        if (((board >> (i * 4)) & 0xF) == 0) {
            zero_pos = i;
            break;
        }
    }

    int row = zero_pos / 4;
    int col = zero_pos % 4;
    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (auto [dr, dc] : directions) {
        int new_row = row + dr;
        int new_col = col + dc;
        if (new_row >= 0 && new_row < 4 && new_col >= 0 && new_col < 4) {
            int new_pos = new_row * 4 + new_col;
            uint64_t new_board = board;
            uint64_t val = (new_board >> (new_pos * 4)) & 0xF;

            new_board &= ~(0xFULL << (new_pos * 4));
            new_board |= val << (zero_pos * 4);

            new_board &= ~(0xFULL << (zero_pos * 4));
            new_board |= 0ULL << (new_pos * 4);

            neighbors.emplace_back(new_board, 1);
        }
    }

    return neighbors;
}

void reconstructPath(unordered_map<uint64_t, uint64_t>& cameFrom, uint64_t current) {
    vector<uint64_t> totalPath = {current};
    while (cameFrom.count(current)) {
        current = cameFrom[current];
        totalPath.push_back(current);
    }
    reverse(totalPath.begin(), totalPath.end());
    for (auto state : totalPath) {
        for (int i = 0; i < 16; ++i) {
            int val = (state >> (i * 4)) & 0xF;
            cout << val << ' ';
            if ((i + 1) % 4 == 0) cout << '\n';
        }
        cout << "\n";
    }
}

void aStar(uint64_t start, uint64_t goal) {
    priority_queue<State, vector<State>, CompareState> openSet;
    unordered_map<uint64_t, int> gScore;
    unordered_map<uint64_t, uint64_t> cameFrom;
    unordered_set<uint64_t> visited;

    gScore[start] = 0;
    openSet.emplace(start, 0, manhattanDistance(start), start);

    while (!openSet.empty()) {
        State current = openSet.top();
        openSet.pop();

        if (current.board == goal) {
            reconstructPath(cameFrom, current.board);
            return;
        }

        if (visited.count(current.board)) continue;
        visited.insert(current.board);

        for (auto [neighbor, cost] : getNeighbors(current.board)) {
            int tentative_gScore = gScore[current.board] + cost;
            if (!gScore.count(neighbor) || tentative_gScore < gScore[neighbor]) {
                cameFrom[neighbor] = current.board;
                gScore[neighbor] = tentative_gScore;
                int h = manhattanDistance(neighbor);
                openSet.emplace(neighbor, tentative_gScore, h, current.board);
            }
        }
    }

    cout << "No solution found." << endl;
}

int main() {
    uint64_t start = 0x123456789ABCDEF0ULL; // Example board
    uint64_t goal = 0x123456789ABCDEF0ULL;

    // Matrix ex1 = {{
    //     {{12, 10, 14, 11}},
    //     {{9, 8, 6, 2}},
    //     {{0, 7, 15, 13}},
    //     {{4, 1, 5, 3}}
    // }};

    // Matrix ex2 = {{
    //     {{4, 12, 15, 5}},
    //     {{14, 10, 11, 13}},
    //     {{0, 7, 2, 8}},
    //     {{6, 9, 1, 3}}
    // }};

    // Matrix ex3 = {{
    //     {{7, 5, 8, 12}},
    //     {{1, 6, 9, 14}},
    //     {{10, 0, 2, 4}},
    //     {{3, 11, 13, 15}}
    // }};

    // Matrix ex4 = {{
    //     {{15, 11, 13, 12}},
    //     {{14, 10, 9, 5}},
    //     {{2, 6, 8, 1}},
    //     {{3, 7, 4, 0}}
    // }};
    //    int tiles2[] = {15, 14, 8, 12, 10, 11, 9, 13, 2, 6, 5, 1, 3, 7, 4, 0};

    uint64_t start1 = 0xCAEB986207FD4153ULL;
    uint64_t start2 = 0x4CF5EABD07286913ULL;
    uint64_t start3 = 0x758C169EA0243BDFULL;
    uint64_t start4 = 0xFBDCEA9526813740ULL;
    uint64_t start5 = 0xFE8CAB9D26513740ULL;

    cout<<"da sie 1 "<<isSolvable(start1)<<endl;
    cout<<"da sie 2 "<<isSolvable(start2)<<endl;
    cout<<"da sie 3 "<<isSolvable(start3)<<endl;
    cout<<"da sie 4 "<<isSolvable(start4)<<endl;
    cout<<"da sie 5 "<<isSolvable(start5)<<endl;

    aStar(start3, goal);

    return 0;
}
