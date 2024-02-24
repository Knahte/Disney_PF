#ifndef IMPORTDATA_H
#define IMPORTDATA_H

#include <vector>
#include <map>
#include <string>

////////////////////////
//-------struct-------//
////////////////////////

struct coordinate {
    float lon = 0.0;
    float lat = 0.0;
};

struct attraction {
    int ID = -1;
    std::string name = "NONE";
    coordinate location;
    std::map<int, int> wait_time = {};
    int single_rider = -1;
    std::vector<int> intersection_linked = {};
    bool visited = true;
};

struct hotel {
    int ID = -1;
    std::string name = "NONE";
    coordinate location;
    std::vector<int> intersection_linked = {};
};

struct intersection {
    int ID = -1;
    std::string name = "NONE";
    coordinate location;
    std::vector<int> attraction_linked = {};
    std::vector<int> hotel_linked = {};
    std::vector<int> intersection_linked = {};
};

// Déclarez les signatures des fonctions ici

std::map<int, attraction> getAttractionData(); // Par exemple
std::map<int, hotel> getHotelData();
std::map<int, intersection> getIntersectionData();

#endif // IMPORT_DATA_H