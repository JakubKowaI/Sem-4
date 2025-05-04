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
        file << "Algorithm,Array Size,Comparisons,Swaps,Time (microsec)\n";
    }
    
    file << algorithm << "," << array_size << "," << comparisons << "," << swaps << "," << time<<","<< "\n";
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

int* partition(int* A,int p,int q){
    int ppivot= A[p];
    int qpivot= A[q];
    int pcounter=0;
    int qcounter=0;
    if(ppivot>qpivot)swap(A,p,q);
    ppivot= A[p];
    qpivot= A[q];
    for(int i=p+1;i<q-qcounter;i++){
        if(qcounter>pcounter){
            if(A[i]>qpivot){
                qcounter++;
                por++;
                swap(A,q-qcounter,i);

                i--;
            }else if(A[i]<=ppivot){
                pcounter++;
                por+=2;

                swap(A,p+pcounter,i);
            }else {
                por+=2;
            }
        }else{
            if(A[i]<=ppivot){
                pcounter++;
                por++;

                swap(A,p+pcounter,i);
            }else if(A[i]>qpivot){
                qcounter++;
                por+=2;

                swap(A,q-qcounter,i);
                i--;
            }else{
                por+=2;
            }
        }
        
    }
    swap(A,p+pcounter,p);
    swap(A,q-qcounter,q);
    //stan(A);

    int* T = new int[2];
    T[0]=pcounter+p;
    T[1]=q-qcounter;
    return T;
}

void QSort(int* A, int p, int q){
    if(p>=q||p<0){
        return ;
    }
    int* index = partition(A,p,q);
    
    //stan(A);

    QSort(A,p,index[0]-1);
    QSort(A,index[0]+1,index[1]-1);
    QSort(A,index[1]+1,q);
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
    QSort(A, 0, n - 1);
    auto end_time = chrono::steady_clock::now();
    auto time = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
   
    appendResultToCSV("DPQS", n, por, swp, time, "results.csv");
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
    }
    
    cout<<"Liczba porownan: "<<por<<" Liczba swapow: "<<swp<<endl;

    if(temp==-1){
        cout<<"Tablica zle posortowana"<<endl;
    }else{
        cout<<"Poprawnie posortowane"<<endl;
    }

    delete[] A;

    return 0;
}