#include <iostream>
#include <string>

using namespace std;

struct packet{
int num;
packet* front;
packet* back;
packet(int input):num(input),front(nullptr),back(nullptr){}
};

void insert(packet* l,int n){
    if(l==nullptr)return;
        packet* temp = new packet(n);
        l->num++;
    if(l->front==nullptr){
        temp->front=temp;
        temp->back=temp;
        l->front=temp;
    }else{
        temp->front=l->front;
        temp->back=l->front->back;
        l->front->back->front=temp;
        l->front->back=temp;
        l->front=temp;
    }
}

packet* merge(packet* l1,packet* l2){
    packet* temp = l2->front;
    for(int i =0;i<l2->num;i++){
        //cout<<"hej"<<endl;
        insert(l1,temp->num);
        temp=temp->front;
    }
    temp = l1;
    l1=nullptr;
    l2=nullptr;
    return temp;
}

void view(packet* l){
    if(l==nullptr)return;
    cout<<"Size: "<<l->num<<endl;
    packet* it = l->front;
    for(int i=0;i<l->num;i++){
        cout<<it->num<<endl;
        it=it->front;
    }
}

void mergeTest(){
    packet* l1=new packet(0);
    insert(l1,15);
    insert(l1,47);
    insert(l1,69);
    insert(l1,99);
    view(l1);
    packet* l2=new packet(0);
    insert(l2,444);
    insert(l2,888);
    insert(l2,1998);
    insert(l2,14);
    insert(l2,33);
    view(l2);
    packet* l3 = merge(l1,l2);
    view(l3);
    view(l1);
}

int find(packet* l, int n){
    int counter=0;
    packet* temp = l->front;
    if(rand()%2){
        while(temp->num!=n&&counter<l->num){
            counter++;
            temp=temp->front;
        }
    }else{
        while(temp->num!=n&&counter<l->num){
            counter++;
            temp=temp->back;
        }
    }
    return counter;
}

void mainTestT(){
    int T[10000];
    for(int i=0;i<10000;i++){
        T[i]= rand() % 100001;
    }
    packet* L = new packet(0);
    for(int it:T){
        insert(L,it);
    }
    double avg=0;
    for(int i=0;i<1000;i++){
        int temp =find(L,T[rand()%10000]);
        //cout<<temp<<endl;
        avg+=temp;
    }
    avg=avg/1000.0;
    cout<<"AVG= "<<avg<<endl;
}

void mainTestI(){
    int T[10000];
    for(int i=0;i<10000;i++){
        T[i]= rand() % 100001;
    }
    packet* L = new packet(0);
    for(int it:T){
        insert(L,it);
    }
    double avg=0;
    for(int i=0;i<1000;i++){
        int temp =find(L,rand()%100001);
        //cout<<temp<<endl;
        avg+=temp;
    }
    avg=avg/1000.0;
    cout<<"AVG= "<<avg<<endl;
}

int main() {
    srand (time(NULL));

    //mergeTest();
    mainTestT();
    mainTestI();
    return 0;
}
