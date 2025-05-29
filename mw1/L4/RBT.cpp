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
    bool color=false;//false = czerwony
};
//Declarations
void print_T(Node* root, int depth = 0, char prefix = ' ', string left_trace = "", string right_trace = "");

struct Tree{
    public:

    void rightRotate(Node* x){
        Node* y = x->left;
        x->left=y->right;
        if(y->right){
            y->right->p=x;
        }
        y->p=x->p;
        if(!x->p){
            root=y;
        }else if(x==x->p->left){
            x->p->left=y;
        }else{
            x->p->right=y;
        }
        y->right=x;
        x->p=y;
    }

    void leftRotate(Node* x){
        Node* y = x->right;
        x->right=y->left;//null
        if(y->left){
            y->left->p=x;
        }
        y->p=x->p;
        if(!x->p){
            root=y;
        }else if(x==x->p->left){
            x->p->left=y;
        }else{
            x->p->right=y;
        }
        y->left=x;
        x->p=y;
    }

    Node* insert(int key){
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
                return nullptr;
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

        return z;
    }

    void RBinsert(int key){
        Node* x=insert(key);
        Node* y;
        if(!x)return;

        while(x!=root&&!x->p->color){
            if (!x->p || !x->p->p) {
                break;
            }

            if(x->p==x->p->p->left){
                y=x->p->p->right;
                
                bool tempcolor=true;
                if(y)tempcolor=y->color;

                if(!tempcolor){
                    x->p->color=true;
                    if(y)y->color=true;
                    x->p->p->color=false;
                    x=x->p->p;
                }else{
                    if(x==x->p->right){
                        x=x->p;
                        leftRotate(x);
                    }
                    x->p->color=true;
                    x->p->p->color=false;
                    rightRotate(x->p->p);
                }
            }else{
                y=x->p->p->left;
                bool tempcolor=true;
                if(y)tempcolor=y->color;

                if(!tempcolor){
                    x->p->color=true;
                    if(y)y->color=true;
                    x->p->p->color=false;
                    x=x->p->p;
                }else{
                    if(x==x->p->left){
                        x=x->p;
                        rightRotate(x);
                    }
                    cout<<x->key<<endl;
                    x->p->color=true;
                    x->p->p->color=false;
                    leftRotate(x->p->p);
                }
            }
            root->color=true;
        }
        root->color=true;
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

    int height(Node* n) {
        if(n->right){
            if(n->right->color)return 1 + height(n->right);
            else return height(n->right);
        }else
        return 0;
    }

    

    void print(){
        print_T(root);
    }

    ~Tree() {
        free(root);
    }

    private:

    Node* root = nullptr;

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

void print_T(Node* root, int depth , char prefix , string left_trace , string right_trace ) {
    if (!root) return;

    left_trace.resize(depth + 1, ' ');
    right_trace.resize(depth + 1, ' ');

    if (root->left) {
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

    if (root->right) {
        left_trace[depth] = '|';
        print_T(root->right, depth + 1, '\\', left_trace, right_trace);
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
    Tree RBTtree;
    string A="";
    try{
        int i=0;
        while(getline(cin, line)){
            A.resize(i+1);
            int temp=stoi(line);
            A[i]=temp;
            RBTtree.RBinsert(temp);
            i++;
        }
    }catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return -1;
    }
    string B=Mix(A);
    for(int i=0;i<B.size();i++){
        int temp =(int)B[i];
        RBTtree.remove(temp);
    }
}