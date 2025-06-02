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
        file << "RBT,"
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


struct Node{
    int key;
    Node *left=nullptr;
    Node * right=nullptr;
    Node * p=nullptr;
    bool color=false;//false = czerwony

    Node(int k) : key(k), left(nullptr), right(nullptr), p(nullptr), color(false) {}
    Node() : key(-1), left(nullptr), right(nullptr), p(nullptr), color(true) {}
};
//Declarations
void print_T(Node* root, int depth = 0, char prefix = ' ', string left_trace = "", string right_trace = "");

struct Tree{
public:
    static Node* NIL; // Declare as static
    Node* root; // Root is per-tree

    void rightRotate(Node* x){
        Node* y = x->left;
        pointer_reads++;
        pointer_assignments++;

        x->left=y->right;
        pointer_reads++;
        pointer_reads++;
        pointer_assignments++;

        if(y->right!=NIL){
            pointer_reads++;
            y->right->p=x;
            pointer_reads++;
            pointer_assignments++;
        }

        y->p=x->p;
        pointer_reads++;
        pointer_assignments++;

        if(x->p==NIL){
            pointer_reads++;
            root=y;
            pointer_assignments++;
        }else if(x==x->p->left){
            pointer_reads += 2;
            x->p->left=y;
            pointer_assignments++;
        }else{
            pointer_reads += 2;
            x->p->right=y;
            pointer_assignments++;
        }

        y->right=x;
        pointer_assignments++;

        x->p=y;
        pointer_assignments++;
    }

    void leftRotate(Node* x){
        Node* y = x->right;
        pointer_reads++;
        pointer_assignments++;

        x->right = y->left;
        pointer_reads++;
        pointer_reads++;
        pointer_assignments++;

        if (y->left != NIL) {
            pointer_reads++;
            y->left->p = x;
            pointer_reads++;
            pointer_assignments++;
        }

        y->p = x->p;
        pointer_reads++;
        pointer_assignments++;

        if (x->p == NIL) {
            pointer_reads++;
            root = y;
            pointer_assignments++;
        } else if (x == x->p->left) {
            pointer_reads += 2;
            x->p->left = y;
            pointer_assignments++;
        } else {
            pointer_reads += 2;
            x->p->right = y;
            pointer_assignments++;
        }

        y->left = x;
        pointer_assignments++;

        x->p = y;
        pointer_assignments++;
    }

    Node* insert(int key){
        //cout<<"\nInserting: "<<key<<endl;
        Node *y=NIL;
        Node* x=root;
        while (x != NIL) {
            pointer_reads++; 
            y = x;
            pointer_assignments++;

            comparisons++;
            pointer_reads++; 
            if (key < x->key) {
                x = x->left;
                pointer_reads++;
                pointer_assignments++;
            } else if (key > x->key) {
                comparisons++;
                pointer_reads++;
                x = x->right;
                pointer_reads++;
                pointer_assignments++;
            } else {
                comparisons++;
                return nullptr;
            }
        }

        Node* z=new Node(key);
        z->left = NIL;
        z->right = NIL;
        z->p=y;
        z->color=false;
        pointer_assignments+=3;
        if(y==NIL){
            root=z;
            comparisons++;
        }else if(z->key<y->key){
            pointer_reads+=2;
            y->left=z;
            comparisons+=2;
        }else{
            y->right=z;
            comparisons+=2;
        }
        pointer_assignments++;

        return z;
    }

