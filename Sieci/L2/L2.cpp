#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <climits>
#include <queue>
#include <algorithm>
#include <unordered_set>
#include <fstream>
#include <string>


using namespace std;

int packetSize = 13;
double p = 0.98;
mt19937 mt(time(nullptr));
int maxEdges =30;

// Function to save Pr() results to CSV with float modifier
void saveResultsToCSV(const std::string& filename, double p, double T_max, 
    double pr_result, float modifier = 0.0f) {
std::ofstream outfile(filename, std::ios::app); // Open in append mode

if (!outfile.is_open()) {
std::cerr << "Error opening file: " << filename << std::endl;
return;
}

// Write header if file is empty
if (outfile.tellp() == 0) {
outfile << "Modifier,p,T_max,pr_result\n";
}

// Write data with float modifier
outfile << modifier << "," << p << "," << T_max << "," << pr_result << "\n";

outfile.close();
}

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
        e.c=mt()%1000+3000;
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
        e.c=mt()%1000+3000;
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



vector<int> dijkstra(const G& g, int from, int to, const unordered_set<int>& forbidden_edges = {}) {
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

        for (int ei = 0; ei < g.E.size(); ei++) {
            if (forbidden_edges.count(ei)) continue;
            
            const edge& e = g.E[ei];
            int v = -1;
            if (e.ends[0] == u && e.c != 0) v = e.ends[1];
            else if (e.ends[1] == u && e.c != 0) v = e.ends[0];
            
            if (v != -1 && e.c != 0) {
                int new_dist = dist[u] + 1;
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    parent[v] = u;
                    pq.push({new_dist, v});
                }
            }
        }
    }

    vector<int> path;
    if (parent[to] == -1) return path;

    for (int v = to; v != from; v = parent[v]) {
        path.push_back(v);
    }
    path.push_back(from);
    reverse(path.begin(), path.end());
    return path;
}

vector<int> find_any_path(const G& g, int from, int to) {
    // First try regular Dijkstra
    vector<int> path = dijkstra(g, from, to);
    if (!path.empty()) return path;

    // If no path found, try to find any path by ignoring edge capacities
    unordered_set<int> visited;
    vector<int> parent(20, -1);
    queue<int> q;
    q.push(from);
    visited.insert(from);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        if (u == to) break;

        for (const edge& e : g.E) {
            int v = -1;
            if (e.ends[0] == u) v = e.ends[1];
            else if (e.ends[1] == u) v = e.ends[0];

            if (v != -1 && !visited.count(v)) {
                visited.insert(v);
                parent[v] = u;
                q.push(v);
            }
        }
    }

    vector<int> any_path;
    if (parent[to] == -1) return any_path;

    for (int v = to; v != from; v = parent[v]) {
        any_path.push_back(v);
    }
    any_path.push_back(from);
    reverse(any_path.begin(), any_path.end());
    return any_path;
}

bool Send(G& g, int from, int to, int a) {
    if (from == to) return true;

    // Try to find the least congested path (weighted Dijkstra)
    auto get_cost = [](const edge& e, int additional_a) {
        double utilization = (e.a + additional_a) * packetSize / (double)e.c;
        return 1.0 + utilization * 100;  // Penalize high utilization
    };

    vector<int> path;
    vector<double> dist(20, INT_MAX);
    vector<int> parent(20, -1);
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    dist[from] = 0;
    pq.push({0, from});

    while (!pq.empty()) {
        int u = pq.top().second;
        double current_dist = pq.top().first;
        pq.pop();

        if (u == to) break;
        if (current_dist > dist[u]) continue;

        for (const edge& e : g.E) {
            int v = -1;
            if (e.ends[0] == u) v = e.ends[1];
            else if (e.ends[1] == u) v = e.ends[0];

            if (v != -1) {
                // Skip if edge would be overloaded
                if ((e.a + a) * packetSize > e.c) continue;
                
                double new_dist = dist[u] + get_cost(e, a);
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    parent[v] = u;
                    pq.push({new_dist, v});
                }
            }
        }
    }

    // Reconstruct path
    if (parent[to] == -1) {
        //cerr << "No valid path from " << from << " to " << to << " (would overload edges)" << endl;
        return false;
    }

    for (int v = to; v != from; v = parent[v]) {
        path.push_back(v);
    }
    path.push_back(from);
    reverse(path.begin(), path.end());

    // Reserve capacity on the path
    for (size_t i = 0; i < path.size() - 1; i++) {
        int u = path[i], v = path[i + 1];
        for (edge& e : g.E) {
            if ((e.ends[0] == u && e.ends[1] == v) || (e.ends[0] == v && e.ends[1] == u)) {
                e.a += a;
                break;
            }
        }
    }

    return true;
}
bool exec(G* g,int N[20][20]){
    for(int i=0;i<20;i++){
        for(int j=0;j<20;j++){
            if(N[i][j]!=0){
                if(!Send(*g, i, j, N[i][j]))return false;
            }
        }
    }
    return true;
}

