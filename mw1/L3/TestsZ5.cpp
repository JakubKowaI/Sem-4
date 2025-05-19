#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <chrono>
#include <random>


using namespace std;

int main(){
    // mt19937 mt{
    //     static_cast<std::mt19937::result_type>(
    //     std::chrono::steady_clock::now().time_since_epoch().count()
    // )    };

    int k=25;
    for(int i =100;i<=10000;i+=100){
        for(int j=0;j<k;j++){
            //int temp = mt()%i;
            int k=10;
            string gen = "./Sorted.out "+to_string(i)+" "+to_string(k)+" > temp.txt";
            const char* gencom = gen.c_str();
            system(gencom);
            string rndSel = "./QuickSort.out < temp.txt";
            const char* rndcom = rndSel.c_str();
            system(rndcom);
            string sel = "./QuickSortSelect.out < temp.txt";
            const char* selcom = sel.c_str();
            system(selcom);
            string h = "./DualPivotQuickSort.out < temp.txt";
            const char* hcom = h.c_str();
            system(hcom);
            string h1 = "./DualPivotQuickSortSelect.out < temp.txt";
            const char* h1com = h1.c_str();
            system(h1com);
        }
    }
    return 0;
}