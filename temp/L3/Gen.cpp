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
    if(argc==3){
        try{
            int n=stoi(argv[1]);
            cout<<n<<endl;
            int k=stoi(argv[2]);
            if(k>n||k<1)return -1;
            cout<<k<<endl;
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