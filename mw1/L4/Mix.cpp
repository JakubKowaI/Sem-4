#include <iostream>
#include <random>
#include <chrono>

using namespace std;

int main(){
    mt19937 mt{
        static_cast<std::mt19937::result_type>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        )    };

    string line;
    string A="";
    try{
        int i=0;
        while(getline(cin, line)){
            A.resize(i+1);
            A[i]=stoi(line);
            i++;
        }
    }catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return -1;
    }
    string B="";
    B.resize(A.size(),' ');
    for(int i=0;i<A.size();i++){
        int temp = mt()%A.size();
        while(B[temp]!=' '){
            temp = mt()%A.size();
        }
        B[temp]=A[i];
    }
    for(int i=0;i<B.size();i++){
        cout<<to_string(B[i])<<endl;
    }
}