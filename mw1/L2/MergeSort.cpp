#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <vector>

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

void stan(int* A){
    cout<<"Tablica w waznym momencie: "<<endl;
    for(int i =0;i<n;i++){
        cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<A[i]<<endl;
    }
}

vector<int> Merge(vector<int> B,vector<int> C){
    if(B.empty())return C;
    if(C.empty())return B;
    if(B[0]<=C[0]){
        vector<int> T;
        T.push_back(B[0]);
        B.erase(B.begin());
        vector<int> temp = Merge(B,C);
        T.insert(T.end(),temp.begin(),temp.end());
        return T;
    }else{
        vector<int> T;
        T.push_back(C[0]);
        C.erase(C.begin());
        vector<int> temp = Merge(B,C);
        T.insert(T.end(),temp.begin(),temp.end());
        return T;
    }
}

vector<int> MergeSort(vector<int> A, int l, int p ){
    if(A.size()<=1)return A;
    
        vector<int> B = MergeSort(A,l,A.size()/2);
        vector<int> C = MergeSort(A,(A.size()/2)+1,p);
    
    return Merge(B,C);
}

int main(){
    string line;
    vector<int> A;
    try{
        getline(cin,line);
        n = stoi(line);
        //A = new int[n];
        int i=0;
        while(getline(cin, line)){
            //A[i]=stoi(line);
            //i++;
            A.push_back(stoi(line));
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
    vector<int> S=MergeSort(A,0,A.size());
    appendResultToCSV("MergeSort", n, por, swp, "results.csv");

    int temp=0;
    int status=0;
    if(n<40){

        cout<<"Tablica przed posortowaniem: "<<endl;
        for(int i =0;i<n;i++){
            cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<T[i]<<endl;
        }

        cout<<"Tablica po posortowaniu: "<<endl;
        
        for(int i =0;i<n;i++){
            if(i==0){
                temp=S[i];
            }else{
                if(temp>S[i]){
                    status=-1;
                }
                temp=S[i];
            }
            cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<S[i]<<endl;
        }
    }
    
    cout<<"Liczba porownan: "<<por<<" Liczba swapow: "<<swp<<endl;

    if(status==-1){
        cout<<"Tablica zle posortowana"<<endl;
    }else{
        cout<<"Wszytko git!"<<endl;
    }

    //delete A;
    

    return 0;
}