#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

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



int por=0;
int swp=0;
int n=0;
int prog=0;

void stan(int* A){
    cout<<"Tablica w waznym momencie: "<<endl;
    for(int i =0;i<n;i++){
        cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<A[i]<<endl;
    }
}

void InsSort(int* A, int p,int q){
    for(int i=p;i<=q;i++){
        int k = A[i];
        int temp=i-1;
        while (temp >= 0) {  
            por++;  
            if (A[temp] > k) {  
                A[temp + 1] = A[temp];
                temp--;
                swp++;  
            } else {
                break;  
            }
        }
            A[temp+1]=k;
            swp++;
            //stan(A);
    }
}

void swap(int* A,int p,int q){
    int temp = A[p];
        A[p]=A[q];
        A[q] = temp;
        swp++;
}

int partition(int* A,int p,int q){
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
    if(q-p<prog){
        InsSort(A,p,q);
    }else{
        int index = partition(A,p,q);
        //stan(A);
    QSort(A,p,index-1);
    QSort(A,index+1,q);
    }
    
}

int main(){
    string line;
    int* A = NULL;
    try{
        // ifstream data("temp.txt");
        // if(!data.is_open()){
        //     cout<<"blad"<<endl;
        //     return -1;
        // }
        
        getline(cin,line);
        //getline(data,line);
        n = stoi(line);
        A = new int[n];
        int i=0;
        while(getline(cin, line)){
            A[i]=stoi(line);
            i++;
            
        }
        //data.close();
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

    int* T = new int[n];
    for(int i =0;i<n;i++){
        T[i]=A[i];
    }

    for(int i = 10;i>=0;i--){
        prog=i;
        cout<<"Prog wynosi: "<<prog<<endl;

        for(int i =0;i<n;i++){
            A[i]=T[i];
        }
        QSort(A,0,n-1);
        cout<<"Liczba porownan: "<<por<<" Liczba swapow: "<<swp<<endl;
        por=0;
        swp=0;
    }

    QSort(A,0,n-1);
    
    //appendResultToCSV("HybridSort", n, por, swp, "results.csv");

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