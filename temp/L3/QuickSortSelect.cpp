#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <chrono>

void appendResultToCSV(const std::string& algorithm, int array_size, int comparisons, int swaps,long long time, const std::string& filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    std::ifstream checkFile(filename);
    if (checkFile.peek() == std::ifstream::traits_type::eof()) {
        file << "Algorithm,Array Size,Comparisons,Swaps,Time (microsec) \n";
    }
    
    file << algorithm << "," << array_size << "," << comparisons << "," << swaps << "," << time<< "\n";
    file.flush();
    file.close();
}


using namespace std;

int por =0;
int swp=0;
int n=0;

void stan(int* A){
    cout<<"Tablica w waznym momencie: "<<endl;
    for(int i =0;i<n;i++){
        cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<A[i]<<endl;
    }
}

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
    
    //stan(A);

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

void QSortWithSelect(int* A, int p, int q, int groupsize){
    if(p >= q || p < 0){
        return;
    }

    int len = q - p + 1;
    int i = len / 2 + 1; // szukamy mediany

    int pivotValue = Select(A, p, q, i, groupsize);

    // znajdź indeks pivotValue
    int pivotIndex = -1;
    for(int j = p; j <= q; j++){
        if (A[j] == pivotValue) {
            pivotIndex = j;
            break;
        }
    }

    if (pivotIndex == -1) {
        std::cerr << "Pivot not found" << std::endl;
        return;
    }

    int index = partition(A, p, q, pivotIndex);

    QSortWithSelect(A, p, index - 1, groupsize);
    QSortWithSelect(A, index + 1, q, groupsize);
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
    if(n<40){

        cout<<"Tablica przed posortowaniem: "<<endl;
        for(int i =0;i<n;i++){
            cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<A[i]<<endl;
        }
    }

    int* T = new int[n];
    for(int i =0;i<n;i++){
        T[i]=A[i];
    }
    auto start_time = chrono::steady_clock::now();
    QSortWithSelect(A,0,n-1,5);
    auto end_time = chrono::steady_clock::now();
    auto time = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
    
    appendResultToCSV("QS_Select", n, por, swp, time, "results.csv");

    int temp=0;
    if(n<40){

        cout<<"Tablica przed posortowaniem: "<<endl;
        for(int i =0;i<n;i++){
            cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<T[i]<<endl;
        }

        cout<<"Tablica po posortowaniu: "<<endl;
        
        for(int i =0;i<n;i++){
            if(i==0){
                temp=A[i];
            }else{
                if(temp>A[i]){
                    temp=-1;
                }else{
                    temp=A[i];
                }
            }
            cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<A[i]<<endl;
        }
        cout<<"Liczba porownan: "<<por<<" Liczba swapow: "<<swp<<endl;

        if(temp==-1){
            cout<<"Tablica zle posortowana"<<endl;
        }else{
            cout<<"Poprawnie posortowane"<<endl;
        }
    }
    
    

    delete[] A;

    return 0;
}