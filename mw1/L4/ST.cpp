#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <random>
#include <vector>
#include <algorithm>

using namespace std;

int comparisons = 0;
int pointer_reads = 0;
int pointer_assignments = 0;

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

void appendResultToCSV(const std::string& algorithm, const std::string& operation, string n, int comparisons, int pointer_reads,int pointer_assignments,int height, long long time_microsec, const std::string& filename="results.csv") {
    std::ifstream checkFile(filename);
    bool isEmpty = checkFile.peek() == std::ifstream::traits_type::eof();
    checkFile.close();

    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    if (isEmpty) {
        file << "Algorithm,Operation,n,Comparisons,Pointer reads,Pointer swaps,Height,Time (microsec)\n";
    }

    file << algorithm << "," << operation << "," << n << "," << comparisons << "," << pointer_reads<<","<<pointer_assignments<<","<<height << "," << time_microsec << "\n";
    file.close();
    reset_counters();
}

//Declarations
void print_BST(Node* root, int depth = 0, char prefix = ' ', string left_trace = "", string right_trace = "");

struct Tree{
    public:

    void insert(int key){
        //cout<<"\nInserting: "<<key<<endl;
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
        splay(z);
        //print();
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
        Node* last = nullptr;
        pointer_assignments+=2;
        while(n){
            comparisons++;
            last = n;
            pointer_assignments++;
            pointer_reads++;
            if(key < n->key){
                comparisons++;
                n = n->left;
                pointer_reads+=2;
                pointer_assignments++;
            }else if(key > n->key){
                comparisons++;
                n = n->right;
                pointer_reads+=2;
                pointer_assignments++;
            }else { 
                comparisons+=2;
                splay(n);
                return n;
            }
            
        }
        
        if (last) { 
            pointer_reads++;
            splay(last);
        }
        comparisons++;
        return nullptr; 
    }

