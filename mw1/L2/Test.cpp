#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <bits/stdc++.h>

using namespace std;

int main(){
    for(int i =8;i<=32;i++){
        string ins = "./InsSort.out < ./Tables/DG"+to_string(i)+".txt";
        const char* inscom = ins.c_str();
        system(inscom);
        string q = "./QSort.out < ./Tables/DG"+to_string(i)+".txt";
        const char* qcom = q.c_str();
        system(qcom);
        string h = "./HybridSort.out < ./Tables/DG"+to_string(i)+".txt";
        const char* hcom = h.c_str();
        system(hcom);
    }
    return 0;
}