#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const string CRC32 = "00000100110000010001110110110111";
const string FLAG = "01111110";

string calculateCRC(const string& input) {
    string data = input + string(CRC32.length() - 1, '0');
    string remainder = data;

    for (size_t i = 0; i <= data.length() - CRC32.length(); ++i) {
        if (remainder[i] == '1') {
            for (size_t j = 0; j < CRC32.length(); ++j) {
                remainder[i + j] = (remainder[i + j] == CRC32[j]) ? '0' : '1';
            }
        }
    }
    return remainder.substr(remainder.length() - (CRC32.length() - 1));
}

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

    size_t crcLen = CRC32.length() - 1;
    string data = full.substr(0, full.size() - crcLen);
    string crc = full.substr(full.size() - crcLen);

    if (calculateCRC(data) == crc) {
        out << data << endl;
        cout << "CRC OK: dane poprawne.\n";
    } else {
        out << "CRC ERROR: dane uszkodzone.\n" << endl;
        cerr << "CRC ERROR: dane uszkodzone.\n";
    }

    in.close();
    out.close();
}

int main() {
    deframe("W.txt", "Z_out.txt");
    return 0;
}
