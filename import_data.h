#pragma once

#include <vector>
#include <map>
#include <string>

////////////////////////
//--returned--struct--//
////////////////////////

struct coordinate {
    double lon = 0.0; // Longitude
    double lat = 0.0; // Latitude
};

struct attraction {
    int ID = -1; // Attraction ID
    std::string name = "NONE"; // Attraction name
    coordinate location; // Location of the attraction
    std::map<int, int> wait_time = {}; // Wait time for regular visitors
    std::map<int, int> wait_time_sinlge_rider = {}; // Wait time for single riders
    int single_rider = -1; // Single rider information
    std::vector<int> intersection_linked = {}; // Linked intersections
};

struct hotel {
    int ID = -1; // Hotel ID
    std::string name = "NONE"; // Hotel name
    coordinate location; // Location of the hotel
    std::vector<int> intersection_linked = {}; // Linked intersections
};

struct intersection {
    int ID = -1; // Intersection ID
    std::string name = "NONE"; // Intersection name
    coordinate location; // Location of the intersection
    std::vector<int> attraction_linked = {}; // Linked attractions
    std::vector<int> hotel_linked = {}; // Linked hotels
    std::vector<int> intersection_linked = {}; // Linked intersections
};

struct setting {
    bool single_rider = false; // Single rider mode enable ?
    double entry_time = 6.0; // Entry time in hours ( 0 <= X < 24)
    int hotel_ID = 0; // Used hotel ID
    double walking_speed = 3; // Walking speed in km/h
    std::vector<int> full_ID_list = { 1, 5630, 2, 5631, 3, 4, 5, 6, 7, 5621, 5604, 5619, 5737, 5608, 5611, 8, 9, 10, 11, 12, 27, 5617, 13, 14, 15, 16, 5626, 17, 5623, 5622, 5603,5614, 5620, 18, 19, 5618, 5627, 20, 13349, 12, 5629, 22, 5606, 5609, 5624, 5610, 23, 5605, 24, 25, 5613, 26, 7301, 5628, 5612, 5625, 28 }; // list of all attraction IDs
    std::vector<int> ID_list = { 8, 9, 5, 4, 2707, 7, 6, 19, 22, 15, 20, 23, 14, 16, 18, 17, 21, 3, 2, 1, 25, 26, 28, 24, 13, 4573, 29, 37, 32, 35, 34, 36, 31, 10848, 10845, 33, 40 }; //  List of attraction IDs used for the simulation
    std::vector<int> single_rider_list = {}; // List of attraction IDs for single riders
    // DISNEY LAND { 8, 9, 5, 4, 2707, 7, 6, 19, 22, 15, 20, 23, 14, 16, 18, 17, 21, 3, 2, 1, 25, 26, 28, 24, 13, 4573, 29, 37, 32, 35, 34, 36, 31, 10848, 10845, 33, 40 }{ 7277,7278,7279,7280,7281,7306,10846,10849 };
    bool consider_waiting_times = true; // Consider waiting times ?
    int number_of_path = 100; // The number of different paths created when executing a path search.
    bool debug_mode = false; // Enable the Debug output

};



////////////////////////
//-returned-functions-//
////////////////////////

/**
 * The grouping of all the functions used to obtain the attractions map.
 *
 * @param link_to_data The link to the data file in JSON format.
 *
 * @return The attractions map.
 *
 */
std::map<int, attraction> getAttractionData(std::string& link_to_data);

/**
 * The grouping of all the functions used to obtain the hotels map.
 *
 * @param link_to_data The link to the data file in JSON format.
 *
 * @return The hotels map.
 *
 */
std::map<int, hotel> getHotelData(std::string& link_to_data);

/**
 * The grouping of all the functions used to obtain the intersections map.
 *
 * @param link_to_data The link to the data file in JSON format.
 * @param current_setting_too_use The current settings to use.
 *
 * @return The intersections map.
 *
 */
std::map<int, intersection> getIntersectionData(std::string& link_to_data, setting& current_setting);

extern std::string  DATA_LINK_JSON;