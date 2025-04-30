#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <bits/stdc++.h>
#include <chrono>
#include <random>


void appendResultToCSV(const std::string& algorithm, int array_size, int comparisons, int swaps, const std::string& filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    std::ifstream checkFile(filename);
    if (checkFile.peek() == std::ifstream::traits_type::eof()) {
        file << "Algorithm,Array Size,Comparisons,Swaps\n";
    }
    
    file << algorithm << "," << array_size << "," << comparisons << "," << swaps << "\n";
    file.close();
}


using namespace std;

int por =0;
int swp=0;
int n=0;

void swap(int* A,int p,int q){
    int temp = A[p];
        A[p]=A[q];
        A[q] = temp;
        swp++;
}

int partition(int* A,int p,int q){

    mt19937 mt{
        static_cast<std::mt19937::result_type>(
            std::chrono::steady_clock::now().time_since_epoch().count()
            )
    };

    int pivot= A[(mt()%(q-p))+p];
    int counter=0;
    for(int i=p+1;i<=q;i++){
        if(A[i]<=pivot){
            counter++;
            por++;
            swap(A,p+counter,i);
        }
    }
    swap(A,p+counter,p);
    
    return counter+p;
}

int randomSelect(int* A, int p, int q,int i){
    if(p==q)return A[p];
    int r = partition(A,p,q);
    int k = r-p+1;
    if(i==k)return A[r];
    if(i<k){
        return randomSelect(A,p,r-1,i);
    }else{
        return randomSelect(A,r+1,q,i-k);
    }
}

int main(){
    string line;
    int* A = NULL;
    try{
        getline(cin,line);
        n = stoi(line);
        A = new int[n];
        int i=0;
        while(getline(cin, line)){
            A[i]=stoi(line);
            i++;
        }
    }catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return -1;
    }
    if(n<40){

        cout<<"Tablica przed posortowaniem: "<<endl;
        for(int i =0;i<n;i++){
            cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<A[i]<<endl;
        }
    }

    cout<<randomSelect(A,0,n-1,2)<<" - wynik"<<endl;
    //appendResultToCSV("DualPivotQuickSort", n, por, swp, "results.csv");


    delete[] A;

    return 0;
}