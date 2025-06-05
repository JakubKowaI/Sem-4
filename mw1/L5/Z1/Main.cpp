#include <iostream>
#include <vector>
#include <tuple>
#include <queue>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>

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

double kruskal(int n, const vector<Edge>& edges) {
    vector<Edge> sorted_edges = edges;
    sort(sorted_edges.begin(), sorted_edges.end());
    DSU dsu(n);
    double total_weight = 0.0;
    int count = 0;

    for (const auto& e : sorted_edges) {
        if (dsu.unite(e.u, e.v)) {
            total_weight += e.weight;
            count++;
            if (count == n - 1) break;
        }
    }
    return total_weight;
}

double prim(int n, const vector<Edge>& edges) {
    vector<vector<pair<int, double>>> adj(n);
    for (const auto& e : edges) {
        adj[e.u].emplace_back(e.v, e.weight);
        adj[e.v].emplace_back(e.u, e.weight);
    }

    vector<bool> visited(n, false);
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;
    pq.emplace(0.0, 0);
    double total_weight = 0.0;

    while (!pq.empty()) {
        auto [w, u] = pq.top();
        pq.pop();
        if (visited[u]) continue;
        visited[u] = true;
        total_weight += w;

        for (auto [v, weight] : adj[u]) {
            if (!visited[v]) {
                pq.emplace(weight, v);
            }
        }
    }
    return total_weight;
}

void run_experiment(int nMin, int nMax, int step, int rep) {
    ofstream out("mst_times.csv");
    out << "n,kruskal_time,prim_time\n";

    for (int n = nMin; n <= nMax; n += step) {
        double total_kruskal_time = 0.0;
        double total_prim_time = 0.0;

        for (int r = 0; r < rep; ++r) {
            auto edges = generate_complete_graph(n);

            auto start_k = chrono::high_resolution_clock::now();
            kruskal(n, edges);
            auto end_k = chrono::high_resolution_clock::now();
            total_kruskal_time += chrono::duration<double, milli>(end_k - start_k).count();

            auto start_p = chrono::high_resolution_clock::now();
            prim(n, edges);
            auto end_p = chrono::high_resolution_clock::now();
            total_prim_time += chrono::duration<double, milli>(end_p - start_p).count();
        }

        out << n << "," << (total_kruskal_time / rep) << "," << (total_prim_time / rep) << "\n";
        cout << "n = " << n << " done.\n";
    }

    out.close();
}

int main() {
    int nMin = 100, nMax = 1000, step = 100, rep = 5;
    run_experiment(nMin, nMax, step, rep);
    return 0;
}
