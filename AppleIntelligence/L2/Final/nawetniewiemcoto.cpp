#include <iostream>
#include <vector>
#include <array>
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
using CompactState = array<char, 8>; // Reverting to array<char,8> for portability
int states;
CompactState goal;

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

// Custom hash function for CompactState
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

bool isSolvable(const array<char, 8> state) {
    int inv = 0;
    int zero_row_from_bottom = 0;
    const int size = 4;
    int tiles[16];

    // Rozpakuj wartości 0–15 z tablicy 8 bajtów
    for (int i = 0; i < 8; ++i) {
        tiles[2 * i]     = (state[i] >> 4) & 0xF;
        tiles[2 * i + 1] = state[i] & 0xF;
    }

    // Oblicz liczbę inwersji
    for (int i = 0; i < 16; ++i) {
        if (tiles[i] == 0) {
            zero_row_from_bottom = 3 - (i / 4); // rząd od dołu (0-indexed)
            continue;
        }
        for (int j = i + 1; j < 16; ++j) {
            if (tiles[j] != 0 && tiles[i] > tiles[j]) {
                ++inv;
            }
        }
    }

    // Dla 4x4: parzysty wiersz od dołu → inwersje nieparzyste
    //         nieparzysty wiersz od dołu → inwersje parzyste
    return (zero_row_from_bottom % 2 == 0) ? (inv % 2 == 1) : (inv % 2 == 0);
}


// Manhattan distance + linear conflict heuristic
int h1(const CompactState& s) {
    int dist = 0;
    int linear_conflict = 0;
    
    // Manhattan distance
    for (int i = 0; i < 16; ++i) {
        int val = getTile(s, i);
        if (val == 0) continue;
        int goal_row = (val - 1) / 4;
        int goal_col = (val - 1) % 4;
        int cur_row = i / 4;
        int cur_col = i % 4;
        dist += abs(goal_row - cur_row) + abs(goal_col - cur_col);
    }
    
    // Linear conflict - rows
    for (int row = 0; row < 4; row++) {
        for (int i = 0; i < 4; i++) {
            int val1 = getTile(s, row*4 + i);
            if (val1 == 0 || (val1-1)/4 != row) continue;
            for (int j = i+1; j < 4; j++) {
                int val2 = getTile(s, row*4 + j);
                if (val2 != 0 && (val2-1)/4 == row && val1 > val2) 
                    linear_conflict += 2;
            }
        }
    }
    
    // Linear conflict - columns
    for (int col = 0; col < 4; col++) {
        for (int i = 0; i < 4; i++) {
            int val1 = getTile(s, i*4 + col);
            if (val1 == 0 || (val1-1)%4 != col) continue;
            for (int j = i+1; j < 4; j++) {
                int val2 = getTile(s, j*4 + col);
                if (val2 != 0 && (val2-1)%4 == col && val1 > val2) 
                    linear_conflict += 2;
            }
        }
    }
    
    return dist + linear_conflict;
}

// Thread-safe shared data
struct SharedSearchData {
    mutex mtx;
    condition_variable cv;
    unordered_map<CompactState, CompactState, CompactStateHash> came_from_start;
    unordered_map<CompactState, CompactState, CompactStateHash> came_from_goal;
    unordered_map<CompactState, int, CompactStateHash> g_score_start;
    unordered_map<CompactState, int, CompactStateHash> g_score_goal;
    CompactState meeting_point;
    atomic<int> best_cost{INT_MAX};
    atomic<bool> found{false};
    atomic<bool> start_done{false};
    atomic<bool> goal_done{false};
    const int max_depth = 81;
};

// Priority queue wrapper that's more efficient than std::priority_queue
template<typename T, typename Priority>
struct FastPriorityQueue {
    vector<pair<Priority, T>> elements;
    
    void push(T item, Priority priority) {
        elements.emplace_back(priority, item);
        push_heap(elements.begin(), elements.end(), greater<pair<Priority, T>>());
    }
    
    T pop() {
        pop_heap(elements.begin(), elements.end(), greater<pair<Priority, T>>());
        T item = elements.back().second;
        elements.pop_back();
        return item;
    }
    
    bool empty() const { return elements.empty(); }
};



