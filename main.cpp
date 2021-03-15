#include "yaml-cpp/yaml.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <map>
#include <functional>
#include <exception>

#define PROJECT_DEBUG
const int& MAX_NAME_LENGTH = 60;

class Person {
public:
    Person(std::string name, float speed, std::string bridge):
                Name(name), Speed(speed), Bridge(bridge) {}
    std::string Name;
    float Speed;
    std::string Bridge; 
};

void readAndInitializeData(std::map<std::string, int> &mBridge,
                           std::vector<Person> &lPerson,
                           std::vector<std::string> &seqBridge,
                           std::string &file);
double findFastestTimeToCross(std::map<std::string, int> &bridge,
                              std::vector<Person> &lPerson);


//Function to read ymal file and intitialize containers to represent the data
void readAndInitializeData(std::map<std::string, int> &mBridge,
                              std::vector<Person> &lPerson,
                              std::vector<std::string> &seqBridge,
                              std::string &file){
    YAML::Node bc = YAML::LoadFile("../" + file);

    //initialize map of bridge, key=bridge name (string), 
    //value=length (int), in feet
    for(auto it=bc["Bridge"].begin(); it!=bc["Bridge"].end(); ++it) {
        mBridge.emplace((*it)["name"].as<std::string>(), 
                        (*it)["length"].as<int>());
    }

#ifdef PROJECT_DEBUG
    for(const auto& br : mBridge){
        std::cout << "bridge name " << br.first << std::endl;
        std::cout << "distance " << br.second << std::endl;
    }
#endif

    //initialize sequence of bridge
    for(auto it=bc["Sequence"].begin(); it!=bc["Sequence"].end(); ++it) {
        seqBridge.push_back((*it).as<std::string>());
    }

#ifdef PROJECT_DEBUG
    for(const auto& sb : seqBridge) {
        std::cout << sb << std::endl;
    }
#endif

    //initialize list of person object
    for(auto it=bc["Person"].begin(); it!=bc["Person"].end(); ++it) {
        lPerson.emplace_back((*it)["name"].as<std::string>(), 
                             (*it)["speed"].as<float>(),
                             (*it)["bridge"].as<std::string>());
    }

#ifdef PROJECT_DEBUG
    for(const auto& pr : lPerson){
        std::cout << "Person name " << pr.Name << std::endl;
        std::cout << "Speed " << pr.Speed << std::endl;
        std::cout << "Bridge " << pr.Bridge << std::endl;
    }
#endif
}

//Function to calculate the fastest time hikers take to cross bridges.
double findFastestTimeToCross(std::map<std::string, int> &mBridge,
                              std::vector<Person> &lPerson,
                              std::vector<std::string> &seqBridge) {
    
    std::vector<float> hikerAtBridge;
    double total_time = 0;   //total time the group crossing all bridges, 
                             //in minute
    double fastest_speed = 0;//fastest runner speed in feet/minute
    for(auto it = seqBridge.begin(); it != seqBridge.end(); ++it) {
        std::string &bridge = *it;
        
        //iterate over lPerson, and get the person that starts at each bridge
        for(auto it_p = lPerson.begin(); it_p != lPerson.end(); ++it_p) {
            if(it_p->Bridge == bridge) {
                hikerAtBridge.push_back(it_p->Speed);
            }
        }

#ifdef PROJECT_DEBUG
        std::for_each(hikerAtBridge.begin(), hikerAtBridge.end(), 
                [](const float& dist) {std::cout << dist << " " << std::endl;});
#endif

        auto it_max = max_element(hikerAtBridge.begin(), hikerAtBridge.end());
        fastest_speed = *it_max;
        if(fastest_speed == 0) {
            std::cout << "fastest speed 0" << std::endl;
            return 0;
        }
        
        float last_runner_speed = 0;
        for(auto it_hab = hikerAtBridge.begin(); 
                        it_hab != hikerAtBridge.end(); ++it_hab) {
            //No need to count the time for torch bearer unless he's the only 
            //hiker
            if(it_hab == it_max && hikerAtBridge.size() != 1) continue;
            
            //Time taken for each trip is time taken to cross, and 
            //time taken by the torch bearer to go back. 
            if((*it_hab) != 0) {
                total_time += (mBridge[bridge]/(*it_hab) + 
                            mBridge[bridge]/fastest_speed);
            }
            last_runner_speed = (*it_hab);
        }
        //No need to go back after the last trip 
        if(hikerAtBridge.size() != 0 || last_runner_speed != 0) { 
            total_time -= mBridge[bridge]/fastest_speed;
        }
    }
    return total_time;
}

int main()
{
    std::map<std::string, int> mBridge;    
    std::vector<std::string> seqBridge;
    std::vector<Person> lPerson;    
    std::string file;

    while(true) {
        std::cout << std::endl;
        std::cout << "Input name of yaml file (^C to stop): ";
        std::cin >> file;
        try {
            //This function might throw YAML exception as well as STL exception
            //It's all handled here. We just bail out for the moment.
            readAndInitializeData(mBridge, lPerson, seqBridge, file);
        } catch (std::exception& e) {
            std::cerr << "exception caught: " << e.what() << std::endl;
        }

        double t = findFastestTimeToCross(mBridge, lPerson, seqBridge);
        std::cout << "total speed is " << t << " minute" << std::endl;
        
        //Clean up containers for next use
        mBridge.clear();
        seqBridge.clear();
        lPerson.clear();
    }
    return 0;
}
