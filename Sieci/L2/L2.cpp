#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <climits>
#include <queue>
#include <algorithm>

using namespace std;

int packetSize = 10;
double p = 0.98;
mt19937 mt(time(nullptr));
int maxEdges =30;


struct node{
    int no;
};

struct edge{
    int ends[2];
    int c;
    int a;
};

struct G{
    vector<node> V; //nodes
    vector<edge> E; //edges
    bool fail=false;
};

bool c(edge e,int a){
    if((a+e.a)*packetSize>e.c)return 0;
    return 1;
}

G genG(){
    G temp;
    int edgeCount=0;
    for(int i=0;i<20;i++){
        node v;
        v.no = i;
        temp.V.push_back(v);
        edge e;
        edgeCount++;
        e.ends[0]=i;
        e.ends[1]=(i+1)%20;
        e.c=mt()%10001+10000;
        e.a=0;
        temp.E.push_back(e);
        
    }
    for(;edgeCount<30;edgeCount++){
        edge e;
        e.ends[0]=mt()%20;
        int d = mt()%20;
        while(d==e.ends[0]){
            d=mt()%20;
        }
        e.ends[1]=d;
        e.c=mt()%10001+10000;
        e.a=0;
        temp.E.push_back(e);
    }
    return temp;
}

double T(G g, int N[20][20]) {
    if(g.fail)return 0;
    int AllPakiety = 0;
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            if (N[i][j] != 0) {
                AllPakiety += N[i][j];
            }
        }
    }

    if (AllPakiety == 0) {
        return 0.0;  
    }

    double suma = 0;
    for (edge e : g.E) {
        double denominator = (e.c / (double)packetSize) - e.a;
        if (denominator <= 0.0) {
            suma += 0;
        } else {
            suma += e.a / denominator;
        }
    }
    return suma / (double)AllPakiety;
}



vector<int> dijkstra(const G& g, int from, int to) {
    vector<int> dist(20, INT_MAX);
    vector<int> parent(20, -1);
    dist[from] = 0;

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, from});

    while (!pq.empty()) {
        int u = pq.top().second;
        int current_dist = pq.top().first;
        pq.pop();

        if (u == to) break;
        if (current_dist > dist[u]) continue;

        // Przeszukaj wszystkie krawędzie (zarówno wychodzące, jak i przychodzące)
        for (const edge& e : g.E) {
            int v = -1;
            if (e.ends[0] == u&&e.c!=0) v = e.ends[1];
            else if (e.ends[1] == u&&e.c!=0) v = e.ends[0];
            
            if (v != -1&&e.c!=0) {
                int new_dist = dist[u] + 1;
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    parent[v] = u;
                    pq.push({new_dist, v});
                }
            }
        }
    }

    // Odtwórz ścieżkę
    vector<int> path;
    if (parent[to] == -1) return path;

    for (int v = to; v != from; v = parent[v]) {
        path.push_back(v);
    }
    path.push_back(from);
    reverse(path.begin(), path.end());
    return path;
}

bool Send(G& g, int from, int to, int a) {
    if (from == to) return true;

    vector<int> path = dijkstra(g, from, to);
    if (path.empty()) {
        cerr << "Brak ścieżki z " << from << " do " << to << endl;
        g.fail=true;
        return false;
    }

    // Najpierw sprawdź, czy wszystkie krawędzie mogą obsłużyć przepływ
    for (size_t i = 0; i < path.size() - 1; i++) {
        int u = path[i];
        int v = path[i + 1];

        for (edge& e : g.E) {
            if ((e.ends[0] == u && e.ends[1] == v) || (e.ends[0] == v && e.ends[1] == u)) {
                if (!c(e, a)) {
                    cerr << "Przekroczono przepustowość na krawędzi " << u << "->" << v << endl;
                    g.fail=true;
                    return false;
                }
            }
        }
    }

    // Dopiero potem zaktualizuj przepływy
    for (size_t i = 0; i < path.size() - 1; i++) {
        int u = path[i];
        int v = path[i + 1];

        for (edge& e : g.E) {
            if ((e.ends[0] == u && e.ends[1] == v) || (e.ends[0] == v && e.ends[1] == u)) {
                e.a += a;
                break;
            }
        }
    }

    return true;
}

void exec(G* g,int N[20][20]){
    for(int i=0;i<20;i++){
        for(int j=0;j<20;j++){
            if(N[i][j]!=0){
                if (!Send(*g, i, j, N[i][j])) {
                    cerr << "Nie udało się przesłać pakietów z " << i << " do " << j << endl;
                    g->fail=true;
                }
            }
        }
    }
}

void zerwij(G* g){
    for(edge e : g->E){
        if(mt()%100>p*100){
            e.c=0;
        }
    }
}

double Pr(int N[20][20],double p, double T_max,G g,G main){
    int powt=1000;
    int good=0;
    for(int i=0;i<powt;i++){
        g=main;
        zerwij(&g);
        exec(&g,N);
        if(T(g,N)>=T_max){
            //cout<<"Siec rozspojna"<<endl;
            continue;
        }else{
            good++;
        }
    }
    cout<<good<<" : "<<powt<<endl;
    double wynik = (double)good/powt;
    return wynik;
}



int main(){
    int N[20][20]={};

    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            if (i != j) {
                int rand_val = mt() % 50;  
                if (rand_val < 20) {      
                    N[i][j] = rand_val;
                }
            }
        }
    }

    G g = genG();
    G main=g;
    // zerwij(&g);
    // exec(&g,N);
    // for(int i =0;i<20;i++){
    //     if(i==0){
    //         for(int j =0;j<20;j++){
    //             cout<<setw(2)<<j<<":";
    //         }
    //         cout<<endl;
    //     }
    //     cout<<setw(2)<<i<<" ";
    //     for(int j =0;j<20;j++){

    //         cout<<setw(2)<<N[i][j]<<":";
    //     }
    //     cout<<endl;
    // }
    // for (edge e : g.E) {
    //     cout << e.ends[0] << " <-> " << e.ends[1] 
    //          << " | c: " << e.c 
    //          << " | a: " << e.a
    //          << " | wykorzystanie: " << (100.0 * e.a * packetSize / e.c) << "%" << endl;
    // }
    cout<<Pr(N,(double)0.98,(double)0.1,g,main)<<endl;
    return 0;
}