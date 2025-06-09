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

void standardize(vector<vector<double>>& data) {
    if (data.empty()) return;

    size_t num_samples = data.size();       
    size_t num_features = data[0].size();   

    vector<double> mean(num_features, 0.0);
    vector<double> stddev(num_features, 0.0);

    for (const auto& sample : data) {
        for (size_t j = 0; j < num_features; ++j) {
            mean[j] += sample[j];
        }
    }
    for (size_t j = 0; j < num_features; ++j) {
        mean[j] /= num_samples;
    }

    for (const auto& sample : data) {
        for (size_t j = 0; j < num_features; ++j) {
            stddev[j] += pow(sample[j] - mean[j], 2);
        }
    }
    for (size_t j = 0; j < num_features; ++j) {
        stddev[j] = sqrt(stddev[j] / num_samples);
        if (stddev[j] == 0.0) stddev[j] = 1.0; 
    }

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

        if (first_line) {
            first_line = false;
            continue;
        }

        if (!getline(ss, cell, ',')) continue;

        try {
            int label = stoi(cell);
            labels.push_back(label);
        } catch (...) {
            cerr << "Błąd parsowania etykiety: " << cell << endl;
            continue;
        }

        vector<double> row;
        while (getline(ss, cell, ',')) {
            try {
                row.push_back(stod(cell));
            } catch (...) {
                row.push_back(0.0);
            }
        }

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

void print_image(const vector<vector<double>>& image) {
    for (const auto& row : image) {
        for (double val : row) {
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

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double val = image[i][j];
            val = (val + 3.0) / 6.0 * 255.0; 
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            png_image[i * width + j] = static_cast<unsigned char>(val);
        }
    }

    unsigned error = lodepng::encode(filename, png_image, width, height, LCT_GREY, 8);

    if (error) {
        cerr << "Błąd podczas zapisu obrazu PNG: " << lodepng_error_text(error) << endl;
    } else {
        cout << "Zapisano obraz do pliku: " << filename << endl;
    }
}

void save_matrix_as_png(const vector<vector<double>>& matrix, const string& filename) {
    const int height = matrix.size();        // liczba klastrów
    const int width = matrix[0].size();      // liczba etykiet (np. 10)
    
    vector<unsigned char> png_image(width * height);
    
    // Znajdź maksymalną wartość (powinno być ~1.0)
    double max_val = 0.0;
    for (const auto& row : matrix) {
        for (double val : row) {
            if (val > max_val) max_val = val;
        }
    }
    if (max_val == 0.0) max_val = 1.0;

    // Skalowanie i zapis
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double val = matrix[i][j] / max_val;
            val *= 255.0;
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            png_image[i * width + j] = static_cast<unsigned char>(val);
        }
    }

    unsigned error = lodepng::encode(filename, png_image, width, height, LCT_GREY, 8);
    if (error) {
        cerr << "Błąd zapisu PNG: " << lodepng_error_text(error) << endl;
    } else {
        cout << "Macierz zapisana jako: " << filename << endl;
    }
}

