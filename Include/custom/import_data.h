#ifndef IMPORTDATA_H
#define IMPORTDATA_H

#include <vector>
#include <map>
#include <string>

////////////////////////
//-------struct-------//
////////////////////////

struct coordinate { 
    double lon = 0.0;
    double lat = 0.0;
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

struct setting {
    bool single_rider = true;
    double entry_time = 6;
    int available_days = 3;
    int hotel_ID = 2; //see Json file "data.json"
    double walking_speed = 3; //in km/h - (min: 2 | max: 8)
    std::vector<int> ID_list = {1, 3, 4, 5, 6, 7, 9, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 31, 33, 36, 40, 2707, 4573, 7277, 7278, 7279, 7280, 7281, 7306, 10846, 10849};
};

////////////////////////
//-returned-functions-//
////////////////////////

std::map<int, attraction> getAttractionData(); 
std::map<int, hotel> getHotelData();
std::map<int, intersection> getIntersectionData();

#endif