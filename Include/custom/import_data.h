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



////////////////////////
//-returned-functions-//
////////////////////////

std::map<int, attraction> getAttractionData(); 
std::map<int, hotel> getHotelData();
std::map<int, intersection> getIntersectionData();

#endif