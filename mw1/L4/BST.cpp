#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <random>
#include <vector>
#include <algorithm>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <thread>
#include <cstring>

using namespace std;

int comparisons = 0;
int pointer_reads = 0;
int pointer_assignments = 0;

//"Algorithm,Operation,n,Comparisons,Pointer reads,Pointer swaps,Height,Time (microsec)\n"

// vector<int> Algo;
vector<bool> Oper;
vector<int> N_num;
vector<int> comps;
vector<int> p_reads;
vector<int> p_swaps;
vector<int> High;
vector<int> Time;


mt19937 mt{
    static_cast<std::mt19937::result_type>(
        std::chrono::steady_clock::now().time_since_epoch().count()
    )    
};

void reset_counters() {
    comparisons = 0;
    pointer_reads = 0;
    pointer_assignments = 0;
}

struct Node{
    int key;
    Node *left=nullptr;
    Node * right=nullptr;
    Node * p=nullptr;
};

void save(bool operation, int n, int comparisons, int pointer_reads,int pointer_assignments,int height, int time_microsec){
    // Algo.push_back(algorithm);
    Oper.push_back(operation);
    N_num.push_back(n);
    comps.push_back(comparisons);
    p_reads.push_back(pointer_reads);
    p_swaps.push_back(pointer_assignments);
    High.push_back(height);
    Time.push_back(time_microsec);
}

void appendResultToCSV(const std::string& filename = "results.csv") {
    int fd = -1;

    // Próbuj otworzyć i zablokować plik dopóki się nie uda
    while (true) {
        fd = open(filename.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644);
        if (fd == -1) {
            cerr << "❌ Błąd przy otwieraniu pliku: " << strerror(errno) << std::endl;
            this_thread::sleep_for(std::chrono::milliseconds(100)); // poczekaj chwilę i próbuj dalej
            continue;
        }

        if (flock(fd, LOCK_EX) == -1) {
            cerr << "🔒 Plik zablokowany, oczekiwanie..." << std::endl;
            close(fd);
            this_thread::sleep_for(std::chrono::milliseconds(100)); // poczekaj 100 ms i próbuj ponownie
            continue;
        }

        break; // udało się otworzyć i zablokować plik
    }

    // Teraz mamy blokadę, możemy pisać
    std::ifstream checkFile(filename);
    checkFile.seekg(0, std::ios::end);
    bool isEmpty = (checkFile.tellg() == 0);
    checkFile.close();

    ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        cerr << "❌ Nie można otworzyć pliku do zapisu!" << std::endl;
        flock(fd, LOCK_UN);
        close(fd);
        return;
    }

    if (isEmpty) {
        file << "Algorithm,Operation,n,Comparisons,Pointer reads,Pointer swaps,Height,Time (microsec)\n";
    }

    for (int i = 0; i < comps.size(); ++i) {
        file << "BST,"
             << (Oper[i] ? "remove" : "insert") << ","
             << N_num[i] << ","
             << comps[i] << ","
             << p_reads[i] << ","
             << p_swaps[i] << ","
             << High[i] << ","
             << Time[i] << "\n";
    }

    file.close();
    flock(fd, LOCK_UN); // odblokuj plik
    close(fd);          // zamknij deskryptor
    reset_counters();
}

//Declarations
void print_BST(Node* root, int depth = 0, char prefix = ' ', string left_trace = "", string right_trace = "");

struct Tree{
    public:

    void insert(int key){
        cout<<"\nInserting: "<<key<<endl;
        Node *y = nullptr;
        Node* x = root;
        while (x) {
            pointer_reads++;          
            y = x;
            comparisons++;
            pointer_reads++;          
            if (key < x->key) {
                pointer_reads++;      
                x = x->left;
                pointer_assignments++;
            } else if (key > x->key) {
                pointer_reads++;      
                x = x->right;
                pointer_assignments++;
            } else {
                comparisons++;
                return;
            }
        }

        Node* z = new Node{key};
        pointer_assignments++;
        z->p = y;

        comparisons++;
        if (!y) {
            root = z;
            pointer_assignments++;
        } else {
            pointer_reads++;          
            comparisons++;
            if (z->key < y->key) {
                pointer_assignments++;
                y->left = z;
            } else {
                pointer_assignments++;
                y->right = z;
            }
        }
        pointer_assignments++;
        print();
    }

    Node* Min(Node* n){
        pointer_reads++;
        comparisons++;
        if(!n->left)return n;
        return Min(n->left);
    }

    Node* getSuccessor(Node* n){
        pointer_reads++;
        comparisons++;
        if (n->right) {
            pointer_reads++;
            return Min(n->right);
        }
        Node* y = n->p;
        pointer_assignments++;
        pointer_reads++;
        while (y && n == y->right) {
            comparisons++;
            pointer_reads++;
            n = y;
            pointer_assignments++;
            y = y->p;
            pointer_assignments++;
        }
        return y;
    }

