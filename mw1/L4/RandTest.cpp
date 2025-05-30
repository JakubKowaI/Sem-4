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

    int k=20;
    for(int i =10000;i<=100000;i+=10000){
        for(int j=0;j<k;j++){
            int temp = mt()%i;
            string gen = "./RandGen "+to_string(i)+" > temp.txt";
            const char* gencom = gen.c_str();
            system(gencom);
            string rndSel = "./BST < temp.txt";
            const char* rndcom = rndSel.c_str();
            system(rndcom);
            string sel = "./RBT < temp.txt";
            const char* selcom = sel.c_str();
            system(selcom);
            string h = "./ST < temp.txt";
            const char* hcom = h.c_str();
            system(hcom);
        }
    }
    return 0;
}