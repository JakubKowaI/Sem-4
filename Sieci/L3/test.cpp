#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

// CRC generator: x^8 + x^2 + x + 1 → 100000111
const string GENERATOR = "100000111";
const string FLAG = "1010101010101010101010101010101010101010101010101010101010101011";
const string CRC_32 = "00000100110000010001110110110111"; //Wydaje mi sie ze to jest takie samo zawsze
//const string CRC_32 = "1011"; //Wydaje mi sie ze to jest takie samo zawsze

string decToBinary(int n) {
    string bin = "";
	
    while (n > 0) {
		int bit = n%2;
      	bin.push_back('0' + bit);
        n /= 2;
    }
  
	reverse(bin.begin(), bin.end());
    return bin;
}

// Oblicz CRC dla danego ciągu bitów
string calculateCRC(const string& input) {
    string data = input + string(CRC_32.length() - 1, '0');
    string remainder = data;

    for (size_t i = 0; i <= data.length() - CRC_32.length(); i++) {
        if (remainder[i] == '1') {
            for (size_t j = 0; j < CRC_32.length(); j++) {
                remainder[i + j] = (remainder[i + j] == CRC_32[j]) ? '0' : '1';
            }
        }
    }

    return remainder.substr(remainder.length() - (CRC_32.length() - 1));
}

string calculateStuffedCRC(const string& input) {
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

string check(string& data){
    while(data.length()<46*8){
        data+='0';
    }
return data;
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

void frame(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile);
    ofstream out(outputFile);

    string bits;
    getline(in, bits);
    if(bits.length()>1500*8)return;
    string dataLength=decToBinary(bits.length());
    bits=check(bits);

    string crc = calculateCRC(bits);
    string full =dataLength + bits + crc;
    //string stuffed = bitStuffing(full);
    // string framed = FLAG + stuffed;
    string framed = FLAG + full + FLAG;

    out << framed << endl;

    in.close();
    out.close();
}

void stuffedFrame(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile);
    ofstream out(outputFile);

    string bits;
    getline(in, bits);
    if(bits.length()>1500*8)return;
    //string dataLength=decToBinary(bits.length());
    //bits=check(bits);

    string crc = calculateStuffedCRC(bits);
    string full =bits + crc;
    string stuffed = bitStuffing(full);
    string framed = FLAG + stuffed + FLAG;
    //string framed = FLAG + full;

    out << framed << endl;

    in.close();
    out.close();
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
    //string full = bitUnstuffing(stuffed);
    string dataLength=stuffed.substr(0,15);

    size_t crcLen = CRC_32.length() - 1;
    string data = stuffed.substr(15, stuffed.size() - crcLen);
    string crc = stuffed.substr(stuffed.size() - crcLen);

    if (calculateCRC(data) == crc) {
        out << data << endl;
        cout << "CRC OK: dane poprawne.\n";
    } else {
        cerr << "CRC ERROR: dane uszkodzone.\n";
    }

    in.close();
    out.close();
}

void stuffedDeframe(const string& inputFile, const string& outputFile) {
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

    if (calculateStuffedCRC(data) == crc) {
        out << data << endl;
        cout << "CRC OK: dane poprawne.\n";
    } else {
        cerr << "CRC ERROR: dane uszkodzone.\n";
    }

    in.close();
    out.close();
}

int main() {
    cout << "Wybierz opcję:\n1. Ramkuj (Z → W)\n2. Odramkuj (W → Z)\n> ";
    int choice;
    cin >> choice;

    if (choice == 1) {
        frame("Z.txt", "W.txt");
        //stuffedFrame("Z.txt", "W.txt");
        cout << "Zapisano do W.txt\n";
    } else if (choice == 2) {
        deframe("W.txt", "Z_out.txt");
        //stuffedDeframe("W.txt", "Z_out.txt");
        cout << "Zapisano do Z_out.txt\n";
    } else {
        cerr << "Nieznana opcja.\n";
    }

    return 0;
}
