#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <bits/stdc++.h>
#include <chrono>
#include <random>


void appendResultToCSV(const std::string& algorithm, int array_size, int comparisons, int swaps,long long time, const std::string& filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    std::ifstream checkFile(filename);
    if (checkFile.peek() == std::ifstream::traits_type::eof()) {
        file << "Algorithm,Array Size,Comparisons,Swaps,Time (microsec)\n";
    }
    
    file << algorithm << "," << array_size << "," << comparisons << "," << swaps << "," << time << "\n";
    file.flush();
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

int qpartition(int* A,int p,int q){
    int pivot= A[p];
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

void QSort(int* A, int p, int q){
    if(p>=q||p<0){
        return ;
    }
    int index = qpartition(A,p,q);
    QSort(A,p,index-1);
    QSort(A,index+1,q);
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
    int k;
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
        cout << "Error: " << e.what()<<n << endl;
        return -1;
    }
    int* T = new int[n];
    for(int i =0;i<n;i++){
        T[i]=A[i];
    }
    auto start_time = chrono::steady_clock::now();
    int wynik=randomSelect(A,0,n-1,k);
    auto end_time = chrono::steady_clock::now();
    auto time = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
    int finalpor=por;
    int finalswp=swp;

    if(n<=30){
        cout<<"Tablica przed RandomSelect: "<<endl;
        for(int i =0;i<n;i++){
            cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<T[i]<<endl;
        }
        cout<<"Tablica po RandomSelect: "<<endl;
        for(int i =0;i<n;i++){
            cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<A[i]<<endl;
        }
        cout<<"Wynik: "<<wynik<<endl;
        QSort(T,0,n-1);
        cout<<"Posortowana Tablica: "<<endl;
        for(int i =0;i<n;i++){
            cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<T[i]<<endl;
        }
    }

    appendResultToCSV("RandomSelect", n, finalpor, finalswp, time, "results.csv");


    delete[] A;
    delete[] T;
    return 0;
}