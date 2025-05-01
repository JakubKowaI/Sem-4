#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <chrono>
#include <random>


using namespace std;

int main(){
    mt19937 mt{
        static_cast<std::mt19937::result_type>(
        std::chrono::steady_clock::now().time_since_epoch().count()
    )    };

    int k=25;
    for(int i =100;i<=50000;i+=100){
        for(int j=0;j<k;j++){
            int temp = mt()%i;
            string gen = "./RandGen.out "+to_string(i)+" "+to_string(temp)+" | ./Select.out";
            const char* gencom = gen.c_str();
            system(gencom);
        }
    }
    return 0;
}