    void RBinsert(int key){
        Node* z=insert(key);
        pointer_assignments++;
        Node* y;
        if(!z)return;

        while(z!=root&&!z->p->color){
            comparisons+=2;
            pointer_reads+=2;
            if(z->p==z->p->p->left){
                pointer_reads+=3;
                y=z->p->p->right;
                pointer_assignments++;
                pointer_reads++;
                if(!y->color){
                    z->p->color=true;
                    y->color=true;
                    z->p->p->color=false;
                    z=z->p->p;
                    pointer_assignments+=3;
                    pointer_reads+=4;
                }else{
                    if(z==z->p->right){
                        comparisons++;
                        z=z->p;
                        pointer_reads+=2;
                        pointer_assignments++;
                        leftRotate(z);
                    }
                    z->p->color=true;
                    z->p->p->color=false;
                    pointer_reads+=2;
                    pointer_assignments+=2;
                    rightRotate(z->p->p);
                }
            }else{
                pointer_reads+=3;
                y=z->p->p->left;
                pointer_assignments++;
                pointer_reads++;
                if(!y->color){
                    z->p->color=true;
                    y->color=true;
                    z->p->p->color=false;
                    z=z->p->p;
                    pointer_assignments+=3;
                    pointer_reads+=4;
                }else{
                    if(z==z->p->left){
                        comparisons++;
                        z=z->p;
                        pointer_reads+=2;
                        pointer_assignments++;
                        rightRotate(z);
                    }
                    z->p->color=true;
                    z->p->p->color=false;
                    pointer_reads+=2;
                    pointer_assignments+=2;
                    leftRotate(z->p->p);
                }
            }
        }
        root->color=true;
        //print();
    }

    Node* Min(Node* n){
        pointer_reads++;
        comparisons++;
        if(n->left==NIL)return n;
        return Min(n->left);
    }

