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
    char bit;
    int send_time;
    int index;
    int recIndex=0;
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
    bool bounced=false;
};

//std::string load_frame(const std::string& filename,char znak) {
//     std::ifstream file(filename);
//     if (!file.is_open()) {
//         std::cerr << "Error opening file: " << filename << std::endl;
//         return std::string(200, '0'); // Return default frame if file can't be opened
//     }
    
//     std::string line;
//     std::getline(file, line);
//     file.close();
    
//     // Ensure the frame is exactly 200 characters long
//     if (line.size() < 200) {
//         line.append(200 - line.size(), '0'); // Pad with zeros if too short
//     } else if (line.size() > 200) {
//         line = line.substr(0, 200); // Truncate if too long
//     }
    
//     return line;
// }

void propagate(std::vector<char>& medium,std::vector<Packet>& packets) {
    for (auto it = packets.begin(); it != packets.end();) {
        if (it->position >= 0 && it->position < static_cast<int>(medium.size())) {
            medium[it->position] = it->bit;
            if (it->direction)
                it->position++;
            else
                it->position--;
            ++it;
        } else {
            //if(it->bounced){
                it = packets.erase(it);
            // }else{
            //     it->bounced=true;
            //     if(it->direction){
            //         it->direction=false;
            //         it->position--;
            //     }else{
            //         it->direction=true;
            //         it->position++;
            //     }
            // }
        }
    }
}

void detect_collision(std::vector<Packet>& packets,std::vector<Station>& stations) {
    for (auto& i : packets) {
        for (auto& j : packets) {
            if (i.position == j.position&&i.source!=j.source){
                i.bit='X';
                j.bit='X';
                //i.Colliders.push_back(j.source);
                //j.Colliders.push_back(i.source);
            }
        }
    }
    // for (auto& i : packets) {
    //     for (auto& s : stations) {
    //         if (i.position == s.position&&i.source!=&s && i.bit=='X'){
    //             for (auto& temp : i.Colliders) {
    //                 if(temp==&s)s.collision_detected=true;
    //             }
    //         }
    //     }
    // }
}

void station_activity(Station& station, std::vector<Packet>& packets, int time,std::vector<char>& medium) {
    if(station.recIndex==200&&!station.finished&&!station.collision_detected){
        station.finished=true;
        std::cout<<"Station "<<station.bit<< " finished"<<std::endl;
    }

    if (0 == station.send_time && !station.transmitting&&!station.finished) {
        if(medium[station.position]!='-'){
            station.send_time=0;
        }else{
            station.index=0;
            station.collision_detected=false;
            station.transmitting=true;
        }
    }
    if(station.send_time>0){
        station.send_time--;
    }

    if(medium[station.position]=='X'&&station.collision_detected==false){
        station.collision_counter++;
        if(station.collision_counter==16)station.finished=true;
        if(station.collision_counter>10){
            station.send_time = (rand()%(int)pow(2,10))*200;
        }else{
            station.send_time = (rand()%(int)pow(2,station.collision_counter))*MEDIUM_LENGTH*2;
        }
        //station.transmitting=false;
        station.collision_detected=true;
        std::cout<<"Kolizja wykrywyta przez stacje "<<station.bit<<" nadam za  "<< station.send_time<<" o "<<time+station.send_time<<std::endl;
    }
    if (station.transmitting) {
        if (station.recIndex < 200) {
            if(station.collision_detected){
                Packet temp1={station.position,'X',&station,0};
                Packet temp2={station.position,'X',&station,1};
                packets.push_back(temp1);
                packets.push_back(temp2);
            }else{
                //char bit = station.frame[station.index];
                Packet temp1={station.position,station.bit,&station,0};
                Packet temp2={station.position,station.bit,&station,1};
                packets.push_back(temp1);
                packets.push_back(temp2);
            }
            station.recIndex++;
        }else{
            if(station.collision_detected){
                station.recIndex=0;
                station.transmitting=false;
            }
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
    srand(time(NULL));
    //system("./frame.out");
    // std::string frame = load_frame("W.txt");
    // std::cout<<frame.size()<<std::endl;

    std::vector<Station> stations = {
        {10, 'A', 2},
        {30, 'B', 5},
        {70, 'C', 8},
    };
    std::vector<Packet> packets;

    for (int time = 0; time < MAX_TIME; ++time) {
        
        for (auto& s : stations) {
            station_activity(s, packets, time,medium);
        }
        
        detect_collision(packets,stations);
        int doneStations=0;
        for (auto& s : stations) {
                if(s.finished)doneStations++;
            }
        std::fill(medium.begin(), medium.end(), '-');
        propagate(medium,packets);
        print_medium(medium, time);
        //std::this_thread::sleep_for(std::chrono::milliseconds(25));
            if(doneStations==stations.size()){
                std::cout<<"All Stations Finished \n";
                return 1;
            }
    }

    return 0;
}
