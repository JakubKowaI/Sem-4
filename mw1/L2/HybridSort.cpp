#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

int por=0;
int swp=0;
int n=0;
int prog=10;

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
            stan(A);
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
        stan(A);
    QSort(A,p,index-1);
    QSort(A,index+1,q);
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

    int* T = new int[n];
    for(int i =0;i<n;i++){
        T[i]=A[i];
    }
    QSort(A,0,n-1);
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
        cout<<"Wszytko git!"<<endl;
    }

    delete[] A;

    return 0;
}