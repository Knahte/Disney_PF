#pragma once

#include <vector>
#include <map>
#include <string>

////////////////////////
//--returned--struct--//
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
    bool single_rider = false;
    double entry_time = 6.0;
    int hotel_ID = 0; //please refer to "data.json"
    double walking_speed = 3; //in km/h - (min: 2 | max: 8)
    std::vector<int> full_ID_list = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 31, 32, 33, 34, 35, 36, 37, 40, 2707, 4573, 10845, 10848 };// excluding Single Rider
    std::vector<int>      ID_list = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 31, 32, 33, 34, 35, 36, 37, 40, 2707, 4573, 10845, 10848 };
    //{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 31, 32, 33, 34, 35, 36, 37, 40, 2707, 4573, 7277, 7278, 7279, 7280, 7281, 7306, 10845, 10846, 10848, 10849 }
// SINGLE RIDER { 1, 3, 4, 5, 6, 7, 9, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 31, 33, 36, 40, 2707, 4573, 7277, 7278, 7279, 7280, 7281, 7306, 10846, 10849 }
// NOT SINGLE RIDER { 1, 2, 3, 4, 5, 6, 7, 8, 9, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 31, 32, 33, 34, 35, 36, 37, 40, 2707, 4573, 10845, 10848 }
// SMALL SCALE DEBUG { 5, 7, 4, 6, 9, 4573, 2707, 19, 24, 26, 28, 25, 1, 2 , 10845, 10848 };
    bool consider_waiting_times = true;
};

////////////////////////
//-returned-functions-//
////////////////////////

std::map<int, attraction> getAttractionData(std::string& link_to_data);
std::map<int, hotel> getHotelData(std::string& link_to_data);
std::map<int, intersection> getIntersectionData(std::string& link_to_data);

extern std::string  DATA_LINK_JSON;