#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// CRC generator: x^8 + x^2 + x + 1 → 100000111
const string GENERATOR = "100000111";
const string FLAG = "01111110";

// Oblicz CRC dla danego ciągu bitów
string calculateCRC(const string& input) {
    string data = input + string(GENERATOR.length() - 1, '0');
    string remainder = data;

    for (size_t i = 0; i <= data.length() - GENERATOR.length(); ++i) {
        if (remainder[i] == '1') {
            for (size_t j = 0; j < GENERATOR.length(); ++j) {
                remainder[i + j] = (remainder[i + j] == GENERATOR[j]) ? '0' : '1';
            }
        }
    }

    return remainder.substr(remainder.length() - (GENERATOR.length() - 1));
}

// Rozpychanie bitów: po każdych pięciu '1' dodaj '0'
string bitStuffing(const string& data) {
    string stuffed;
    int oneCount = 0;

    for (char bit : data) {
        stuffed += bit;
        if (bit == '1') {
            oneCount++;
            if (oneCount == 5) {
                stuffed += '0';
                oneCount = 0;
            }
        } else {
            oneCount = 0;
        }
    }

    return stuffed;
}

// Cofanie rozpychania bitów
string bitUnstuffing(const string& data) {
    string unstuffed;
    int oneCount = 0;

    for (size_t i = 0; i < data.size(); ++i) {
        unstuffed += data[i];
        if (data[i] == '1') {
            oneCount++;
            if (oneCount == 5) {
                i++; // pomiń dodatkowe '0'
                oneCount = 0;
            }
        } else {
            oneCount = 0;
        }
    }

    return unstuffed;
}

// RAMKOWANIE: Z → W
void frame(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile);
    ofstream out(outputFile);

    string bits;
    getline(in, bits);

    string crc = calculateCRC(bits);
    string full = bits + crc;
    string stuffed = bitStuffing(full);
    string framed = FLAG + stuffed + FLAG;

    out << framed << endl;

    in.close();
    out.close();
}

// DEFRAMKOWANIE I WALIDACJA: W → Z
void deframe(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile);
    ofstream out(outputFile);

    string line;
    getline(in, line);

    size_t start = line.find(FLAG);
    size_t end = line.rfind(FLAG);

    if (start == string::npos || end == string::npos || start == end) {
        cerr << "Brak poprawnych ramek.\n";
        return;
    }

    string stuffed = line.substr(start + FLAG.length(), end - start - FLAG.length());
    string full = bitUnstuffing(stuffed);

    size_t crcLen = GENERATOR.length() - 1;
    string data = full.substr(0, full.size() - crcLen);
    string crc = full.substr(full.size() - crcLen);

    if (calculateCRC(data) == crc) {
        out << data << endl;
        cout << "CRC OK: dane poprawne.\n";
    } else {
        cerr << "CRC ERROR: dane uszkodzone.\n";
    }

    in.close();
    out.close();
}

// MAIN
int main() {
    cout << "Wybierz opcję:\n1. Ramkuj (Z → W)\n2. Odramkuj (W → Z)\n> ";
    int choice;
    cin >> choice;

    if (choice == 1) {
        frame("Z.txt", "W.txt");
        cout << "Zapisano do W.txt\n";
    } else if (choice == 2) {
        deframe("W.txt", "Z_out.txt");
        cout << "Zapisano do Z_out.txt\n";
    } else {
        cerr << "Nieznana opcja.\n";
    }

    return 0;
}
