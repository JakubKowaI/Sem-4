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
        cout<<"\nInserting: "<<key<<endl;
        Node *y=nullptr;
        Node* x=root;
        while(x){
            y=x;
            if(key<x->key)
                x=x->left;
            else if(key>x->key)
                x=x->right;
            else
                return;
        }
        Node* z=new Node{key};
        z->p=y;
        if(!y){
            root=z;
        }else if(z->key<y->key){
            y->left=z;
        }else{
            y->right=z;
        }
        print();
    }

    Node* Min(Node* n){
        if(!n->left)return n;
        return Min(n->left);
    }

    Node* getSuccessor(Node* n){
        if(n->right){
            return Min(n->right);
        }
        Node* y=n->p;
        while(y&&n==y->right){
            n=y;
            y=y->p;
        }
        return y;
    }

    Node* search(int key) {
        Node* n=root;
        if(n->key==key)return n;
        while(n){
            if(n->key==key)return n;
            if(key>n->key)n=n->right;
                else n=n->left;
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

    if (x < n->key) {
        n->left = removeNode(n->left, x);
        if (n->left) n->left->p = n;
    } 
    else if (x > n->key) {
        n->right = removeNode(n->right, x);
        if (n->right) n->right->p = n;
    }
    else {
        if (!n->left) {
            Node* temp = n->right;
            // if (n == root) root = temp;
            // if (temp) temp->p = n->p;
            delete n;
            return temp;
        }
        else if (!n->right) {
            Node* temp = n->left;
            // if (n == root) root = temp;
            // if (temp) temp->p = n->p;
            delete n;
            return temp;
        }else{
            Node* successor = Min(n->right);
            n->key = successor->key;
            n->right = removeNode(n->right, successor->key);
            if (n->right) n->right->p = n;
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

// vector<int> Mix(vector<int> A){
//     vector<int> B = A;
//     shuffle(B.begin(), B.end(), mt);
    
//     return B;
// }

int main(int argc,char* argv[]){
string line;
    Tree BSTree;
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
            BSTree.insert(temp);
            auto end = chrono::high_resolution_clock::now();
            long long duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

            appendResultToCSV("BST", "insert",n, comparisons, pointer_reads,pointer_assignments, BSTree.height(), duration, "results.csv");
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
        long long duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

        appendResultToCSV("BST", "remove",n, comparisons, pointer_reads,pointer_assignments, BSTree.height(), duration, "results.csv");
    }
}