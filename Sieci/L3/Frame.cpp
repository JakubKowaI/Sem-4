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

int main() {
    frame("Z.txt", "W.txt");
    return 0;
}
