#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

int por=0;
int swp=0;

int* InsSort(int* A, int n){
    for(int i=1;i<n;i++){
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
        
            cout<<"Tablica w waznym momencie: "<<endl;
            for(int i =0;i<n;i++){
                cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<A[i]<<endl;
            }

    }
    return A;
}

int main(){
    string line;
    int n=-1;
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
    int* S = InsSort(A,n);

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