#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <iomanip>

constexpr int MEDIUM_LENGTH = 100;
constexpr int MAX_TIME = 200000;
constexpr int NUM_STATIONS = 3;
constexpr int SIGNAL_SPEED = 1;
constexpr int BACKOFF_TIME = 5;

struct Station {
    int position;
    std::string frame;
    int send_time;
    int index;
    int collision_counter=0;
    bool transmitting = false;
    bool collision_detected = false;
    bool finished=false;
};

struct Packet {
    int position;
    char bit;
    Station* source;
    bool direction;
    std::vector<Station*> Colliders;
};

std::string load_frame(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line);
    return line;
}

void propagate(std::vector<char>& medium,std::vector<Packet>& packets) {
    for (auto it = packets.begin(); it != packets.end(); ) {
        if (it->position >= 0 && it->position < static_cast<int>(medium.size())) {
            medium[it->position] = it->bit;
            if (it->direction)
                it->position++;
            else
                it->position--;
            ++it;
        } else {
            it = packets.erase(it);
        }
    }
}

void detect_collision(std::vector<Packet>& packets,std::vector<Station>& stations) {
    for (auto& i : packets) {
        for (auto& j : packets) {
            if (i.position == j.position&&i.source!=j.source){
                i.bit='X';
                j.bit='X';
                i.Colliders.push_back(j.source);
                j.Colliders.push_back(i.source);
            }
        }
    }
    for (auto& i : packets) {
        for (auto& s : stations) {
            if (i.position == s.position&&i.source!=&s && i.bit=='X'){
                for (auto& temp : i.Colliders) {
                    if(temp==&s)s.collision_detected=true;
                }
            }
        }
    }
}

void station_activity(Station& station, std::vector<Packet>& packets, int time) {
    if (time == station.send_time && !station.transmitting) {
        station.index=0;
        station.transmitting=true;
    }
    if(station.collision_detected){
        station.collision_counter++;
        station.send_time = time + (BACKOFF_TIME + rand() % 10)*station.collision_counter;
        station.transmitting=false;
        station.finished=false;
        station.collision_detected=false;
        //std::cout<<"Kolizja wykrywyta przez stacje "<<station.position<<" zaraz znowu nadam o "<< station.send_time<<std::endl;
    }
    if (station.transmitting) {
        
        if (station.index < station.frame.size()) {
            char bit = station.frame[station.index];
            Packet temp1={station.position,bit,&station,0};
            Packet temp2={station.position,bit,&station,1};
            packets.push_back(temp1);
            packets.push_back(temp2);
            station.index++;
        } else {
            station.transmitting = false;
            station.finished=true;
            //std::cout<<"Station "<<station.position<<" finished\n";
        }
    }
    
}

void print_medium(const std::vector<char>& medium, int time) {
    std::cout << "[" <<std::setw(4)<< time << "] ";
    for (char c : medium) std::cout << c;
    std::cout << "\n";
}

int main() {
    std::vector<char> medium(MEDIUM_LENGTH, '-');

    //system("./frame.out");
    std::string frame = load_frame("W.txt");

    std::vector<Station> stations = {
        {10, frame, 2},
        {30, frame, 5},
        {70, frame, 8},
    };
    std::vector<Packet> packets;

    for (int time = 0; time < MAX_TIME; ++time) {
        
        for (auto& s : stations) {
            station_activity(s, packets, time);
        }
        
        detect_collision(packets,stations);
        
            for (auto& s : stations) {
                if (s.transmitting&&s.collision_detected) {
                    s.transmitting = false;
                    s.collision_detected = false;
                    s.send_time += BACKOFF_TIME + rand() % 16;
                    std::cout << "!! KOLIZJA WYSTĄPIŁA !!\n";
                }
            }
        std::fill(medium.begin(), medium.end(), '-');
        propagate(medium,packets);
        print_medium(medium, time);
        std::this_thread::sleep_for(std::chrono::milliseconds(75));
    }

    return 0;
}
