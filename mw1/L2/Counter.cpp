#include <iostream>
#include <string>

using namespace std;

int main(){
    string line;
    int n=-1;
    try{
        getline(cin,line);
        n = stoi(line);
    }catch(...){
        cout<<"N broken"<<endl;
    }
    int counter =0;
    while(getline(cin, line)){
        counter++;
    }
    cout<<"N: "<<n<<" Counter: "<<counter<<endl;
    return 0;
}