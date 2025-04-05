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

long int por=0;
long int swp=0;
int n=0;

void stan(int* A){
    cout<<"Tablica w waznym momencie: "<<endl;
    for(int i =0;i<n;i++){
        cout<<setw(2)<<setfill('0')<<i<<" : "<<setw(2)<<setfill('0')<<A[i]<<endl;
    }
}

// vector<int> Merge(vector<int> B,vector<int> C){
//     if(B.empty()){
//         swp+=C.size();
//         return C;
//     }
//     if(C.empty()){
//         swp+=B.size();
//         return B;
//     }

//     vector<int> T;
//     if(B[0]<=C[0]){
//         T.push_back(B[0]);
//         B.erase(B.begin());
//     }else{
//         T.push_back(C[0]);
//         C.erase(C.begin());
//     }
//     por++;
//     swp++;
//     vector<int> temp = Merge(B,C);
//     T.insert(T.end(),temp.begin(),temp.end());
//     return T;
// }


// vector<int> MySort(vector<int> A){
//     if(A.size()<=1)return A;

//     vector<int>::iterator it= A.begin()+1;
//     vector<int>::iterator itemp = A.begin();
//     for(;it!=A.end();it++,itemp++){
//         por++;        
//         if(*itemp>*it)break;
//     }
//     vector<int> temp(A.begin(),it);
//     vector<int> B(it,A.end());
//     return Merge(temp,MySort(B));
// }

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

vector<int> MySort(vector<int>::iterator begin, vector<int>::iterator end) {
    if (end - begin <= 1) return vector<int>(begin, end);

    auto it = begin + 1;
    auto itemp = begin;

    for (; it != end; ++it, ++itemp) {
        por++;
        if (*itemp > *it) break;
    }

    vector<int> temp(begin, it);
    vector<int> B = MySort(it, end);
    return Merge(temp, B);
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
    vector<int> S=MySort(A.begin(),A.end());
    appendResultToCSV("MySort", n, por, swp, "results.csv");

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
    
    //cout<<"Liczba porownan: "<<por<<" Liczba swapow: "<<swp<<endl;

    if(status==-1){
        cout<<"Tablica zle posortowana"<<endl;
    }else{
        //cout<<"Wszytko git!"<<endl;
    }

    //delete A;
    

    return 0;
}