#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <chrono>
#include <iomanip>
#include <array>

#include "lodepng.h"

using namespace std;

mt19937 mt{
    static_cast<std::mt19937::result_type>(
        std::chrono::steady_clock::now().time_since_epoch().count()
    )    
};

// Funkcja standaryzująca dane (z-score)
void standardize(vector<vector<double>>& data) {
    if (data.empty()) return;

    size_t num_samples = data.size();       // liczba przykładów
    size_t num_features = data[0].size();   // liczba cech (np. 784 pikseli)

    vector<double> mean(num_features, 0.0);
    vector<double> stddev(num_features, 0.0);

    // 1. Oblicz średnią dla każdej cechy (kolumny)
    for (const auto& sample : data) {
        for (size_t j = 0; j < num_features; ++j) {
            mean[j] += sample[j];
        }
    }
    for (size_t j = 0; j < num_features; ++j) {
        mean[j] /= num_samples;
    }

    // 2. Oblicz odchylenie standardowe dla każdej cechy
    for (const auto& sample : data) {
        for (size_t j = 0; j < num_features; ++j) {
            stddev[j] += pow(sample[j] - mean[j], 2);
        }
    }
    for (size_t j = 0; j < num_features; ++j) {
        stddev[j] = sqrt(stddev[j] / num_samples);
        if (stddev[j] == 0.0) stddev[j] = 1.0; // unikamy dzielenia przez zero
    }

    // 3. Standaryzuj dane
    for (auto& sample : data) {
        for (size_t j = 0; j < num_features; ++j) {
            sample[j] = (sample[j] - mean[j]) / stddev[j];
        }
    }
}

bool load_mnist_csv(const string& filename,
                    vector<vector<double>>& data,
                    vector<int>& labels) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Nie można otworzyć pliku: " << filename << endl;
        return false;
    }

    string line;
    bool first_line = true;

    while (getline(file, line)) {
        stringstream ss(line);
        string cell;

        // Pomijamy pierwszą linię (nagłówki)
        if (first_line) {
            first_line = false;
            continue;
        }

        // Wczytaj etykietę (pierwsza kolumna)
        if (!getline(ss, cell, ',')) continue;

        try {
            int label = stoi(cell);
            labels.push_back(label);
        } catch (...) {
            cerr << "Błąd parsowania etykiety: " << cell << endl;
            continue;
        }

        // Wczytaj 784 wartości pikseli
        vector<double> row;
        while (getline(ss, cell, ',')) {
            try {
                row.push_back(stod(cell));
            } catch (...) {
                row.push_back(0.0); // lub pominąć?
            }
        }

        // Sprawdzenie poprawnej długości
        if (row.size() != 784) {
            cerr << "Błąd: oczekiwano 784 pikseli, otrzymano " << row.size() << endl;
            return false;
        }

        data.push_back(row);
    }

    file.close();
    return true;
}


const int IMAGE_SIZE = 28;

// Zwraca 2D wektor reprezentujący obraz 28x28 z danych wejściowych
vector<vector<double>> to_image_28x28(const vector<double>& flat_input) {
    if (flat_input.size() != IMAGE_SIZE * IMAGE_SIZE) {
        throw runtime_error("Długość wektora nie wynosi 784 (28x28)");
    }

    vector<vector<double>> image(IMAGE_SIZE, vector<double>(IMAGE_SIZE));
    for (int i = 0; i < IMAGE_SIZE; ++i) {
        for (int j = 0; j < IMAGE_SIZE; ++j) {
            image[i][j] = flat_input[i * IMAGE_SIZE + j];
        }
    }

    return image;
}

// Przykład wypisania obrazu w konsoli
void print_image(const vector<vector<double>>& image) {
    for (const auto& row : image) {
        for (double val : row) {
            // Można zmienić np. na int(val * 10) jeśli chcesz wizualnie pogrubić
            cout << setw(6) << fixed << setprecision(2) << val << " ";
        }
        cout << '\n';
    }
}

