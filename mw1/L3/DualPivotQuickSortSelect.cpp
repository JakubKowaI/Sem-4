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

int qpartition(int* A,int p,int q,int index){
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

int* partition(int* A, int p, int q, int pindex, int qindex) {
    int ppivot = A[pindex];
    int qpivot = A[qindex];
    int pcounter = 0;
    int qcounter = 0;

    // Upewnij się, że pivot1 ≤ pivot2 — jeśli nie, zamień je miejscami
    if (ppivot > qpivot) {
        swap(A, pindex, qindex);
        std::swap(ppivot, qpivot); // zmieniamy też wartości pivotów
    }

    // Umieść pivoty na krańcach (na początku i na końcu zakresu)
    swap(A, p, pindex);  // ← zmodyfikowano: przeniesienie pivotu1 na początek
    if (qindex == p) qindex = pindex; // jeśli zamieniliśmy miejsce, zaktualizuj qindex
    swap(A, q, qindex);  // ← zmodyfikowano: przeniesienie pivotu2 na koniec

    // Zaktualizuj pivoty po przestawieniu
    ppivot = A[p];
    qpivot = A[q];

    for (int i = p + 1; i < q - qcounter; i++) {
        if (qcounter > pcounter) {
            if (A[i] > qpivot) {
                qcounter++;
                por++;
                swap(A, q - qcounter, i);
                i--;
            } else if (A[i] <= ppivot) {
                pcounter++;
                por += 2;
                swap(A, p + pcounter, i);
            } else {
                por += 2;
            }
        } else {
            if (A[i] <= ppivot) {
                pcounter++;
                por++;
                swap(A, p + pcounter, i);
            } else if (A[i] > qpivot) {
                qcounter++;
                por += 2;
                swap(A, q - qcounter, i);
                i--;
            } else {
                por += 2;
            }
        }
    }

    swap(A, p + pcounter, p); // ← pivot1 wraca na miejsce
    swap(A, q - qcounter, q); // ← pivot2 wraca na miejsce

    int* T = new int[2];
    T[0] = pcounter + p;
    T[1] = q - qcounter;
    return T;
}

void QSort(int* A, int p, int q){
    if(p>=q||p<0){
        return ;
    }
    int index = qpartition(A,p,q,p);
    
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
    int r = qpartition(A,p,q,pivotIndex);
    // cout<< "R = "<<r<<endl;
    int k = r-p+1;
    if(i==k)return A[r];
    if(i<k){
        return Select(A,p,r-1,i,groupsize);
    }else{
        return Select(A,r+1,q,i-k,groupsize);
    }
}

void DualPivotQSortWithSelect(int* A, int p, int q, int groupsize){
    if(p >= q) return;

    int len = q - p + 1;
    int i1 = len / 3 + 1;       // pierwszy pivot — 1/3
    int i2 = 2 * len / 3 + 1;   // drugi pivot — 2/3

    // znajdź wartości pivotów
    int pivot1 = Select(A, p, q, i1, groupsize);
    int pivot2 = Select(A, p, q, i2, groupsize);

    if (pivot1 > pivot2) std::swap(pivot1, pivot2);

    // znajdź indeksy pivotów
    int pivotIndex1 = -1, pivotIndex2 = -1;
    for (int i = p; i <= q; i++) {
        if (A[i] == pivot1 && pivotIndex1 == -1) pivotIndex1 = i;
        else if (A[i] == pivot2 && pivotIndex2 == -1) pivotIndex2 = i;
    }

    if (pivotIndex1 == -1 || pivotIndex2 == -1) {
        std::cerr << "Error: pivot(s) not found\n";
        return;
    }

    int* t = partition(A,p,q,i1,i2);


    // Rekurencyjne wywołania
    DualPivotQSortWithSelect(A, p, t[0] - 1, groupsize);
    DualPivotQSortWithSelect(A, t[0] + 1, t[1] - 1, groupsize);
    DualPivotQSortWithSelect(A, t[1] + 1, q, groupsize);
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
    DualPivotQSortWithSelect(A, 0, n - 1, 5);
    auto end_time = chrono::steady_clock::now();
    auto time = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
   
    appendResultToCSV("DPQS_Select", n, por, swp, time, "results.csv");
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