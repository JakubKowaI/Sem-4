#include <iostream>
#include <array>
#include <vector>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <climits>
#include <iomanip>
#include <map>
#include <random>
#include <algorithm>
#include <functional>

using namespace std;

constexpr int n = 4;
using Matrix = array<array<int, n>, n>;
using CompactState = array<char, 8>;
int states;
CompactState goal;

// Custom comparison for CompactState (needed for std::map)
struct CompactStateCompare {
    bool operator()(const CompactState& a, const CompactState& b) const {
        return a < b;
    }
};

// Custom hash function for CompactState (for unordered containers)
struct CompactStateHash {
    size_t operator()(const CompactState& s) const {
        size_t hash = 0;
        for (char c : s) {
            hash ^= hash << 13;
            hash ^= hash >> 7;
            hash ^= hash << 17;
            hash += c;
        }
        return hash;
    }
};

// Custom equality for CompactState
struct CompactStateEqual {
    bool operator()(const CompactState& a, const CompactState& b) const {
        return a == b;
    }
};

// Thread-safe priority queue
template<typename T, typename priority_t>
struct PriorityQueue {
    typedef pair<priority_t, T> PQElement;
    priority_queue<PQElement, vector<PQElement>, greater<PQElement>> elements;
    unordered_map<T, priority_t, CompactStateHash, CompactStateEqual> element_priorities;

    bool empty() const { return elements.empty(); }

    void put(T item, priority_t priority) {
        elements.emplace(priority, item);
        element_priorities[item] = priority;
    }

    T get() {
        T best_item = elements.top().second;
        elements.pop();
        element_priorities.erase(best_item);
        return best_item;
    }

    bool contains(T item) const {
        return element_priorities.find(item) != element_priorities.end();
    }

    priority_t get_priority(T item) const {
        auto it = element_priorities.find(item);
        return it != element_priorities.end() ? it->second : numeric_limits<priority_t>::max();
    }
};

// Modified SharedSearchData to include max_depth
struct SharedSearchData {
    mutex mtx;
    condition_variable cv;
    map<CompactState, CompactState, CompactStateCompare> came_from_start;
    map<CompactState, CompactState, CompactStateCompare> came_from_goal;
    unordered_map<CompactState, int, CompactStateHash, CompactStateEqual> g_score_start;
    unordered_map<CompactState, int, CompactStateHash, CompactStateEqual> g_score_goal;
    CompactState meeting_point;
    atomic<int> best_cost{INT_MAX};
    atomic<bool> found{false};
    atomic<bool> start_done{false};
    atomic<bool> goal_done{false};
    const int max_depth = 81;  // Maximum allowed depth
};

inline int getTile(const CompactState& s, int idx) {
    char byte = s[idx / 2];
    if (idx % 2 == 0) return (byte >> 4) & 0xF;
    else return byte & 0xF;
}

inline void setTile(CompactState& s, int idx, int value) {
    char& byte = s[idx / 2];
    if (idx % 2 == 0) {
        byte = (byte & 0x0F) | ((value & 0xF) << 4);
    } else {
        byte = (byte & 0xF0) | (value & 0xF);
    }
}

CompactState matrixToCompact(const Matrix& m) {
    CompactState s{};
    for (int i = 0; i < 16; ++i) {
        setTile(s, i, m[i / 4][i % 4]);
    }
    return s;
}

