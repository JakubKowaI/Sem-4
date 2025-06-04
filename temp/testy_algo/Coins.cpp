#include <iostream>
#include <vector>

using namespace std;

int main(){
    int nominaly[4] = {1,2,4,5};
    int kwota = 11;
    vector<int> dp(kwota + 1, 9999999);
    dp[0] = 0;

    for(int i = 1; i <= kwota; ++i){
        for(int j = 0; j < 4; ++j){
            if(i - nominaly[j] >= 0){
                dp[i] = min(dp[i], dp[i - nominaly[j]] + 1);
            }
        }
    }

    cout << dp[kwota] << endl;
    return 0;
}