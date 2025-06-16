#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>

using namespace std;

mt19937 mt{
    static_cast<std::mt19937::result_type>(
        std::chrono::steady_clock::now().time_since_epoch().count()
    )   
};
uniform_real_distribution<> dis(-1.0, 1.0);
uniform_real_distribution<> dis2(0.0, 0.5);

vector<double> L1(vector<double> x){
    double sum=0.0;
    vector<double> result;
    for(double el : x){
        sum+=el;
    }
    for(double el : x){
        result.push_back(el/sum);
    }
    return result;
}

vector<double> L2(vector<double> x){
    double sum = 0.0;
    for(double el : x){
        sum += el * el;
    }
    sum = sqrt(sum);
    vector<double> result;
    for(double el : x){
        result.push_back(el / sum);
    }
    return result;
}

vector<double> softmax(vector<double> x){
    double sum=0.0;
    vector<double> result;
    for(double el : x){
        sum+=exp(el);
    }
    for(double el : x){
        result.push_back(exp(el)/sum);
    }
    return result;
}

double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double sigmoid_pochodna(double x) {
    double s = sigmoid(x);
    return s * (1.0 - s);
}

double Relu(double x){
    return max(0.0,x);
}

double Relu_pochodna(double x) {
    return x > 0 ? 1.0 : 0.0;
}

struct wartswa_ukryta{
    vector<double> biasy;
    vector<vector<double>> wagi;
    double (*f)(double);
    double (*f_prim)(double);
    double wspolczynnik_uczenia;

    wartswa_ukryta(int n,double wspl_ucz,auto func,auto func_prim){
        wagi.assign(n,vector<double>(2,0.0));
        wspolczynnik_uczenia=wspl_ucz;
        f=func;
        f_prim=func_prim;
        for(int i=0;i<n;i++){
            for(int j=0;j<2;j++){
                wagi[i][j]=dis2(mt);
            }
        }
    }

    vector<double> forward(double x[2]){
        vector<double> result;
        for(int i=0;i<wagi.size();i++){
            result.push_back(wagi[i][0]*x[0]+wagi[i][1]*x[1]+biasy[i]);
            result[i]=f(result[i]);
        }
        return result;
    }
};

struct wartswa_wyjściowa{
    double bias;
    vector<double> wagi;
    double (*f)(double);
    double (*f_prim)(double);
    double wspolczynnik_uczenia;

    wartswa_wyjściowa(double wspl_ucz,auto func,auto func_prim){
        wagi.assign(4,dis2(mt));
        wspolczynnik_uczenia=wspl_ucz;
        f=func;
        f_prim=func_prim;
    }

    double forward(vector<double> x){
        return f(wagi[0]*x[0]+wagi[1]*x[1]+wagi[2]*x[2]+wagi[3]*x[3]+bias);
    }

    void back_propagation(double loss,double h){
        double delta_W = wspl_ucz*(loss*f_prim(h))
    }
};

double loss(double label,double predykcja){
    return label-predykcja;
}

int main(){
    int ilosc_testow=10;
    vector<pair<double,double>> dane;
    vector<bool> labels;
    for(int i=0;i<ilosc_testow;i++){
        dane.push_back(make_pair(dis(mt),dis(mt)));
        if(get<0>(dane[i])*get<1>(dane[i])>=0)labels.push_back(true);
        else labels.push_back(false);
        //cout<< get<0>(dane[i])<<" : "<<get<1>(dane[i])<<" | " <<labels[i]<<endl;
    }

    return 0;
}