    Node* search(int key) {
        Node* n = root;
        pointer_reads++;
        while (n) {
            pointer_reads++; 
            comparisons++;
            if (n->key == key) return n;
            comparisons++;
            if (key > n->key) {
                pointer_reads++;
                n = n->right;
            } else {
                pointer_reads++;
                n = n->left;
            }
            pointer_assignments++;
        }
        return nullptr;
    }

    bool remove(int key){
        cout<<"\nDelete: "<<key<<endl;    
        Node* node_to_delete = search(key);
        if (!node_to_delete) {
            cout << "Node " << key << " not found." << endl;
            return false;
        }
        root = removeNode(root, key);
        print();
        return true;
    }

    int height() const {
        return height(root);
    }

    void print(){
        print_BST(root);
    }

    ~Tree() {
        free(root);
    }

    private:

    Node* root = nullptr;
    int height(const Node* n) const {
        if (n == nullptr) {
            return -1; 
        }
        return 1 + max(height(n->left), height(n->right));
    }

    Node* removeNode(Node* n, int x) {
        if (n == nullptr) return nullptr;
        pointer_reads++;
        comparisons++;
        if (x < n->key) {
            pointer_reads++;
            n->left = removeNode(n->left, x);
            pointer_assignments++;
            if (n->left) {
                pointer_reads++;
                pointer_assignments++;
                n->left->p = n;
            }
        } else if (x > n->key) {
            pointer_reads++;
            n->right = removeNode(n->right, x);
            pointer_assignments++;
            if (n->right) {
                pointer_reads++;
                pointer_assignments++;
                n->right->p = n;
            }
        } else {
            comparisons++;
            if (!n->left) {
                pointer_reads++;
                Node* temp = n->right;
                delete n;
                return temp;
            } else if (!n->right) {
                pointer_reads++;
                Node* temp = n->left;
                delete n;
                return temp;
            } else {
                Node* successor = Min(n->right);
                pointer_reads++;
                pointer_assignments++;
                n->key = successor->key;
                n->right = removeNode(n->right, successor->key);
                pointer_assignments++;
                if (n->right) {
                    pointer_reads++;
                    pointer_assignments++;
                    n->right->p = n;
                }
            }
        }
        return n;
    }

    void free(Node* n){
        if(!n)return;
        free(n->left);
        free(n->right);
        delete n;
    }
};

void print_BST(Node* root, int depth , char prefix , string left_trace , string right_trace ) {
    if (!root) return;

    left_trace.resize(depth + 1, ' ');
    right_trace.resize(depth + 1, ' ');

    if (root->left) {
        right_trace[depth] = ' ';
        print_BST(root->left, depth + 1, '/', left_trace, right_trace);
    }

    if (prefix == '/')  left_trace[depth - 1] = '|';
    if (prefix == '\\'){
        right_trace[depth - 1] = ' ';
        left_trace[depth - 1] = ' ';
    } 

    if (depth == 0) cout << "-";
    if (depth >  0) cout << " ";

    for (int i = 0; i < depth - 1; i++) {
        if (left_trace[i] == '|'  || right_trace[i] == '|') {
            cout << "| ";
        }else {
            cout << "  ";
        }
    }

    if (depth > 0) cout << prefix << "-";
    cout << "[" << root->key << "]\n";

    if (root->right) {
        left_trace[depth] = '|';
        print_BST(root->right, depth + 1, '\\', left_trace, right_trace);
    }
}

int main(int argc,char* argv[]){
string line;
    Tree BSTree;
    vector<int> A;
    // if(argc!=2){
    //     cout<<"Zla liczba argumentow\n";
    //     return 0;
    // }
    // string n =argv[1];
    try{
        while(getline(cin, line)){
            int temp=stoi(line);
            A.push_back(temp);
            auto start = chrono::high_resolution_clock::now();
            BSTree.insert(temp);
            auto end = chrono::high_resolution_clock::now();
            long long duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

            //save(0,stoi(n),comparisons,pointer_reads,pointer_assignments,BSTree.height(),duration);
            //reset_counters();
        }
    }catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return -1;
    }

    shuffle(A.begin(),A.end(),mt);

    for(int i : A){
        auto start = chrono::high_resolution_clock::now();
        BSTree.remove(i);
        auto end = chrono::high_resolution_clock::now();
        long long duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        //save(1,stoi(n),comparisons,pointer_reads,pointer_assignments,BSTree.height(),duration);
        //reset_counters();
    }
    //appendResultToCSV();
}