vector<CompactState> getNeighbors(const CompactState& s) {
    vector<CompactState> neighbors;
    int zero_pos;
    for (int i = 0; i < 16; ++i) {
        if (getTile(s, i) == 0) {
            zero_pos = i;
            break;
        }
    }
    
    const int moves[] = {-4, +4, -1, +1};
    for (int d : moves) {
        int new_pos = zero_pos + d;
        if (new_pos < 0 || new_pos >= 16) continue;
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

struct ThreadLocalData {
    unordered_map<CompactState, CompactState, CompactStateHash> came_from;
    unordered_map<CompactState, int, CompactStateHash> g_score;
};

void aStarFromStart(const CompactState& start, SharedSearchData& shared) {
    FastPriorityQueue<CompactState, int> open_set;
    ThreadLocalData local;
    const int merge_interval = 10000; // Merge every 10k nodes
    
    {
        lock_guard<mutex> lock(shared.mtx);
        shared.g_score_start[start] = 0;
    }
    local.g_score[start] = 0;
    open_set.push(start, h1(start));

    while (!open_set.empty() && !shared.found) {
        CompactState current = open_set.pop();
        states++;
        
        int current_g_score = local.g_score[current];
        if (current_g_score > shared.max_depth) continue;

        // Periodic merging
        if (states % merge_interval == 0) {
            lock_guard<mutex> lock(shared.mtx);
            for (auto& [state, g] : local.g_score) {
                if (!shared.g_score_start.count(state) || g < shared.g_score_start[state]) {
                    shared.g_score_start[state] = g;
                    shared.came_from_start[state] = local.came_from[state];
                }
            }
        }

        // Check for solution
        {
            lock_guard<mutex> lock(shared.mtx);
            if (shared.g_score_goal.count(current)) {
                int total_cost = current_g_score + shared.g_score_goal[current];
                if (total_cost < shared.best_cost) {
                    // Update best solution
                    shared.best_cost = total_cost;
                    shared.meeting_point = current;
                    shared.found = true;
                    shared.cv.notify_one();
                    return;
                }
            }
        }

        // Expand neighbors
        for (const CompactState& neighbor : getNeighbors(current)) {
            int tentative_g_score = current_g_score + 1;
            if (tentative_g_score > shared.max_depth) continue;
            
            if (!local.g_score.count(neighbor) || tentative_g_score < local.g_score[neighbor]) {
                local.came_from[neighbor] = current;
                local.g_score[neighbor] = tentative_g_score;
                open_set.push(neighbor, tentative_g_score + h1(neighbor));
            }
        }
    }

    // Final merge before exiting
    lock_guard<mutex> lock(shared.mtx);
    for (auto& [state, g] : local.g_score) {
        if (!shared.g_score_start.count(state) || g < shared.g_score_start[state]) {
            shared.g_score_start[state] = g;
            shared.came_from_start[state] = local.came_from[state];
        }
    }
    shared.start_done = true;
    shared.cv.notify_one();
}

void aStarFromGoal(SharedSearchData& shared) {
    FastPriorityQueue<CompactState, int> open_set;
    
    {
        lock_guard<mutex> lock(shared.mtx);
        shared.g_score_goal[goal] = 0;
        open_set.push(goal, h1(goal));
    }

    while (!open_set.empty() && !shared.found) {
        CompactState current = open_set.pop();
        states++;
        
        int current_g_score;
        {
            lock_guard<mutex> lock(shared.mtx);
            current_g_score = shared.g_score_goal[current];
            
            if (current_g_score > shared.max_depth) continue;
            
            if (shared.g_score_start.count(current)) {
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
            
            if (tentative_g_score > shared.max_depth) continue;
            
            lock_guard<mutex> lock(shared.mtx);
            if (!shared.g_score_goal.count(neighbor) || 
                tentative_g_score < shared.g_score_goal[neighbor]) {
                shared.came_from_goal[neighbor] = current;
                shared.g_score_goal[neighbor] = tentative_g_score;
                int f_score = tentative_g_score + h1(neighbor);
                open_set.push(neighbor, f_score);
            }
        }
    }

    shared.goal_done = true;
    shared.cv.notify_one();
}

vector<CompactState> reconstructPath(unordered_map<CompactState, CompactState, CompactStateHash>& came_from, CompactState current) {
    vector<CompactState> path = {current};
    while (came_from.count(current)) {
        current = came_from[current];
        path.insert(path.begin(), current);
    }
    return path;
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
    
    vector<CompactState> path_start = reconstructPath(shared.came_from_start, shared.meeting_point);
    vector<CompactState> path_goal = reconstructPath(shared.came_from_goal, shared.meeting_point);
    
    path_start.insert(path_start.end(), path_goal.begin() + 1, path_goal.end());
    return path_start;
}

CompactState createGoalState() {
    CompactState g{};
    for (int i = 0; i < 15; i++) {
        setTile(g, i, i + 1);
    }
    setTile(g, 15, 0);
    return g;
}

void printCompact(const CompactState& s) {
    for (int i = 0; i < 16; ++i) {
        int val = getTile(s, i);
        if (val == 0) cout << setw(3) << " ";
        else cout << setw(3) << val;
        if (i % 4 == 3) cout << "\n";
    }
}

int main() {
    goal = createGoalState();
    states = 0;
    
    // Hardest 80-move puzzle
    CompactState hard_puzzle{};
    int tiles[] = {15, 11, 13, 12, 14, 10, 9, 5, 2, 6, 8, 1, 3, 7, 4, 0};
    int tiles1[] = {12, 10, 14, 11, 9, 8, 6, 2, 0, 7, 15, 13, 4, 1, 5, 3};
    int tiles2[] = {15, 14, 8, 12, 10, 11, 9, 13, 2, 6, 5, 1, 3, 7, 4, 0};

    for (int i = 0; i < 16; i++) {
        setTile(hard_puzzle, i, tiles2[i]);
    }
    // if(isSolvable(hard_puzzle)){
    //     cout<<"Nie do ulozenia"<<endl;
    //     return 0;
    // }
    auto start_time = chrono::steady_clock::now();
    auto solution = threadedBidirectionalAStar(hard_puzzle);
    auto end_time = chrono::steady_clock::now();
    
    
    if (!solution.empty()) {
        cout << "Solution found in " << solution.size()-1 << " moves!\n";
        cout << "States explored: " << states << "\n";
        cout << "Time: " 
             << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() 
             << " ms\n";
        
        // Uncomment to print solution
        // for (const auto& state : solution) {
        //     printCompact(state);
        //     cout << "\n";
        // }
    } else {
        cout << "No solution found!\n";
    }
    
    return 0;
}