void printCompact(const CompactState& s) {
    for (int i = 0; i < 16; ++i) {
        int val = getTile(s, i);
        if (val == 0) cout << setw(3) << " ";
        else cout << setw(3) << val;
        if (i % 4 == 3) cout << "\n";
    }
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

bool isSolvable(const CompactState& state) {
    int inv = 0;
    int zero_row_from_bottom = 0;
    int tiles[16];

    for (int i = 0; i < 16; ++i) {
        tiles[i] = getTile(state, i);
    }

    for (int i = 0; i < 16; ++i) {
        if (tiles[i] == 0) {
            zero_row_from_bottom = 3 - (i / 4);
            continue;
        }
        for (int j = i + 1; j < 16; ++j) {
            if (tiles[j] != 0 && tiles[i] > tiles[j]) {
                ++inv;
            }
        }
    }

    return (zero_row_from_bottom % 2 == 0) ? (inv % 2 == 1) : (inv % 2 == 0);
}

CompactState createGoalState() {
    CompactState goal;
    goal[0]=18;
    goal[1]=52;
    goal[2]=86;
    goal[3]=120;
    goal[4]=154;
    goal[5]=188;
    goal[6]=222;
    goal[7]=240;
    return goal;
}

vector<CompactState> getNeighbors(const CompactState& s) {
    vector<CompactState> neighbors;
    int zero_pos;
    for (int i = 0; i < 16; ++i) {
        if (getTile(s, i) == 0) {
            zero_pos = i;
            break;
        }
    }
    const int moves[] = {-4, +4, -1, +1}; // up, down, left, right
    for (int d : moves) {
        int new_pos = zero_pos + d;
        if (new_pos < 0 || new_pos >= 16) continue;
        // disallow wrap around in rows
        if (d == -1 && zero_pos % 4 == 0) continue;
        if (d == +1 && zero_pos % 4 == 3) continue;
        CompactState new_state = s;
        int val = getTile(s, new_pos);
        setTile(new_state, zero_pos, val);
        setTile(new_state, new_pos, 0);
        neighbors.push_back(new_state);
    }
    return neighbors;
}

CompactState generateSolvablePuzzle() {
    random_device rd;
    mt19937 gen(rd());
    CompactState puzzle = goal; 

    do {
        for (int i = 0; i < 15; i++) {
            uniform_int_distribution<> dis(0, 14);
            int j = dis(gen);
            int val_i = getTile(puzzle, i);
            int val_j = getTile(puzzle, j);
            setTile(puzzle, i, val_j);
            setTile(puzzle, j, val_i);
        }
    } while (!isSolvable(puzzle));
    
    return puzzle;
}

CompactState makeRandomMoves(int k) {
    random_device rd;
    mt19937 gen(rd());
    CompactState current = goal;
    CompactState previous = goal;
    
    for (int i = 0; i < k; i++) {
        auto neighbors = getNeighbors(current);
        
        // Remove the previous state to avoid going back
        neighbors.erase(
            remove_if(neighbors.begin(), neighbors.end(), 
                     [&previous](const CompactState& m) { return m == previous; }),
            neighbors.end()
        );
        
        if (neighbors.empty()) break;
        
        uniform_int_distribution<> dis(0, neighbors.size()-1);
        previous = current;
        current = neighbors[dis(gen)];
    }
    
    return current;
}

int h1(const CompactState& s) {
    int dist = 0;
    for (int i = 0; i < 16; ++i) {
        int val = getTile(s, i);
        if (val == 0) continue;
        int goal_row = (val - 1) / 4;
        int goal_col = (val - 1) % 4;
        int cur_row = i / 4;
        int cur_col = i % 4;
        dist += abs(goal_row - cur_row) + abs(goal_col - cur_col);
    }
    return dist;
}

int h2(const CompactState& s) {
    int dist = 0;
    
    // Hamming distance (misplaced tiles)
    for (int i = 0; i < 16; ++i) {
        int val = getTile(s, i);
        if (val != 0) {
            int goal_row = (val - 1) / 4;
            int goal_col = (val - 1) % 4;
            int cur_row = i / 4;
            int cur_col = i % 4;
            dist += abs(goal_row - cur_row) + abs(goal_col - cur_col);
        }
    }

    // Linear conflict
    for (int row = 0; row < 4; ++row) {
        for (int col1 = 0; col1 < 4; ++col1) {
            for (int col2 = col1 + 1; col2 < 4; ++col2) {
                int tile1 = getTile(s, row * 4 + col1);
                int tile2 = getTile(s, row * 4 + col2);
                
                // Check if the tiles are misplaced and in conflict
                if (tile1 != 0 && tile2 != 0) {
                    int goal_row1 = (tile1 - 1) / 4;
                    int goal_col1 = (tile1 - 1) % 4;
                    int goal_row2 = (tile2 - 1) / 4;
                    int goal_col2 = (tile2 - 1) % 4;
                    
                    // Both tiles are in the same row but reversed relative to their goal positions
                    if (goal_row1 == goal_row2 && goal_col1 > goal_col2) {
                        dist += 2; // Linear conflict requires two extra moves
                    }
                }
            }
        }
    }

    for (int col = 0; col < 4; ++col) {
        for (int row1 = 0; row1 < 4; ++row1) {
            for (int row2 = row1 + 1; row2 < 4; ++row2) {
                int tile1 = getTile(s, row1 * 4 + col);
                int tile2 = getTile(s, row2 * 4 + col);
                
                // Check if the tiles are misplaced and in conflict
                if (tile1 != 0 && tile2 != 0) {
                    int goal_row1 = (tile1 - 1) / 4;
                    int goal_col1 = (tile1 - 1) % 4;
                    int goal_row2 = (tile2 - 1) / 4;
                    int goal_col2 = (tile2 - 1) % 4;
                    
                    // Both tiles are in the same column but reversed relative to their goal positions
                    if (goal_col1 == goal_col2 && goal_row1 > goal_row2) {
                        dist += 2; // Linear conflict requires two extra moves
                    }
                }
            }
        }
    }

    return dist;
}

vector<CompactState> path(map<CompactState, CompactState, CompactStateCompare> previous, CompactState& state) {
    vector<CompactState> fullPath = {state};
    auto search = previous.find(state);
    while(search != previous.end()) {
        state = search->second;
        fullPath.insert(fullPath.begin(), state);
        search = previous.find(state);
    } 
    return fullPath;
}

// Improved thread function for searching from start
void aStarFromStart(const CompactState& start, SharedSearchData& shared) {
    PriorityQueue<CompactState, int> open_set;
    open_set.put(start, h1(start));

    {
        lock_guard<mutex> lock(shared.mtx);
        shared.g_score_start[start] = 0;
    }

    while (!open_set.empty() && !shared.found) {
        CompactState current = open_set.get();
        states++;

        int current_g_score;
        {
            lock_guard<mutex> lock(shared.mtx);
            current_g_score = shared.g_score_start[current];
            
            // Skip processing if we're already beyond max depth
            if (current_g_score > shared.max_depth) {
                continue;
            }

            if (shared.g_score_goal.find(current) != shared.g_score_goal.end()) {
                int total_cost = current_g_score + shared.g_score_goal[current];
                if (total_cost < shared.best_cost) {
                    shared.best_cost = total_cost;
                    shared.meeting_point = current;
                    shared.found = true;
                    shared.cv.notify_one();
                    return;
                }
            }
        }

        for (const CompactState& neighbor : getNeighbors(current)) {
            int tentative_g_score = current_g_score + 1;

            // Skip neighbors that would exceed max depth
            if (tentative_g_score > shared.max_depth) {
                continue;
            }

            lock_guard<mutex> lock(shared.mtx);
            if (shared.g_score_start.find(neighbor) == shared.g_score_start.end() || 
                tentative_g_score < shared.g_score_start[neighbor]) {
                shared.came_from_start[neighbor] = current;
                shared.g_score_start[neighbor] = tentative_g_score;
                int f_score = tentative_g_score + h1(neighbor);
                open_set.put(neighbor, f_score);
            }
        }
    }

    shared.start_done = true;
    shared.cv.notify_one();
}

// Improved thread function for searching from goal
void aStarFromGoal(SharedSearchData& shared) {
    PriorityQueue<CompactState, int> open_set;
    open_set.put(goal, h1(goal));

    {
        lock_guard<mutex> lock(shared.mtx);
        shared.g_score_goal[goal] = 0;
    }

    while (!open_set.empty() && !shared.found) {
        CompactState current = open_set.get();
        states++;

        int current_g_score;
        {
            lock_guard<mutex> lock(shared.mtx);
            current_g_score = shared.g_score_goal[current];
            
            // Skip processing if we're already beyond max depth
            if (current_g_score > shared.max_depth) {
                continue;
            }

            if (shared.g_score_start.find(current) != shared.g_score_start.end()) {
                int total_cost = shared.g_score_start[current] + current_g_score;
                if (total_cost < shared.best_cost) {
                    shared.best_cost = total_cost;
                    shared.meeting_point = current;
                    shared.found = true;
                    shared.cv.notify_one();
                    return;
                }
            }
        }

        for (const CompactState& neighbor : getNeighbors(current)) {
            int tentative_g_score = current_g_score + 1;

            // Skip neighbors that would exceed max depth
            if (tentative_g_score > shared.max_depth) {
                continue;
            }

            lock_guard<mutex> lock(shared.mtx);
            if (shared.g_score_goal.find(neighbor) == shared.g_score_goal.end() || 
                tentative_g_score < shared.g_score_goal[neighbor]) {
                shared.came_from_goal[neighbor] = current;
                shared.g_score_goal[neighbor] = tentative_g_score;
                int f_score = tentative_g_score + h1(neighbor);
                open_set.put(neighbor, f_score);
            }
        }
    }

    shared.goal_done = true;
    shared.cv.notify_one();
}

vector<CompactState> threadedBidirectionalAStar(const CompactState& start) {
    SharedSearchData shared;
    
    thread start_thread(aStarFromStart, start, ref(shared));
    thread goal_thread(aStarFromGoal, ref(shared));
    
    {
        unique_lock<mutex> lock(shared.mtx);
        shared.cv.wait(lock, [&shared] {
            return shared.found || (shared.start_done && shared.goal_done);
        });
    }
    
    start_thread.join();
    goal_thread.join();
    
    if (!shared.found) {
        return {};
    }
    
    // Reconstruct path
    vector<CompactState> path_start;
    CompactState current = shared.meeting_point;
    while (current != start) {
        path_start.push_back(current);
        current = shared.came_from_start[current];
    }
    path_start.push_back(start);
    reverse(path_start.begin(), path_start.end());
    
    vector<CompactState> path_goal;
    current = shared.meeting_point;
    while (current != goal) {
        current = shared.came_from_goal[current];
        path_goal.push_back(current);
    }
    
    path_start.insert(path_start.end(), path_goal.begin(), path_goal.end());
    return path_start;
}

int main() {
    goal = createGoalState();
    states=0;
    Matrix ex1 = {{
        {{12, 10, 14, 11}},
        {{9, 8, 6, 2}},
        {{0, 7, 15, 13}},
        {{4, 1, 5, 3}}
    }};

    Matrix ex2 = {{
        {{4, 12, 15, 5}},
        {{14, 10, 11, 13}},
        {{0, 7, 2, 8}},
        {{6, 9, 1, 3}}
    }};

    Matrix ex3 = {{
        {{7, 5, 8, 12}},
        {{1, 6, 9, 14}},
        {{10, 0, 2, 4}},
        {{3, 11, 13, 15}}
    }};

    Matrix ex4 = {{
        {{15, 11, 13, 12}},
        {{14, 10, 9, 5}},
        {{2, 6, 8, 1}},
        {{3, 7, 4, 0}}
    }};

    Matrix ex5 = {{
        {{0, 12, 9, 13}},
        {{15, 11, 10, 14}},
        {{7, 8, 5, 6}},
        {{4, 3, 2, 1}}
    }};

    auto solution = threadedBidirectionalAStar(matrixToCompact(ex5));
    if(solution.size()!=0)cout<<"Steps: "<<solution.size()<< " States visited: "<<states<<endl;
    for (const auto& state : solution) {
        printCompact(state);
        cout << endl;
    }

    return 0;
}