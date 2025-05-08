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
            string gen = "./RandGen.out "+to_string(i)+" "+to_string(temp)+" > temp.txt";
            const char* gencom = gen.c_str();
            system(gencom);
            string rndSel = "./RandomSelect.out < temp.txt";
            const char* rndcom = rndSel.c_str();
            system(rndcom);
            string sel = "./Select.out < temp.txt";
            const char* selcom = sel.c_str();
            system(selcom);
            // string h = "./MySort.out < temp.txt";
            // const char* hcom = h.c_str();
            // system(hcom);
        }
    }
    return 0;
}