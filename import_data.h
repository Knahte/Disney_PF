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
    std::vector<int> full_ID_list;// list of all attraction IDs
    std::vector<int> ID_list;//List of attraction IDs used for the simulation
    std::vector<int> single_rider_list;// List of attraction IDs for single riders
    bool consider_waiting_times = true; // Consider waiting times ?
    int number_of_path = 100; // The number of different paths created when executing a path search.
    std::string path_to_data = "data"; // The path to the data directory
    bool debug_mode = true; // Enable the Debug output

};



////////////////////////
//-returned-functions-//
////////////////////////

/**
 * The grouping of all the functions used to obtain the attractions map.
 *
 * @param data_folder The link to the data folder.
 * @param current_setting current setting
 *
 * @return The attractions map.
 *
 */
std::map<int, attraction> getAttractionData(std::string data_folder, setting& current_setting);

/**
 * The grouping of all the functions used to obtain the hotels map.
 *
 * @param data_folder The link to the data folder.
 * @param current_setting current setting
 * 
 * @return The hotels map.
 *
 */
std::map<int, hotel> getHotelData(std::string data_folder, setting& current_setting);

/**
 * The grouping of all the functions used to obtain the intersections map.
 *
 * @param data_folder The link to the data folder.
 * @param current_setting_too_use The current settings to use.
 *
 * @return The intersections map.
 *
 */
std::map<int, intersection> getIntersectionData(std::string data_folder, setting& current_setting);
