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
            cout<<argv[1]<<endl;
            for(int i =0;i<stoi(argv[1]);i++){
            cout<<mt()%(2*stoi(argv[1])-1)<<endl;
        }
        }catch(...){
            cout<<"end"<<endl;
        }
        
    }
    return 0;

}