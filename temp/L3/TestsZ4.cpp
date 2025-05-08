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
    for(int i =1000;i<=100000;i+=1000){
        for(int j=0;j<k;j++){
            //int temp = mt()%i;
            int p=(mt()%i)/10;
            int s=p+(i/2);
            int k=p+(8*(i/10));
            string gen = "./IncGen.out "+to_string(i)+" "+to_string(p)+" > temp.txt";
            const char* gencom = gen.c_str();
            system(gencom);
            string rndSel = "./BinSearch.out Front < temp.txt";
            const char* rndcom = rndSel.c_str();
            system(rndcom);
            string gen2 = "./IncGen.out "+to_string(i)+" "+to_string(s)+" > temp.txt";
            const char* gen2com = gen2.c_str();
            system(gen2com);
            string rndSel2 = "./BinSearch.out Mid < temp.txt";
            const char* rnd2com = rndSel2.c_str();
            system(rnd2com);
            string gen3 = "./IncGen.out "+to_string(i)+" "+to_string(k)+" > temp.txt";
            const char* gen3com = gen3.c_str();
            system(gen3com);
            string rndSel3 = "./BinSearch.out Back < temp.txt";
            const char* rnd3com = rndSel3.c_str();
            system(rnd3com);
            // string gen4 = "./IncGen.out "+to_string(i)+" "+to_string(2*i+1)+" > temp.txt";
            // const char* gen4com = gen4.c_str();
            // system(gen4com);
            // string rndSel4 = "./BinSearch.out Out_of_Table < temp.txt";
            // const char* rnd4com = rndSel4.c_str();
            // system(rnd4com);
        }
    }
    return 0;
}