    Node* getSuccessor(Node* n){
        pointer_reads++;
        comparisons++;
        if(n->right!=NIL){
            pointer_reads++;
            return Min(n->right);
        }
        Node* y=n->p;
        pointer_assignments++;
        pointer_reads++;
        while(y!=NIL&&n==y->right){
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
        if (n->key == key) {
            comparisons++;
            return n;
        }

        while (n != NIL) {
            pointer_reads++; 
            comparisons++;
            pointer_reads++; 
            if (n->key == key) {
                return n;
            }
            comparisons++;
            pointer_reads++; 
            if (key > n->key) {
                n = n->right;
                pointer_reads++;
                pointer_assignments++;
            } else {
                n = n->left;
                pointer_reads++;
                pointer_assignments++;
            }
        }
        return nullptr;
    }

    bool remove(int key){
        //cout<<"\nDelete: "<<key<<endl;  
        Node* forDeletion=search(key);
        if(!forDeletion){
            //cout<<"Nie ma elementu w drzewie"<<endl;
            return false;
        }
        removeNode(forDeletion);
        //print();
        return true;
    }

    int height() {
        return  blackHeight(root);
    }

    void print(){
        print_T(root);
    }

    Tree(){
        if (NIL == nullptr) {
            NIL = new Node();
            NIL->color = true;
            NIL->left = NIL;
            NIL->right = NIL;
            NIL->p = NIL;
        }
        root = NIL;
    }

    ~Tree() {
        free(root);
    }

    void replace(Node* u, Node* v) {
        if (u->p == NIL) {
            pointer_reads++;
            root = v;
            pointer_assignments++;
        } else if (u == u->p->left) {
            pointer_reads += 2;
            u->p->left = v;
            pointer_assignments++;
        } else {
            pointer_reads += 2;
            u->p->right = v;
            pointer_assignments++;
        }

        v->p = u->p;
        pointer_assignments++;
    }

private:

    int blackHeight(Node* n) {
        if (n == NIL) return 1;
        pointer_reads++;
        pointer_reads++;
        //cout<<"Node "<<n->color<<endl;
        if (n->color){
            //cout<<"black"<<endl;
            return 1 + blackHeight(n->left);
            
        }else{
            return blackHeight(n->left);
        } 
    }

    void removeNode(Node* z) {
        Node* y = z; 
        Node* x = NIL; 
        pointer_assignments++;
        if (z->left == NIL || z->right == NIL) {
            y = z;
        } else {
            y = Min(z->right);
        }
        comparisons+=2;
        pointer_reads+=2;
        pointer_assignments++;

        if (y->left != NIL) {
            x = y->left;
        } else {
            x = y->right;
        }
        comparisons++;
        pointer_reads+=2;
        pointer_assignments++;

        bool ogYcolor = y->color;
        pointer_reads++;

        replace(y, x);

        if (y != z) {
            z->key = y->key; 
            pointer_reads++;
            pointer_assignments++;
        }
        comparisons++;
        pointer_reads++;

        delete y;

        if (ogYcolor) {
            RBDelFix(x);
        }
        comparisons++;
    }

    void RBDelFix(Node* x){
        Node* w;
        while(x!=root&&x->color){
            comparisons++;
            pointer_reads+=2;
            if(x==x->p->left){
                w=x->p->right;
                pointer_reads+=2;
                pointer_assignments++;

                if(!w->color){
                    w->color=true;
                    x->p->color=false;
                    leftRotate(x->p);
                    w=x->p->right;
                    pointer_assignments+=3;
                    pointer_reads+=4;
                }
                comparisons++;
                
                if(w->left->color&&w->right->color){
                    w->color=false;
                    x=x->p;
                    pointer_assignments+=2;
                    pointer_reads+=3;
                }else{
                    if(w->right->color){
                        w->left->color=true;
                        w->color=false;
                        rightRotate(w);
                        w=x->p->right;
                        pointer_assignments+=3;
                        pointer_reads+=4;
                    }
                    comparisons++;
                    w->color=x->p->color;
                    x->p->color=true;
                    w->right->color=true;
                    leftRotate(x->p);
                    x=root;
                    pointer_assignments+=4;
                    pointer_reads+=3;
                }
                comparisons++;
            }else{
                w=x->p->left;
                pointer_reads+=2;
                pointer_assignments++;
                if(!w->color){
                    w->color=true;
                    x->p->color=false;
                    rightRotate(x->p);
                    w=x->p->left;
                    pointer_assignments+=3;
                    pointer_reads+=4;
                }
                comparisons++;

                if(w->right->color&&w->left->color){
                    w->color=false;
                    x=x->p;
                    pointer_assignments+=2;
                    pointer_reads+=3;
                }else{
                    if(w->left->color){
                        w->right->color=true;
                        w->color=false;
                        leftRotate(w);
                        w=x->p->left;
                        pointer_assignments+=3;
                        pointer_reads+=4;
                    }
                    comparisons++;

                    w->color=x->p->color;
                    x->p->color=true;
                    w->left->color=true;
                    rightRotate(x->p);
                    x=root;
                    pointer_assignments+=4;
                    pointer_reads+=3;
                }
                comparisons++;
            }
            comparisons++;
        }
    x->color=true;//Moze musi byc w while
    }

    void free(Node* n){
        if(n==NIL)return;
        free(n->left);
        free(n->right);
        delete n;
    }
};

Node* Tree::NIL = nullptr;

void print_T(Node* root, int depth , char prefix , string left_trace , string right_trace ) {
    if (root==Tree::NIL) return;

    left_trace.resize(depth + 1, ' ');
    right_trace.resize(depth + 1, ' ');

    if (root->left!=Tree::NIL) {
        right_trace[depth] = ' ';
        print_T(root->left, depth + 1, '/', left_trace, right_trace);
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
    if(root->color)cout << "[\033[30m" << root->key << "\033[0m]\n";
    if(!root->color)cout << "[\033[31m" << root->key << "\033[0m]\n";

    if (root->right!=Tree::NIL) {
        left_trace[depth] = '|';
        print_T(root->right, depth + 1, '\\', left_trace, right_trace);
    }
}

int main(int argc, char* argv[]){
string line;
    Tree RBTree;
    vector<int> A;
    if(argc!=2){
        cout<<"Zla liczba argumentow\n";
        return 0;
    }
    string n =argv[1];
    try{
        while(getline(cin, line)){
            int temp=stoi(line);
            A.push_back(temp);
            auto start = chrono::high_resolution_clock::now();
            RBTree.RBinsert(temp);
            auto end = chrono::high_resolution_clock::now();
            long long duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

            save(0,stoi(n),comparisons,pointer_reads,pointer_assignments,RBTree.height(),duration);
            reset_counters();
        }
    }catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return -1;
    }

    shuffle(A.begin(),A.end(),mt);

    for(int i : A){
        auto start = chrono::high_resolution_clock::now();
        RBTree.remove(i);
        auto end = chrono::high_resolution_clock::now();
        long long duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
                
        save(1,stoi(n),comparisons,pointer_reads,pointer_assignments,RBTree.height(),duration);
        reset_counters();
    }
   appendResultToCSV();
}