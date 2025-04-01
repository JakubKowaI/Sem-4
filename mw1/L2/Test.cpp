#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <bits/stdc++.h>

using namespace std;

int main(){
    int k=100;
    for(int i =1000;i<=50000;i+=1000){
        for(int j=0;j<k;j++){
            string gen = "./RandGen.out "+to_string(i)+" > temp.txt";
            const char* gencom = gen.c_str();
            system(gencom);
            // string ins = "./InsSort.out < temp.txt";
            // const char* inscom = ins.c_str();
            // system(inscom);
            string q = "./QSort.out < temp.txt";
            const char* qcom = q.c_str();
            system(qcom);
            string h = "./DPQSort.out < temp.txt";
            const char* hcom = h.c_str();
            system(hcom);
        }
    }
    return 0;
}