void save_image_as_png(const vector<vector<double>>& image, const string& filename) {
    const int width = image[0].size();
    const int height = image.size();
    vector<unsigned char> png_image;
    png_image.resize(width * height);

    // Przekształć wartości double (zazwyczaj po standaryzacji) do zakresu 0–255
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            // Zamiast standaryzowanych danych (średnia 0, odch. stand. 1),
            // robimy "denormalizację" – mapujemy np. z [-3, 3] do [0, 255]
            double val = image[i][j];
            val = (val + 3.0) / 6.0 * 255.0; // zakładamy, że większość mieści się w [-3, 3]
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            png_image[i * width + j] = static_cast<unsigned char>(val);
        }
    }

    // Zapisz obraz jako PNG (w odcieniach szarości, czyli 8-bit greyscale)
    unsigned error = lodepng::encode(filename, png_image, width, height, LCT_GREY, 8);

    if (error) {
        cerr << "Błąd podczas zapisu obrazu PNG: " << lodepng_error_text(error) << endl;
    } else {
        cout << "Zapisano obraz do pliku: " << filename << endl;
    }
}

vector<vector<double>> Kmeans(vector<vector<double>> mnist_data, int cetroidNumber) {
    vector<vector<double>> centroidy(cetroidNumber, vector<double>(mnist_data[0].size()));
    vector<vector<int>> assignments(cetroidNumber);
    int num_of_assigments = -1;
    int iterations=0;

    // Losowa inicjalizacja centroidów
    for (int i = 0; i < cetroidNumber; i++) {
        for (int j = 0; j < mnist_data[0].size(); j++) {
            centroidy[i][j] = (mt() % 6000) / 1000.0 - 3.0;  // losowe z [-3, 3]
        }
    }

    while (num_of_assigments != 0&&iterations<100) {
        // for(int i=0;i<centroidy[0].size();i++){
        //     cout<<centroidy[0][i]<<" ";
        // }
        iterations++;
        cout<<iterations<<endl;
        vector<vector<int>> tempAssignments(cetroidNumber);

        // Przypisanie punktów do najbliższych centroidów
        for (int i = 0; i < mnist_data.size(); i++) {
            int closest = 0;
            double minDist = numeric_limits<double>::max();
            for (int j = 0; j < centroidy.size(); j++) {
                double tempDist = 0.0;
                for (int k = 0; k < centroidy[0].size(); k++) {
                    tempDist += pow(mnist_data[i][k] - centroidy[j][k], 2);
                }
                if (tempDist < minDist) {
                    minDist = tempDist;
                    closest = j;
                }
            }
            tempAssignments[closest].push_back(i);
        }

        // Sprawdź, czy przypisania się zmieniły
        num_of_assigments = 0;
        for (int i = 0; i < cetroidNumber; i++) {
            if (assignments[i] != tempAssignments[i]) {
                num_of_assigments++;
            }
        }

        assignments = tempAssignments;

        // Aktualizacja centroidów
        for (int i = 0; i < cetroidNumber; i++) {
            vector<double> newCentroid(mnist_data[0].size(), 0.0);
            if (assignments[i].empty()) continue; // unikamy dzielenia przez 0
            for (int j : assignments[i]) {
                for (int k = 0; k < mnist_data[0].size(); k++) {
                    newCentroid[k] += mnist_data[j][k];
                }
            }
            for (int k = 0; k < newCentroid.size(); k++) {
                newCentroid[k] /= static_cast<double>(assignments[i].size());
            }
            centroidy[i] = newCentroid;
        }
    }

    return centroidy;
}

int main() {
    vector<vector<double>> mnist_data;
    vector<int> mnist_labels;

    if (load_mnist_csv("mnist_train.csv", mnist_data, mnist_labels)) {
        cout << "Wczytano " << mnist_data.size() << " obrazów." << endl;
        cout << "Pierwszy obraz: etykieta = " << mnist_labels[0] << ", pierwsze 5 pikseli: ";
        for (int i = 0; i < 5; ++i) {
            cout << mnist_data[0][i] << " ";
        }
        cout << endl;
    } else {
        cerr << "Błąd podczas wczytywania danych." << endl;
    }

    standardize(mnist_data);

    vector<vector<double>> centroids=Kmeans(mnist_data,10);

    for(int i=0;i<centroids.size();i++){
        auto image = to_image_28x28(centroids[i]);
        save_image_as_png(image, "centroid_"+to_string(i)+".png");
    }
  


    return 0;
}
