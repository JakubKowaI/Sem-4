#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <random>

using namespace std;

const int V = 20;  // Liczba wierzchołków
const int MAX_EDGES = 29;  // Maksymalna liczba krawędzi (|E| < 30)

// Struktura grafu
struct Graph {
    vector<vector<int>> adj;  // Lista sąsiedztwa
    map<pair<int, int>, int> c;  // Przepustowość c(e) (bit/s)
    map<pair<int, int>, int> a;  // Przepływ a(e) (pakiety/s)
    int N[V][V] = {0};  // Macierz natężeń
};

// Generuje losowy graf spójny bez wierzchołków izolowanych
Graph generate_graph() {
    Graph g;
    g.adj.resize(V);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> rand_node(0, V-1);
    uniform_int_distribution<int> rand_capacity(1000, 10000);  // c(e) ∈ [1000, 10000] bit/s
    uniform_int_distribution<int> rand_traffic(1, 50);         // Losowe n(i,j) ∈ [1, 50]

    // Generuj macierz N (kilka losowych połączeń)
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i != j && rand() % 10 < 2) {  // 20% szans na połączenie
                g.N[i][j] = rand_traffic(gen);
            }
        }
    }

    // Generuj krawędzie (graf nieskierowany dla prostoty)
    int edges = 0;
    while (edges < MAX_EDGES) {
        int u = rand_node(gen);
        int v = rand_node(gen);
        if (u != v && find(g.adj[u].begin(), g.adj[u].end(), v) == g.adj[u].end()) {
            g.adj[u].push_back(v);
            g.adj[v].push_back(u);
            int capacity = rand_capacity(gen);
            g.c[{u, v}] = capacity;
            g.c[{v, u}] = capacity;
            edges++;
        }
    }

    return g;
}

// Oblicz przepływ a(e) na podstawie macierzy N (uproszczone: najkrótsze ścieżki)
void calculate_flow(Graph &g) {
    // Dla każdej pary (i,j) z N[i][j] > 0 znajdź ścieżkę i dodaj przepływ
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (g.N[i][j] > 0) {
                // Uproszczony routing: zakładamy, że pakiet idzie bezpośrednio (jeśli krawędź istnieje)
                if (find(g.adj[i].begin(), g.adj[i].end(), j) != g.adj[i].end()) {
                    g.a[{i, j}] += g.N[i][j];
                }
                // W rzeczywistości tutaj powinien być algorytm routingu (np. Dijkstra)
            }
        }
    }
}

// Wypisz graf i parametry
void print_graph(const Graph &g) {
    cout << "Topologia grafu (lista sąsiedztwa):\n";
    for (int i = 0; i < V; i++) {
        cout << "v" << i << ": ";
        for (int v : g.adj[i]) {
            cout << "v" << v << " ";
        }
        cout << "\n";
    }

    cout << "\nPrzepustowości c(e):\n";
    for (auto &[edge, cap] : g.c) {
        cout << "e(v" << edge.first << " -> v" << edge.second << "): " << cap << " bit/s\n";
    }

    cout << "\nPrzepływy a(e):\n";
    for (auto &[edge, flow] : g.a) {
        cout << "e(v" << edge.first << " -> v" << edge.second << "): " << flow << " pakiety/s (N = " << g.N[edge.first][edge.second] << ")\n";
    }

    // Sprawdź warunek c(e) > a(e) * m (m = średni rozmiar pakietu w bitach)
    const int m = 1000;  // Załóżmy, że średni pakiet ma 1000 bitów
    cout << "\nWarunek c(e) > a(e)*m:\n";
    for (auto &[edge, flow] : g.a) {
        bool ok = (g.c.at(edge) > flow * m);
        cout << "e(v" << edge.first << " -> v" << edge.second << "): " 
             << g.c.at(edge) << " > " << flow * m << "? " << (ok ? "TAK" : "NIE") << "\n";
    }
}

int main() {
    Graph g = generate_graph();
    calculate_flow(g);
    print_graph(g);
    return 0;
}