    bool remove(int key) {
        //cout << "\nDelete: " << key << endl;

        Node* n = search(key); 
        pointer_assignments++;

        if (!n || n->key != key) { 
            cout << "Node " << key << " not found." << endl;
            return false;
        }
        comparisons+=2;
        pointer_reads+=2;

        Node* left = n->left;
        Node* right = n->right;
        pointer_reads+=2;
        pointer_reads+=2;

        delete n; 

        if (left == nullptr && right == nullptr) { 
            comparisons++;
            root = nullptr;
            pointer_reads+=2;
            pointer_assignments++;
        } else if (left == nullptr) { 
            comparisons+=2;
            root = right;
            root->p = nullptr; 
            pointer_reads+=3;
            pointer_assignments+=2;
        } else if (right == nullptr) { 
            comparisons+=3;
            root = left;
            root->p = nullptr; 
            pointer_reads+=3;
            pointer_assignments+=2;
        } else { 
            comparisons+=3;
            pointer_reads+=2;
            root = right; 
            root->p = nullptr; 
            pointer_assignments+=2;

            Node* minR = root;
            pointer_assignments++;
            while (minR->left) {
                comparisons++;
                minR = minR->left;
                pointer_assignments++;
                pointer_reads++;
            }
            splay(minR); 

            root->left = left;
            pointer_assignments++;
            pointer_reads++;
            if (left) {
                left->p = root;
                pointer_assignments++;
            }
            comparisons++;
        }

        //print();
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

    void rightRotate(Node* x){
        Node* y = x->left;
        pointer_reads++;
        pointer_assignments++;

        x->left=y->right;
        pointer_reads++;
        pointer_reads++;
        pointer_assignments++;

        if(y->right){
            pointer_reads++;
            y->right->p=x;
            pointer_reads++;
            pointer_assignments++;
        }

        y->p=x->p;
        pointer_reads++;
        pointer_assignments++;

        if(!x->p){
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

        if (y->left) {
            pointer_reads++;
            y->left->p = x;
            pointer_reads++;
            pointer_assignments++;
        }

        y->p = x->p;
        pointer_reads++;
        pointer_assignments++;

        if (!x->p) {
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


    void splay(Node* x){
        while(x!=root){
            comparisons++;
            pointer_reads++;
            if(x==x->p->left&&x->p==root){
                comparisons+=2;
                rightRotate(x->p);
            }else if(x==x->p->right&&x->p==root){
                comparisons+=3;
                leftRotate(x->p);
            }else if(x==x->p->left&&x->p==x->p->p->left){
                comparisons+=4;
                Node* y=x->p;
                Node* z=x->p->p;
                pointer_reads+=2;
                pointer_assignments+=2;
                rightRotate(z);
                rightRotate(y);
            }else if(x==x->p->right&&x->p==x->p->p->left){
                comparisons+=4;
                Node* y=x->p;
                Node* z=x->p->p;
                pointer_reads+=2;
                pointer_assignments+=2;
                leftRotate(y);
                rightRotate(z);
            }else if(x==x->p->left&&x->p==x->p->p->right){
                comparisons+=5;
                Node* y=x->p;
                Node* z=x->p->p;
                pointer_reads+=2;
                pointer_assignments+=2;
                rightRotate(y);
                leftRotate(z);
            }else if(x==x->p->right&&x->p==x->p->p->right){
                comparisons+=5;
                Node* y=x->p;
                Node* z=x->p->p;
                pointer_reads+=2;
                pointer_assignments+=2;
                leftRotate(z);
                leftRotate(y);
            }
        }
    }
  
    int height(const Node* n) const {
        comparisons++;
        pointer_reads++;
        if (n == nullptr) {
            return -1; 
        }
        return 1 + max(height(n->left), height(n->right));
    }

    Node* removeNode(Node* n, int x) {
        comparisons++;
        if (n == nullptr) return nullptr;

        if (x < n->key) {
            comparisons++;
            n->left = removeNode(n->left, x);
            pointer_reads+=2;
            pointer_assignments++;
            if (n->left){
                n->left->p = n;
                pointer_assignments++;
                pointer_reads++;
            } 
            comparisons++;
        }else if (x > n->key) {
            comparisons+=2;
            n->right = removeNode(n->right, x);
            pointer_reads+=2;
            pointer_assignments++;
            if (n->right){
                n->right->p = n;
            pointer_assignments++;
                pointer_reads++;
            } 
            comparisons++; 
        }else {
            comparisons+=2;
            if (!n->left) {
                comparisons++;
                Node* temp = n->right;
                pointer_assignments++;
                pointer_reads++;
                // if (n == root) root = temp;
                // if (temp) temp->p = n->p;
                splay(n->p);
                delete n;
                return temp;
            }else if (!n->right) {
                comparisons+=2;
                Node* temp = n->left;
                pointer_assignments++;
                pointer_reads++;
                // if (n == root) root = temp;
                // if (temp) temp->p = n->p;
                splay(n->p);
                delete n;
                return temp;
            }else{
                comparisons+=2;
                Node* successor = Min(n->right);
                n->key = successor->key;
                n->right = removeNode(n->right, successor->key);
                pointer_assignments+=3;
                pointer_reads+=2;
                if (n->right){
                    n->right->p = n;
                    pointer_assignments++;
                    pointer_reads++;
                } 
                comparisons++;
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
    Tree STree;
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
            STree.insert(temp);
            auto end = chrono::high_resolution_clock::now();
            long long duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

            appendResultToCSV("ST", "insert",n, comparisons, pointer_reads,pointer_assignments, STree.height(), duration, "results.csv");
        }
    }catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return -1;
    }

    shuffle(A.begin(),A.end(),mt);

    for(int i : A){
        auto start = chrono::high_resolution_clock::now();
        STree.remove(i);
        auto end = chrono::high_resolution_clock::now();
        long long duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

        appendResultToCSV("ST", "remove",n, comparisons, pointer_reads,pointer_assignments, STree.height(), duration, "results.csv");
    }
}