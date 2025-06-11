#include <iostream>
#include <vector>
#include <tuple>
#include <queue>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <unordered_set>

using namespace std;

struct Edge {
    int u, v;
    double weight;
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

vector<Edge> generate_complete_graph(int n) {
    vector<Edge> edges;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    for (int u = 0; u < n; ++u) {
        for (int v = u + 1; v < n; ++v) {
            double weight = dis(gen);
            edges.push_back({u, v, weight});
        }
    }
    return edges;
}

struct DSU {
    vector<int> parent, rank;
    DSU(int n) : parent(n), rank(n, 0) {
        for (int i = 0; i < n; ++i)
            parent[i] = i;
    }
    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    }
    bool unite(int x, int y) {
        int xr = find(x), yr = find(y);
        if (xr == yr) return false;
        if (rank[xr] < rank[yr])
            parent[xr] = yr;
        else if (rank[xr] > rank[yr])
            parent[yr] = xr;
        else {
            parent[yr] = xr;
            rank[xr]++;
        }
        return true;
    }
};

// global trees
vector<vector<int>> mst_tree_kruskal, mst_tree_prim;

// Kruskal's algorithm that can also build MST tree
double kruskal(int n, const vector<Edge>& edges, bool build_tree = false) {
    vector<Edge> sorted_edges = edges;
    sort(sorted_edges.begin(), sorted_edges.end());
    DSU dsu(n);
    double total_weight = 0.0;
    int count = 0;

    if (build_tree) mst_tree_kruskal.assign(n, {});

    for (const auto& e : sorted_edges) {
        if (dsu.unite(e.u, e.v)) {
            total_weight += e.weight;
            count++;
            if (build_tree) {
                mst_tree_kruskal[e.u].push_back(e.v);
                mst_tree_kruskal[e.v].push_back(e.u);
            }
            if (count == n - 1) break;
        }
    }
    return total_weight;
}

// Prim's algorithm that also builds MST tree
double prim(int n, const vector<Edge>& edges, bool build_tree = false) {
    vector<vector<pair<int, double>>> graph(n);
    for (auto& e : edges) {
        graph[e.u].emplace_back(e.v, e.weight);
        graph[e.v].emplace_back(e.u, e.weight);
    }

    vector<bool> in_mst(n, false);
    vector<double> min_edge(n, 1e9); 
    vector<int> parent(n, -1);
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;

    min_edge[0] = 0;
    pq.emplace(0.0, 0);
    double total_weight = 0.0;

    while (!pq.empty()) {
        auto [w, u] = pq.top(); pq.pop();
        if (in_mst[u]) continue;
        in_mst[u] = true;
        total_weight += w;

        for (auto [v, weight] : graph[u]) {
            if (!in_mst[v] && weight < min_edge[v]) {
                min_edge[v] = weight;
                parent[v] = u;
                pq.emplace(weight, v);
            }
        }
    }

    if (build_tree) {
        mst_tree_prim.assign(n, {});
        for (int v = 1; v < n; ++v) {
            int u = parent[v];
            if (u != -1) {
                mst_tree_prim[u].push_back(v);
                mst_tree_prim[v].push_back(u);
            }
        }
    }

    return total_weight;
}

// Broadcast DFS
int dfs(const vector<vector<int>>& tree, int u, int parent) {
    vector<int> child_times;
    for (int v : tree[u]) {
        if (v != parent) {
            child_times.push_back(dfs(tree, v, u));
        }
    }

    sort(child_times.rbegin(), child_times.rend());

    int max_time = 0;
    for (int i = 0; i < (int)child_times.size(); ++i) {
        max_time = max(max_time, child_times[i] + i + 1);
    }

    return max_time;
}

// Eksperyment
void run_experiment(int nMin, int nMax, int step, int rep) {
    ofstream out("mst_times_and_rounds.csv");
    out << "n,kruskal_time,prim_time,kruskal_average_rounds,prim_average_rounds,kruskal_min_rounds,prim_min_rounds,kruskal_max_rounds,prim_max_rounds\n";

    random_device rd;
    mt19937 gen(rd());
    

    for (int n = nMin; n <= nMax; n += step) {
        double total_kruskal_time = 0.0, total_prim_time = 0.0;
        double total_kruskal_rounds = 0.0, total_prim_rounds = 0.0;
        int min_kruskal_rounds = numeric_limits<int>::max();
        int max_kruskal_rounds = 0;
        int min_prim_rounds = numeric_limits<int>::max();
        int max_prim_rounds = 0;

        for (int r = 0; r < rep; ++r) {
            auto edges = generate_complete_graph(n);

            auto start_k = chrono::high_resolution_clock::now();
            kruskal(n, edges, true);
            auto end_k = chrono::high_resolution_clock::now();
            total_kruskal_time += chrono::duration<double, milli>(end_k - start_k).count();

            auto start_p = chrono::high_resolution_clock::now();
            prim(n, edges, true);
            auto end_p = chrono::high_resolution_clock::now();
            total_prim_time += chrono::duration<double, milli>(end_p - start_p).count();

            //int root = gen() % n;
            int root1 = gen() % n;
            int root2 = gen() % n;
            int kruskal_rounds=dfs(mst_tree_kruskal, root1, -1);
            int prim_rounds=dfs(mst_tree_prim, root2, -1);
            total_kruskal_rounds += kruskal_rounds;
            total_prim_rounds += prim_rounds;
            min_kruskal_rounds=min(min_kruskal_rounds,kruskal_rounds);
            min_prim_rounds=min(min_prim_rounds,prim_rounds);
            max_kruskal_rounds=max(max_kruskal_rounds,kruskal_rounds);
            max_prim_rounds=max(max_prim_rounds,prim_rounds);
        }

        out << n << ","
            << (total_kruskal_time / rep) << ","
            << (total_prim_time / rep) << ","
            << (total_kruskal_rounds / rep) << ","
            << (total_prim_rounds / rep) << ","
            << (min_kruskal_rounds ) << ","
            << (min_prim_rounds ) << ","
            << (max_kruskal_rounds ) << ","
            << (max_prim_rounds ) <<  "\n";

        cout << "n = " << n << " done.\n";
    }

    out.close();
}

int main() {
    int nMin = 1000, nMax = 5000, step = 500, rep = 15;
    run_experiment(nMin, nMax, step, rep);
    return 0;
}
