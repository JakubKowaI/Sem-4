#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <bits/stdc++.h>
#include <chrono>
#include <random>
#include <array>



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

int partition(int* A,int p,int q,int i){
    int pivot= A[i];
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
    int index = partition(A,p,q,p);
    
    //stan(A);

    QSort(A,p,index-1);
    QSort(A,index+1,q);
}

int Select(int* A, int p, int q,int i){
    if(p==q)return A[p];

    int median[5];
    median[1]=-1;
    int* medians;
    int it=0;
    for(int j=0;j<=q;j++){
        median[j%5]=A[j];
        if(j%5==0&&median[1]!=-1){
            it+=5;
            QSort(median,0,4);
            medians[(j/5)-1]=median[2];
        }
    }
    int t[4];
    int tsize=q-it;
    for(;it<=q;it++;){
        t[(it%5)-1]=A[it];
    }
    QSort(t,0,tsize,0);
    medians[(it/5)+1]=t[]
    int m;
    int r = partition(A,p,q,m);
    int k = r-p+1;
    if(i==k)return A[r];
    if(i<k){
        return Select(A,p,r-1,i);
    }else{
        return Select(A,r+1,q,i-k);
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

    cout<<Select(A,0,n-1,2)<<" - wynik"<<endl;
    //appendResultToCSV("DualPivotQuickSort", n, por, swp, "results.csv");

    delete[] A;

    return 0;
}