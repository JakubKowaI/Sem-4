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
#include <map>
#include <algorithm>

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

void save_matrix_as_svg(const map<int, vector<int>>& Klusters,
                        const vector<int>& mnist_labels,
                        const string& filename) {
    // Tworzymy macierz częstości: [liczba_klastrów][10]
    int num_clusters = Klusters.size();
    vector<vector<double>> matrix(num_clusters, vector<double>(10, 0.0));

    // Wypełnianie macierzy: zliczamy etykiety (0-9) w każdym klastrze
    int row = 0;
    for (const auto& [cluster_id, indices] : Klusters) {
        for (int idx : indices) {
            int label = mnist_labels[idx];
            if (label >= 0 && label <= 9) {
                matrix[row][label] += 1.0;
            }
        }
        row++;
    }

    // Normalizacja (opcjonalnie): dziel przez liczbę obrazów w klastrze, żeby uzyskać udziały
    for (int i = 0; i < num_clusters; ++i) {
        double sum = accumulate(matrix[i].begin(), matrix[i].end(), 0.0);
        if (sum > 0) {
            for (int j = 0; j < 10; ++j) {
                matrix[i][j] /= sum;
            }
        }
    }

    // Zapisujemy SVG z macierzy
    const int cell_size = 40;
    const int width = 10;
    const int height = num_clusters;
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
            double val = matrix[i][j];  // w zakresie 0..1
            int gray = static_cast<int>((1.0 - val) * 255);  // im większa wartość, tym jaśniejsze tło
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

    // Etykiety kolumn (cyfry 0..9)
    for (int j = 0; j < width; ++j) {
        file << "<text x=\"" << (j * cell_size + 100 + cell_size / 2)
             << "\" y=\"25\" font-size=\"14\" text-anchor=\"middle\" fill=\"white\">"
             << j << "</text>\n";
    }

    // Etykiety wierszy (klastry: klaster_0, klaster_1, ...)
    for (int i = 0; i < height; ++i) {
        file << "<text x=\"90\" y=\"" << (i * cell_size + 40 + cell_size / 2 + 5)
             << "\" font-size=\"14\" text-anchor=\"end\" fill=\"white\">"
             << "klaster_" << i << "</text>\n";
    }

    file << "</svg>\n";
    file.close();
    cout << "Macierz klaster-label zapisana jako SVG: " << filename << endl;
}




vector<int> regionQuery(const vector<vector<double>>& data, int pointIdx, double eps) {
    vector<int> neighbors;
    for (int i = 0; i < data.size(); ++i) {
        if (i == pointIdx) continue;

        double dist = 0.0;
        for (int j = 0; j < data[i].size(); ++j) {
            dist += pow(data[pointIdx][j] - data[i][j], 2);
        }

        if (sqrt(dist) <= eps) {
            neighbors.push_back(i);
        }
    }
    return neighbors;
}

void expandCluster(const vector<vector<double>>& data,
                   vector<int>& labels,
                   vector<bool>& visited,
                   int pointIdx,
                   int clusterId,
                   double eps,
                   int minPts) {
    vector<int> seeds = regionQuery(data, pointIdx, eps);
    labels[pointIdx] = clusterId;
    visited[pointIdx] = true;

    size_t i = 0;
    while (i < seeds.size()) {
        int curr = seeds[i];

        if (!visited[curr]) {
            visited[curr] = true;
            vector<int> result = regionQuery(data, curr, eps);

            if (result.size() >= minPts) {
                for (int idx : result) {
                    if (!visited[idx] && find(seeds.begin(), seeds.end(), idx) == seeds.end()) {
                        seeds.push_back(idx);
                    }
                }
            }
        }

        if (labels[curr] == 0) { // 0 = niezdefiniowany
            labels[curr] = clusterId;
        }

        ++i;
    }
}



vector<int> DBSCAN(const vector<vector<double>>& data, double eps, int minPts) {
    int clusterId = 0;
    vector<int> labels(data.size(), 0); // 0 = niezbadany, -1 = szum, >0 = nr klastra
    vector<bool> visited(data.size(), false);


    for (int i = 0; i < data.size(); ++i) {
        if (labels[i] != 0) continue;

        vector<int> neighbors = regionQuery(data, i, eps);

        if (neighbors.size() < minPts) {
            labels[i] = -1; // szum
        } else {
            ++clusterId;
            expandCluster(data, labels, visited, i, clusterId, eps, minPts);
        }
    }

    cout << "Znaleziono " << clusterId << " klastrów." << endl;
    return labels;
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

    double eps = 10.0;     // dobierz eksperymentalnie, np. 10.0, 15.0, ...
    int minPts = 5;

    vector<int> dbscan_labels = DBSCAN(mnist_data, eps, minPts);
    vector<int> klastry;
    for(int i=0;i<dbscan_labels.size();i++){
        bool is_in=false;
        for(int t : klastry){
            if(dbscan_labels[i]==t){
                is_in=true;
            }
        }
        if(!is_in){
            cout<<"Dodaje "<<dbscan_labels[i]<<endl;
            klastry.push_back(dbscan_labels[i]);
        }
    }

    double szum=0.0;
    
    map<int, vector<int>> Klusters;
    for (int i = 0; i < dbscan_labels.size(); ++i) {
        if (dbscan_labels[i] > 0) {
            Klusters[dbscan_labels[i]].push_back(i);
        }else{
            szum+=1.0;
        }
    }
    szum=szum/dbscan_labels.size();




    save_matrix_as_svg(Klusters,mnist_labels, "klastry_DBSCAN.svg");


    return 0;
}
