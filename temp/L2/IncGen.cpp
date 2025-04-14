#include <iostream>
#include <random>
#include <string>
#include <chrono>
#include <bits/stdc++.h>

using namespace std;

int main(int argc,char *argv[]){
    mt19937 mt{
        static_cast<std::mt19937::result_type>(
            std::chrono::steady_clock::now().time_since_epoch().count()
            )    };

    int* A = NULL;

    if(argc==2){
        try {
            int n = stoi(argv[1]);
            cout<<n<<endl;
            if (n <= 0) {
                throw invalid_argument("Size must be positive.");
            }

            int* A = new int[n];

            for (int i = 0; i < n; i++) {
                A[i] = mt() % (2 * n - 1);
            }

            sort(A, A + n);

            for (int i = 0; i < n; i++) {
                cout << A[i] << endl;
            }

            delete[] A;

        } catch (exception e) {
            cout << "Error: " << e.what() << endl;
        }
        
    }
    return 0;

}