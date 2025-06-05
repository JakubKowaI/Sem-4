#include <iostream>
#include <limits>
#include <random>
#include <chrono>
#include <fstream>

using namespace std;

int comparisons = 0;

mt19937 mt{
    static_cast<std::mt19937::result_type>(
        chrono::steady_clock::now().time_since_epoch().count()
    )
};

void resert_comp(){
    comparisons=0;
}

struct Node {
    int key;
    Node* parent;
    Node* child;
    Node* sibling;
    int degree;

    Node(int k) : key(k), parent(nullptr), child(nullptr), sibling(nullptr), degree(0) {}
    Node() : key(-1), parent(nullptr), child(nullptr), sibling(nullptr), degree(0) {}
};

struct Heap {
    static Node* NIL;
    Node* root;

    Heap() {
        if (NIL == nullptr) {
            NIL = new Node();
            NIL->parent = NIL;
            NIL->child = NIL;
            NIL->sibling = NIL;
        }
        root = NIL;
    }

    void insert(int key) {
        Node* x = new Node(key);
        x->parent = x->child = x->sibling = NIL;
        Heap temp;
        temp.root = x;
        root = Binomial_heap_union(*this, temp).root;
    }

    int extract_min() {
        if (root == NIL) return -1;

        Node* prev_min = nullptr;
        Node* min_node = root;
        Node* prev = nullptr;
        Node* curr = root;
        int min_val = curr->key;

        while (curr != NIL) {
            comparisons++;
            if (curr->key < min_val) {
                min_val = curr->key;
                min_node = curr;
                prev_min = prev;
            }
            prev = curr;
            curr = curr->sibling;
        }

        // Remove min_node from root list
        if (prev_min != nullptr) {
            prev_min->sibling = min_node->sibling;
        } else {
            root = min_node->sibling;
        }

        // Reverse child list of min_node
        Node* child = min_node->child;
        Node* rev = NIL;

        while (child != NIL) {
            Node* next = child->sibling;
            child->sibling = rev;
            child->parent = nullptr;
            rev = child;
            child = next;
        }

        Heap h2;
        h2.root = rev;
        root = Binomial_heap_union(*this, h2).root;

        int minKey = min_node->key;
        delete min_node;
        return minKey;
    }

    static Heap Binomial_heap_union(Heap H1, Heap H2) {
        Heap H;
        H.root = Binomial_heap_merge(H1.root, H2.root);
        if (H.root == NIL) return H;

        Node* prev_x = NIL;
        Node* x = H.root;
        Node* next_x = x->sibling;

        while (next_x != NIL) {
            if ((x->degree != next_x->degree) ||
                (next_x->sibling != NIL && next_x->sibling->degree == x->degree)) {
                prev_x = x;
                x = next_x;
            } else if (x->key <= next_x->key) {
                comparisons++;
                x->sibling = next_x->sibling;
                Binomial_heap_link(next_x, x);
            } else {
                if (prev_x == NIL) {
                    H.root = next_x;
                } else {
                    prev_x->sibling = next_x;
                }
                Binomial_heap_link(x, next_x);
                x = next_x;
            }
            next_x = x->sibling;
        }

        return H;
    }

private:
    static void Binomial_heap_link(Node* y, Node* z) {
        y->parent = z;
        y->sibling = z->child;
        z->child = y;
        z->degree++;
    }

    static Node* Binomial_heap_merge(Node* h1, Node* h2) {
        if (h1 == NIL) return h2;
        if (h2 == NIL) return h1;

        Node* head = nullptr;
        Node** tail = &head;

        while (h1 != NIL && h2 != NIL) {
            if (h1->degree <= h2->degree) {
                *tail = h1;
                h1 = h1->sibling;
            } else {
                *tail = h2;
                h2 = h2->sibling;
            }
            tail = &((*tail)->sibling);
        }
        *tail = (h1 != NIL ? h1 : h2);
        return head;
    }
};

// Definicja statycznego pola
Node* Heap::NIL = nullptr;
int j=0;
void save_to_csv(int n, const string filename="results_n.csv") {
    ofstream file(filename, ios::app); // dopisuje
    if (file.tellp() == 0) {
        file << "n, comparisons\n";
    }
    j++;
    file << n << ","
         << comparisons << "\n";
    file.close();
    comparisons=0;
}

int main() {
    int n = 10000;
    

    for(int k=100;k<=n;k+=100){
        Heap H1, H2;
        for (int i = 0; i < k; i++) {
        H1.insert(mt() % (25 * n));
        H2.insert(mt() % (25 * n));
        }

        Heap H = Heap::Binomial_heap_union(H1, H2);

        for (int i = 0; i < 2 * k; i++) {
            int min_val = H.extract_min();
        }
        save_to_csv(k);
    }
    

    return 0;
}