void save_matrix_as_svg(const vector<vector<double>>& matrix, const string& filename) {
    const int cell_size = 40;
    const int width = matrix[0].size();
    const int height = matrix.size();
    const int image_width = cell_size * width + 160;
    const int image_height = cell_size * height + 100;

    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Nie można zapisać SVG do pliku: " << filename << endl;
        return;
    }

    file << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << image_width
         << "\" height=\"" << image_height << "\" font-family=\"Arial\">\n";

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double val = matrix[i][j];
            int gray = static_cast<int>(val * 255);
            file << "<rect x=\"" << (j * cell_size + 100)
                 << "\" y=\"" << (i * cell_size + 40)
                 << "\" width=\"" << cell_size << "\" height=\"" << cell_size
                 << "\" fill=\"rgb(" << gray << "," << gray << "," << gray << ")\" />\n";

            file << "<text x=\"" << (j * cell_size + 100 + cell_size / 2)
                 << "\" y=\"" << (i * cell_size + 40 + cell_size / 2 + 5)
                 << "\" font-size=\"12\" text-anchor=\"middle\" fill=\""
                 << (gray < 128 ? "white" : "black") << "\">"
                 << fixed << setprecision(2) << val << "</text>\n";
        }
    }

    // Etykiety kolumn (label_0, label_1, ...) w kolorze białym
    for (int j = 0; j < width; ++j) {
        file << "<text x=\"" << (j * cell_size + 100 + cell_size / 2)
             << "\" y=\"25\" font-size=\"14\" text-anchor=\"middle\" fill=\"white\">"
             << "" << j << "</text>\n";
    }

    // Etykiety wierszy (klaster_0, klaster_1, ...) w kolorze białym
    for (int i = 0; i < height; ++i) {
        file << "<text x=\"90\" y=\"" << (i * cell_size + 40 + cell_size / 2 + 5)
             << "\" font-size=\"14\" text-anchor=\"end\" fill=\"white\">"
             << "klaster_" << i << "</text>\n";
    }

    file << "</svg>\n";
    file.close();
    cout << "Macierz zapisana jako SVG: " << filename << endl;
}




vector<vector<double>> Kmeans(vector<vector<double>> mnist_data, vector<vector<int>> assignments,vector<int> labels) {
    vector<vector<double>> centroidy(assignments.size(), vector<double>(mnist_data[0].size()));
    
    int num_of_assigments = -1;
    int iterations=0;

    for (int i = 0; i < assignments.size(); i++) {
        for (int j = 0; j < mnist_data[0].size(); j++) {
            centroidy[i][j] = (mt() % 6000) / 1000.0 - 3.0;
        }
    }

    while (num_of_assigments != 0&&iterations<100) {
        // for(int i=0;i<centroidy[0].size();i++){
        //     cout<<centroidy[0][i]<<" ";
        // }
        iterations++;
        cout<<iterations<<endl;
        vector<vector<int>> tempAssignments(assignments.size());

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
        for (int i = 0; i < tempAssignments.size(); i++) {
            if (assignments[i] != tempAssignments[i]) {
                num_of_assigments++;
            }
        }

        assignments = tempAssignments;

        // Aktualizacja centroidów
        for (int i = 0; i < tempAssignments.size(); i++) {
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
    vector<vector<double>> klastry(assignments.size(), vector<double>(10, 0.0));
    for(int i=0;i<assignments.size();i++){
        for(int j=0;j<assignments[i].size();j++){
            klastry[i][labels[assignments[i][j]]]+=1.0;
        }
    }

    for(int i=0;i<10;i++){
        double suma=0;
        for(int j=0;j<klastry.size();j++){
            suma+=klastry[j][i];
        }
        for(int j=0;j<klastry.size();j++){
            klastry[j][i]=klastry[j][i]/suma;
        }
    }

    save_matrix_as_svg(klastry, "klastry_"+to_string(assignments.size())+".svg");


    return centroidy;
}

int main() {
    vector<vector<double>> mnist_data;
    vector<int> mnist_labels;

    if (load_mnist_csv("mnist_train.csv", mnist_data, mnist_labels)) {
        cout << "Wczytano " << mnist_data.size() << " obrazów." << endl;
        // cout << "Pierwszy obraz: etykieta = " << mnist_labels[0] << ", pierwsze 5 pikseli: ";
        // for (int i = 0; i < 5; ++i) {
        //     cout << mnist_data[0][i] << " ";
        // }
        // cout << endl;
    } else {
        cerr << "Błąd podczas wczytywania danych." << endl;
    }

    standardize(mnist_data);
    int cetroidNumber=10;
    vector<vector<int>> assignments(cetroidNumber);
    vector<vector<double>> centroids=Kmeans(mnist_data,assignments,mnist_labels);

    for(int i=0;i<centroids.size();i++){
        auto image = to_image_28x28(centroids[i]);
        save_image_as_png(image, "centroid_"+to_string(i)+".png");
    }
  


    return 0;
}
