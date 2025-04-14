#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <bits/stdc++.h>

using namespace std;

int main(int argc,char *argv[]){
    
    for(int i =8;i<=32;i++){
            cout<<"n = "<<to_string(i)<<endl;
            // string gen = "./RandGen.out "+to_string(i)+" > temp.txt";
            // const char* gencom = gen.c_str();
            // system(gencom);
            // string ins = "./InsSort.out < temp.txt";
            // const char* inscom = ins.c_str();
            // system(inscom);
            //string q = "./MergeSort.out < temp.txt";
            cout<<"Decreasing Table"<<endl;
            string q1 = string("./")+argv[1]+" < Tables/DG"+to_string(i)+".txt";
            const char* q1com = q1.c_str();
            system(q1com);
            cout<<"Increasing Table"<<endl;
            string q2 = string("./")+argv[1]+" < Tables/IG"+to_string(i)+".txt";
            const char* q2com = q2.c_str();
            system(q2com);
            cout<<"Random Table"<<endl;
            string q3 = string("./")+argv[1]+" < Tables/RG"+to_string(i)+".txt";
            const char* q3com = q3.c_str();
            system(q3com);
            // string h = "./MySort.out < temp.txt";
            // const char* hcom = h.c_str();
            // system(hcom);
        
    }
    return 0;
}