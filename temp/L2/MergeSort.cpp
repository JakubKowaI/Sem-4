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

vector<int> Merge(const vector<int>& B, const vector<int>& C) {
    vector<int> T;
    int i = 0, j = 0;

    while (i < B.size() && j < C.size()) {
        por++;
        swp++;
        if (B[i] <= C[j]) {
            T.push_back(B[i++]);
        } else {
            T.push_back(C[j++]);
        }
    }

    while (i < B.size()) {
        T.push_back(B[i++]);
        swp++;
    }
    while (j < C.size()) {
        T.push_back(C[j++]);
        swp++;
    }

    return T;
}
vector<int> MergeSort(vector<int> A){
    if(A.size()<=1)return A;

        int mid =A.size()/2;
        vector<int> B(A.begin(),A.begin()+mid);
        vector<int> C(A.begin()+mid,A.end()); 
        
    return Merge(MergeSort(B),MergeSort(C));
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
    vector<int> S=MergeSort(A);
    //appendResultToCSV("MergeSort", n, por, swp, "results.csv");

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
        cout<<"Poprawnie posortowane"<<endl;
    }

    //delete A;
    

    return 0;
}