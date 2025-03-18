#include <iostream>
#include <string>

using namespace std;

struct packet{
string text;
packet* front;
packet(string ttext):text(ttext),front(nullptr){}
};

void testLIFO(){
    packet* dock = nullptr;
    for(int i = 0; i<50;i++){
            packet* temp = new packet("LIFO"+to_string(i));

            if(dock==nullptr){
                dock = temp;
                 }else{
            temp->front=dock;
            dock = temp;
        }
    }

    while(dock!=nullptr){
        cout<<dock->text<<endl;
        dock = dock->front;
    }
}

void testFIFO(){
    packet* dock = nullptr;
    for(int i = 0; i<50;i++){
            packet* temp = new packet("FIFO"+to_string(i));

            if(dock==nullptr){
                dock = temp;
            }else{
            packet* it = dock;
            while(it->front!=nullptr){
                it = it->front;
            }
            it->front = temp;
        }
    }

    while(dock!=nullptr){
        cout<<dock->text<<endl;
        dock = dock->front;
    }

}

int main() {
    testLIFO();
    testFIFO();
    return 0;
}
