#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <random>

using namespace std;

struct Node{
    int key;
    Node *left=nullptr;
    Node * right=nullptr;
    Node * p=nullptr;
};
//Declarations
void print_BST(Node* root, int depth = 0, char prefix = ' ', string left_trace = "", string right_trace = "");

struct Tree{
    public:

    // bool insert(int key) {
    //     cout<<"\nInsert: "<<key<<endl;
    //     if (!root) {
    //         root = new Node{key};
    //         print();
    //         return true;
    //     }
        
    //     Node* current = root;
    //     while (true) {
    //         if (key < current->key) {
    //             if (!current->left) {
    //                 current->left = new Node{key,nullptr,nullptr,current};
    //                 break;
    //             }
    //             current = current->left;
    //         } 
    //         else if (key > current->key) {
    //             if (!current->right) {
    //                 current->right = new Node{key,nullptr,nullptr,current};
    //                 break;
    //             }
    //             current = current->right;
    //         } 
    //         else {
    //             return false;
    //         }
    //     }
        
    //     print();
    //     return true;
    // }

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
        if(!removeNode(root,key))return false;
        print();
        return true;
    }

    // bool remove(int key){
    //     cout<<"\nDelete: "<<key<<endl;   
    //     Node* temp = search(key); 
    //     if(!temp)return false;
    //     if(!removeNode(temp))return false;
    //     print();
    //     return true;
    // }

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
    if (n == nullptr) return n;

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
            if (n == root) root = temp;
            if (temp) temp->p = n->p;
            delete n;
            return temp;
        }
        else if (!n->right) {
            Node* temp = n->left;
            if (n == root) root = temp;
            if (temp) temp->p = n->p;
            delete n;
            return temp;
        }

        Node* successor = Min(n->right);
        n->key = successor->key;
        n->right = removeNode(n->right, successor->key);
        if (n->right) n->right->p = n;
    }
    return n;
}

    // Node* removeNode(Node* n){
    //     Node* y;
    //     Node* x;
    //     if(!n->left||!n->right){
    //         y=n;
    //     }else{
    //         y=getSuccessor(n);
    //     }
    //     if(y->left){
    //         x=y->left;  
    //     }else{
    //         x=y->right;
    //     }
    //     if(x){
    //         x->p=y->p;
    //     }
    //     if(!y->p){
    //         root=x;
    //     }else if(y=y->p->left){
    //         y->p->left=x;
    //     }else{
    //         y->p->right=x;
    //     }
    //     if(y!=n){
    //         n->key=y->key;
    //         n->p=y->p;
    //         n->right=y->right;
    //         n->left=y->left;
    //     }
    //     return y;
    // }

    void free(Node* n){
        if(!n)return;
        free(n->left);
        free(n->right);
        delete n;
    }
};

void appendResultToCSV(const std::string& algorithm, int array_size, int comparisons, int swaps,long long time, const std::string& filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    std::ifstream checkFile(filename);
    if (checkFile.peek() == std::ifstream::traits_type::eof()) {
        file << "Algorithm,Array Size,Comparisons,Swaps,Time (microsec)\n";
    }
    
    file << algorithm << "," << array_size << "," << comparisons << "," << swaps << "," << time<< "\n";
    file.flush();
    file.close();
}

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

string Mix(string A){
    mt19937 mt{
        static_cast<std::mt19937::result_type>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        )    };

    string B="";
    B.resize(A.size(),' ');
    for(int i=0;i<A.size();i++){
        int temp = mt()%A.size();
        while(B[temp]!=' '){
            temp = mt()%A.size();
        }
        B[temp]=A[i];
    }
    return B;
}
int main(){
string line;
    Tree BSTtree;
    string A="";
    try{
        int i=0;
        while(getline(cin, line)){
            A.resize(i+1);
            int temp=stoi(line);
            A[i]=temp;
            BSTtree.insert(temp);
            i++;
        }
    }catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return -1;
    }
    string B=Mix(A);
    for(int i=0;i<B.size();i++){
        int temp =(int)B[i];
        BSTtree.remove(temp);
    }
}