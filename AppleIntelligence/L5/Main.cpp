#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <fstream>

using namespace std;

mt19937 mt{
    static_cast<std::mt19937::result_type>(
        std::chrono::steady_clock::now().time_since_epoch().count()
    )   
};
uniform_real_distribution<> dis(-1.0, 1.0);
uniform_real_distribution<> dis2(0.0, 0.5);

void save_predictions_to_csv(const string normalize, const string hidden_f, const string output_f, const double learning_rate, const double accuracy, const string& filename="results.csv"){
    ofstream out(filename,ios::app);
    if(!out){
        out << "Normalize_func,Hidden_function,Output_function,Learning_rate,Accuracy\n";
    }
    
        out << normalize << "," << hidden_f << ","
            << output_f << "," << learning_rate << "," << accuracy << "\n";
    

    out.close();
    //cout << "Saved predictions to " << filename << "\n";
    //cout << "Model accuracy: " << (double(correct) / dane.size()) * 100.0 << "%\n";
}


vector<pair<double,double>> L1(vector<pair<double,double>> x){
    double sum1=0.0;
    double sum2=0.0;
    vector<pair<double,double>> result;
    for(pair<double,double> el : x){
        sum1+=el.first;
        sum2+=el.second;
    }
    for(pair<double,double> el : x){
        result.push_back(make_pair(el.first/sum1,el.second/sum2));
    }
    return result;
}

vector<pair<double,double>> L2(vector<pair<double,double>> x){
    vector<pair<double,double>> result;
    for (auto el : x) {
        double norm = sqrt(el.first * el.first + el.second * el.second);
        if (norm == 0) norm = 1e-8;  // prevent division by zero
        result.push_back({ el.first / norm, el.second / norm });
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
    vector<double> last_z, last_output;

    wartswa_ukryta(int n,double wspl_ucz,auto func,auto func_prim){
        wagi.assign(n,vector<double>(2,0.0));
        for (int i = 0; i < n; ++i) biasy.push_back(dis2(mt));
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
        last_z.clear(); last_output.clear();
        for(int i=0;i<wagi.size();i++){
            double z = wagi[i][0]*x[0]+wagi[i][1]*x[1];
            last_z.push_back(z);
            last_output.push_back(f(z+biasy[i]));
        }
        return last_output;
    }

    void back_propagation(double x[2], vector<double> dziwne_o) {
        for (int i = 0; i < wagi.size(); ++i) {
        double dz = f_prim(last_z[i]); // ReLU'(z)
        double delta = dziwne_o[i] * dz;  // dL/dz = dL/da * da/dz

        for (int j = 0; j < 2; ++j) {
            wagi[i][j] += wspolczynnik_uczenia * delta * x[j];
        }
        biasy[i] += wspolczynnik_uczenia * delta;
    }
    }

};

struct wartswa_wyjściowa{
    double bias;
    vector<double> wagi;
    double (*f)(double);
    double (*f_prim)(double);
    double wspolczynnik_uczenia;
    vector<double> last_input;
    double last_z;

    wartswa_wyjściowa(double wspl_ucz,auto func,auto func_prim){
        wagi.assign(4,dis2(mt));
        bias=dis2(mt);
        wspolczynnik_uczenia=wspl_ucz;
        f=func;
        f_prim=func_prim;
    }

    double forward(vector<double> x){
        last_input = x;
        last_z = wagi[0]*x[0]+wagi[1]*x[1]+wagi[2]*x[2]+wagi[3]*x[3];
        return f(last_z+bias);   
    }

    vector<double> back_propagation(double label, double prediction) {
        double pred_loss = label - prediction;
        double g_prim = f_prim(last_z + bias);
        double delta = pred_loss * g_prim;
        vector<double> dziwne_o;
        for (int i = 0; i < wagi.size(); ++i) {
            wagi[i] += wspolczynnik_uczenia * delta * last_input[i];
            dziwne_o.push_back(delta * wagi[i]);
        }
        bias += wspolczynnik_uczenia * delta;

        return dziwne_o;
    }
};

double loss(double label,double predykcja){
    return label-predykcja;
}

int main(){
    int ilosc_testow=100;
    vector<pair<double,double>> dane;
    vector<bool> labels;
    double learning_rate=0.1;

    while(learning_rate<=0.5){

        for(int repetition=0;repetition<15;repetition++){
            for(int i=0;i<ilosc_testow;i++){
                dane.push_back(make_pair(dis(mt),dis(mt)));
                if(get<0>(dane[i])*get<1>(dane[i])>=0)labels.push_back(true);
                else labels.push_back(false);
                //cout<< get<0>(dane[i])<<" : "<<get<1>(dane[i])<<" | " <<labels[i]<<endl;
            }
            
            //dane=L1(dane);

            wartswa_ukryta hidden(4, 0.1, Relu, Relu_pochodna);
            wartswa_wyjściowa output(0.1, sigmoid, sigmoid_pochodna);
            // wartswa_ukryta hidden(4, learning_rate, Relu, Relu_pochodna);
            // wartswa_wyjściowa output(learning_rate, Relu, Relu_pochodna);

            for (int epoch = 0; epoch < 5000; ++epoch) {
                for (int i = 0; i < ilosc_testow; ++i) {
                    double input[2] = { dane[i].first, dane[i].second };
                    double target = labels[i] ? 1.0 : 0.0;

                    // Forward pass
                    vector<double> hidden_out = hidden.forward(input);
                    double y_pred = output.forward(hidden_out);

                    // Backward pass
                    vector<double> grad_from_output = output.back_propagation(target, y_pred);
                    hidden.back_propagation(input, grad_from_output);
                }
            }

            //test
            int correct=0;
            for(int i=0;i<ilosc_testow/10;i++){
                double input[2]={dane[i].first,dane[i].second};
                vector<double> hidden_out = hidden.forward(input);
                double y_pred = output.forward(hidden_out);
                cout<<"Pred: "<<y_pred<<" Label: "<<labels[i]<<endl;
                if((y_pred>=0.5&&labels[i]==1)||(y_pred<0.5&&labels[i]==0))correct++;
            }
            double accuracy=(double)correct/(double)(ilosc_testow/10);
            save_predictions_to_csv("None","Relu","Sigmoid",learning_rate,accuracy);
        }

        learning_rate+=0.1;
    }
    return 0;
}