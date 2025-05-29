#include <iostream>
#include <random>
#include <string>
#include <chrono>

using namespace std;

int main(int argc,char *argv[]){
    mt19937 mt{
        static_cast<std::mt19937::result_type>(
            std::chrono::steady_clock::now().time_since_epoch().count()
            )    };
    if(argc==2){
        try{
            int n=stoi(argv[1]);
            for(int i =0;i<n;i++){
            cout<<mt()%(2*n-1)<<endl;
        }
        }catch(...){
            cout<<"end"<<endl;
        }
        
    }else{
        cout<<"zla liczbva arg"<<endl;
    }
    return 0;

}