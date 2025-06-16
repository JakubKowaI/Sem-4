#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <functional>

using namespace std;

// ===================== UTILS ========================
mt19937 mt{
    static_cast<mt19937::result_type>(
        chrono::steady_clock::now().time_since_epoch().count()
    )
};
uniform_real_distribution<> dis(-1.0, 1.0);

// Normalizacje
vector<double> L1(vector<double> x) {
    double sum = 0.0;
    for (double el : x) sum += abs(el);
    if (sum == 0) return x;
    for (double& el : x) el /= sum;
    return x;
}

vector<double> L2(vector<double> x) {
    double sum = 0.0;
    for (double el : x) sum += el * el;
    sum = sqrt(sum);
    if (sum == 0) return x;
    for (double& el : x) el /= sum;
    return x;
}

// Funkcje aktywacji i pochodne
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double sigmoid_derivative(double x) {
    double s = sigmoid(x);
    return s * (1 - s);
}

double relu(double x) {
    return max(0.0, x);
}

double relu_derivative(double x) {
    return x > 0 ? 1.0 : 0.0;
}

// ===================== STRUKTURY NEURONÓW ========================
struct Neuron {
    vector<double> weights;
    double bias;
    double output;
    double z; // surowa suma ważona (przed aktywacją)
    double delta;

    function<double(double)> activation;
    function<double(double)> activation_derivative;

    Neuron(int input_size,
           function<double(double)> act,
           function<double(double)> act_deriv) {
        weights.resize(input_size);
        for (auto& w : weights) w = dis(mt);
        bias = dis(mt);
        activation = act;
        activation_derivative = act_deriv;
    }

    double forward(const vector<double>& input) {
        z = bias;
        for (int i = 0; i < input.size(); ++i)
            z += weights[i] * input[i];
        output = activation(z);
        return output;
    }

    void update_weights(const vector<double>& input, double learning_rate) {
        for (int i = 0; i < weights.size(); ++i)
            weights[i] -= learning_rate * delta * input[i];
        bias -= learning_rate * delta;
    }
};

// ===================== SIEĆ ========================
struct NeuralNetwork {
    vector<Neuron> hidden;
    Neuron output;
    double learning_rate;

    NeuralNetwork(int hidden_size, double lr,
                  function<double(double)> act,
                  function<double(double)> act_deriv)
        : output(hidden_size, sigmoid, sigmoid_derivative),
          learning_rate(lr) {
        for (int i = 0; i < hidden_size; ++i)
            hidden.emplace_back(2, act, act_deriv);
    }

    double forward(const vector<double>& input) {
        vector<double> hidden_outputs;
        for (auto& neuron : hidden)
            hidden_outputs.push_back(neuron.forward(input));
        return output.forward(hidden_outputs);
    }

    void backward(const vector<double>& input, double target) {
        double pred = output.output;
        double error = pred - target;
        output.delta = error * output.activation_derivative(output.z);

        // ukryte neurony
        for (int i = 0; i < hidden.size(); ++i) {
            hidden[i].delta = output.weights[i] *
                              output.delta *
                              hidden[i].activation_derivative(hidden[i].z);
        }

        // aktualizacja wag
        vector<double> hidden_outputs;
        for (auto& h : hidden)
            hidden_outputs.push_back(h.output);

        output.update_weights(hidden_outputs, learning_rate);
        for (auto& h : hidden)
            h.update_weights(input, learning_rate);
    }
};

// ===================== GŁÓWNA PĘTLA ========================
int main() {
    int epochs = 1000;
    int samples = 100;
    double learning_rate = 0.1;

    // wybór trybu
    // auto activation = sigmoid;
    // auto activation_derivative = sigmoid_derivative;
    auto activation = relu;
    auto activation_derivative = relu_derivative;
    auto normalizacja = [](vector<double> x) { return x; };  // bez normalizacji
    // Możesz zmienić np. na: auto normalizacja = L2;

    NeuralNetwork net(4, learning_rate, activation, activation_derivative);

    // Generowanie danych
    vector<vector<double>> X;
    vector<double> Y;
    for (int i = 0; i < samples; ++i) {
        double x1 = dis(mt);
        double x2 = dis(mt);
        X.push_back({x1, x2});
        Y.push_back((x1 * x2 >= 0) ? 1.0 : 0.0);
    }

    // Trenowanie
    for (int epoch = 0; epoch < epochs; ++epoch) {
        double loss_sum = 0.0;
        for (int i = 0; i < samples; ++i) {
            auto input = normalizacja(X[i]);
            double pred = net.forward(input);
            double label = Y[i];
            double loss = 0.5 * pow(pred - label, 2);
            loss_sum += loss;
            net.backward(input, label);
        }
        if (epoch % 100 == 0)
            cout << "Epoch " << epoch << " | Loss: " << loss_sum / samples << endl;
    }

    // Testowanie
    cout << "\nTestowanie po nauce:\n";
    for (int i = 0; i < 10; ++i) {
        auto input = normalizacja(X[i]);
        double pred = net.forward(input);
        cout << "x1 = " << X[i][0] << ", x2 = " << X[i][1]
             << " | label = " << Y[i] << " | pred = " << pred << endl;
    }

    return 0;
}