void zerwij(G* g){
    for(vector<edge>::iterator e=g->E.begin();e!=g->E.end();e++){
        if(mt()%100>p*100){
            g->E.erase(e);
            e--;
        }
    }
}

double Pr(int N[20][20],double p, double T_max,G g,G main){
    int powt=5000;
    int good=0;
    for(int i=0;i<powt;i++){
        g=main;
        zerwij(&g);
        if(exec(&g,N)){
            double wynik=T(g,N);
            //cout<<"T: "<<wynik<<" Tmax: "<<T_max<<endl;
            if(wynik<T_max){
                good++;
                
            }else{
                //cout<<"to naprawde sie stalo"<<endl;
                //cout<<"T: "<<wynik<<" Tmax: "<<T_max<<endl;
            }
        }else{
            continue;
        }
    }
    //cout<<good<<" : "<<powt<<endl;
    double wynik = (double)good/powt;
    // for (edge e : g.E) {
    //     cout << e.ends[0] << " <-> " << e.ends[1] 
    //          << " | c: " << e.c 
    //          << " | a: " << e.a
    //          << " | wykorzystanie: " << (100.0 * e.a * packetSize / e.c) << "%" << endl;
    // }
    return wynik;
}

// Modified Pr function that saves results
double Pr_with_save(int N[20][20], double p, double T_max, G g, G main, const std::string& filename = "pr_results.csv") {
    double final_result = 0;
    
    for(float i = 1.0f; i <= 2.0f; i += 0.1f) {
        // Create fresh copies for each iteration
        G current_g = main;
        G current_main = main;
        
        // Scale edge capacities
        for(auto& e : current_g.E) {
            e.c = static_cast<int>(e.c * i);
        }
        
        // Scale main graph capacities too
        for(auto& e : current_main.E) {
            e.c = static_cast<int>(e.c * i);
        }
        
        double result = Pr(N, p, T_max, current_g, current_main);
        saveResultsToCSV(filename, p, T_max, result, i);
        final_result = result; // Store last result to return
    }
    
    return final_result;
}

// Modified Pr function that saves results
double Pr_N_with_save(int N[20][20], double p, double T_max, G g, G main, const std::string& filename = "pr_results.csv") {
    double result=0;
    for(float i =1;i<=2;i+=0.1){
        int temp[20][20];
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 20; k++) {
                temp[j][k] = N[j][k] * i;  // Copy and multiply in one step
            }
        }
        result = Pr(temp, p, T_max, g, main);
        g=main;
        saveResultsToCSV(filename, p, T_max, result,i);
    }
    
    return result;
}

// Modified Pr function that saves results
double Pr_Edges_with_save(int N[20][20], double p, double T_max, G g, G main, const std::string& filename = "pr_results.csv") {
    double final_result = 0;
    
    for(int i = 1; i <= 10; i++) {
        // Create fresh copies for each iteration
        G current_g = main;
        int average = 0;

        for(auto& e : current_g.E) {
            average+=e.c;
        }
        average = average/current_g.E.size();
        
        for(int j=0;j<i;j++){
            edge e;
            e.ends[0]=mt()%20;
            int d = mt()%20;
            while(d==e.ends[0]){
                d=mt()%20;
            }
            e.ends[1]=d;
            for(auto& e : current_g.E) {
                average+=e.c;
            }
            average = average/current_g.E.size();
            //cout<<"Average" <<average<<endl;

            e.c=average;
            e.a=0;
            current_g.E.push_back(e);
        }
        
        
        double result = Pr(N, p, T_max, current_g, main);
        saveResultsToCSV(filename, p, T_max, result, i);
        final_result = result; // Store last result to return
    }
    
    return final_result;
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
    
    cout<<Pr_Edges_with_save(N,(double)0.98,(double)0.1,g,main)<<endl;
    
    return 0;
}