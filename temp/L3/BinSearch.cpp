#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <chrono>

using namespace std;

int por=0;
int n;

void appendResultToCSV(const std::string& algorithm, int array_size, int comparisons, int swaps,long long time, const std::string& filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    std::ifstream checkFile(filename);
    if (checkFile.peek() == std::ifstream::traits_type::eof()) {
        file << "Algorithm,Array Size,Comparisons,Result,Time (nanoseconds)\n";
    }
    
    file << algorithm << "," << array_size << "," << comparisons << "," << swaps << "," << time<< "\n";
    file.flush();
    file.close();
}

bool BinSearch(int* A,int p,int q,int v){
    if(q<p)return 0;
    if(q==p && A[p]==v){
        return 1;
    }
    int mid = (p+q)/2;
    por++;
    if(A[mid]==v){
        return 1;
    }else if(A[mid]<v){
        return BinSearch(A,mid+1,q,v);
    }else{
        return BinSearch(A,p,mid-1,v);
    }
}

int main(int argc,char *argv[]){
    bool wynik=0;
    string line;
    int* A = NULL;
    int k;
    int size=9;
    string type = argv[1];
    try{
        getline(cin,line);
        n = stoi(line);
        getline(cin,line);
        k = stoi(line);
        A = new int[n];
        int i=0;
        while(getline(cin, line)){
            A[i]=stoi(line);
            i++;
        }
    }catch (const exception& e) {
        cout << "Error: " << e.what() <<n<< endl;
        return -1;
    }
    // cout<<"Tablica: "<<endl;
    //     for(int i =0;i<n;i++){
    //         cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<A[i]<<endl;
    //     }
    //cout<<"v: "<<k<<endl;

    auto start_time = chrono::steady_clock::now();
    wynik=BinSearch(A,0,n-1,A[k]);
    auto end_time = chrono::steady_clock::now();
    auto time = chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count();
   
    //cout<<wynik<<endl;

    appendResultToCSV(type, n, por, wynik, time, "results.csv");


    return wynik;
}