#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <bits/stdc++.h>
#include <chrono>
#include <random>
#include <array>




// void appendResultToCSV(const std::string& algorithm, int array_size, int comparisons, int swaps,long long time,int size, const std::string& filename) {
//     std::ofstream file(filename, std::ios::app);
//     if (!file.is_open()) {
//         std::cerr << "Error opening file!" << std::endl;
//         return;
//     }

//     std::ifstream checkFile(filename);
//     if (checkFile.peek() == std::ifstream::traits_type::eof()) {
//         file << "Algorithm,Array Size,Comparisons,Swaps,Time (microsec),Group Size\n";
//     }
    
//     file << algorithm << "," << array_size << "," << comparisons << "," << swaps << "," << time<<","<<size << "\n";
//     file.flush();
//     file.close();
// }

void appendResultToCSV(const std::string& algorithm, int array_size, int comparisons, int swaps,long long time, const std::string& filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    std::ifstream checkFile(filename);
    if (checkFile.peek() == std::ifstream::traits_type::eof()) {
        file << "Algorithm,Array Size,Comparisons,Swaps,Time (microsec),Group Size\n";
    }
    
    file << algorithm << "," << array_size << "," << comparisons << "," << swaps << "," << time<<","<< "\n";
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

int partition(int* A,int p,int q,int index){
    int pivot = A[index];
    int counter=0;
    swap(A,p,index);
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
    QSort(A,p,index-1);
    QSort(A,index+1,q);
}

int Select(int* A, int p, int q,int i,int groupsize){
    if(p==q)return A[p];

    int median[5];
    int numGroups;
    
    numGroups = (q - p + 1 + (5-1)) / 5;
    // cout<<q<<" v "<<numGroups<<endl;
    int medians[numGroups];
    int it=0;
    for (int j = 0; j < numGroups; j++) {
        int start = p + j * 5;
        int end;
        if(start+5-1<=q){
            end=start+5-1;
        }else{
            end=q;
            // cout<<"Median "<<start<<" | "<<end<<" | "<<end-start<<endl;
        }
        for(int k=0;k<=end-start;k++){
            median[k]=A[start+k];
        }
        QSort(median, 0, end-start);
        medians[j] = median[(end - start) / 2];
    }
    int mm=Select(medians,0,numGroups-1,numGroups/2,groupsize);
    // cout<<"tets"<<mm<<endl;
    int pivotIndex=-1;
    for (int i = p; i <= q; i++) {
        if(n<=30)cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<A[i]<<endl;
        if (A[i] == mm) {
            pivotIndex = i;
            //break;
        }
    }    if(n<=30)cout<<endl;
    int r = partition(A,p,q,pivotIndex);
    // cout<< "R = "<<r<<endl;
    int k = r-p+1;
    if(i==k)return A[r];
    if(i<k){
        return Select(A,p,r-1,i,groupsize);
    }else{
        return Select(A,r+1,q,i-k,groupsize);
    }
}

int main(){
    string line;
    int* A = NULL;
    int k;
    int size=9;
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

    int* T = new int[n];
    for(int i =0;i<n;i++){
        T[i]=A[i];
    }
    auto start_time = chrono::steady_clock::now();
    int wynik=Select(A,0,n-1,k,size);
    auto end_time = chrono::steady_clock::now();
    auto time = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
    int finalpor=por;
    int finalswp=swp;
    
    if(n<=30){
        cout<<"Tablica przed select: "<<endl;
        for(int i =0;i<n;i++){
            cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<T[i]<<endl;
        }
        cout<<"Tablica po select: "<<endl;
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
    
    
    //appendResultToCSV("Select", n, finalpor, finalswp, time,size, "results.csv");
    appendResultToCSV("Select", n, finalpor, finalswp, time, "results.csv");


    delete[] A;
    delete[] T;
    return 0;
}