#include "import_data.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Include/nlohmann/json.hpp"

using json = nlohmann::json;

static setting current_setting; // the used setting (not important) just to used the full_ID_list

////////////////////////
//---data-functions---//
////////////////////////

/**
 * Opens the JSON file and extracts the attractions to store them in a vector of attractions.
 *
 * @param data_URL The file path to the JSON database.
 *
 * @return A vector containing attractions extracted from the JSON file.
 * 
 */
static std::vector<attraction> attractionsOpenJson(std::string& data_URL) {
    std::vector<attraction> attractions; // Vector to store attractions

    std::ifstream file(data_URL); // Open the JSON file
    if (!file.is_open()) { // Check if the file is successfully opened
        std::cerr << "Erreur : Impossible d'ouvrir le fichier JSON." << std::endl;
        return attractions; // Return empty vector
    }

    json data; // JSON object to store data read from the file


    try {
        file >> data; // Read JSON data from the file

        // Iterate over each attraction data in the JSON
        for (json& attraction_data : data["Attractions"]) {

            // Create a new attraction object
            attraction new_attraction; 


            // Extract attraction data
            new_attraction.ID = attraction_data["id"]; 
            new_attraction.name = attraction_data["name"];
            new_attraction.location.lon = attraction_data["location"]["lon"]; 
            new_attraction.location.lat = attraction_data["location"]["lat"];

            if (attraction_data["single_rider"] == 0)  // Check if the attraction has a single_rider variant
                new_attraction.single_rider = attraction_data["id"]; // If it doesn't exist, use the attraction ID for its own single rider ID
            else
                new_attraction.single_rider = attraction_data["single_rider"]; // Otherwise, extract single rider information
            

            // Add the new attraction to the vector
            attractions.push_back(new_attraction);
        }
    }
    catch (std::exception& error) { // Catch any exceptions that occur during JSON parsing
        std::cerr << "Erreur lors de l'analyse du fichier JSON : " << error.what() << std::endl;
    }

    file.close();

    return attractions;

}

/**
 * Opens the JSON file and extracts the hotels to store them in a vector of hotels.
 *
 * @param data_URL The file path to the JSON database.
 *
 * @return A vector containing hotels extracted from the JSON file.
 * 
 */
static std::vector<hotel> hotelOpenJson(std::string& data_URL) {
    std::vector<hotel> hotels; // Vector to store hotels

    std::ifstream file(data_URL); // Open the JSON file
    if (!file.is_open()) { // Check if the file is successfully opened
        std::cerr << "Erreur : Impossible d'ouvrir le fichier JSON." << std::endl;
        return hotels; // Return empty vector
    }

    json data;// JSON object to store data read from the file

    try {
        file >> data; // Read JSON data from the file

        // Iterate over each hotel data in the JSON
        for (json& hotel_data : data["Hotels"]) {

            // Create a new hotel object
            hotel new_hotel; 

            // Extract hotel data
            new_hotel.ID = hotel_data["id"];
            new_hotel.name = hotel_data["name"];
            new_hotel.location.lon = hotel_data["location"]["lon"];
            new_hotel.location.lat = hotel_data["location"]["lat"];


            hotels.push_back(new_hotel); // Add the new hotel to the vector


        }
    }
    catch (std::exception& error) { // Catch any exceptions that occur during JSON parsing
        std::cerr << "Erreur lors de l'analyse du fichier JSON : " << error.what() << std::endl;
    }

    file.close();

    return hotels;
}

/**
 * Opens the JSON file and extracts the intersections to store them in a vector of intersections.
 *
 * @param data_URL The file path to the JSON database.
 * @param attraction_data A map containing attraction data.
 *
 * @return A vector containing intersections extracted from the JSON file.
 * 
 */
static std::vector<intersection> intersectionOpenJson(std::string& data_URL, std::map<int, attraction>& attraction_data) {
    std::vector<intersection> intersections; // Vector to store intersections

    std::ifstream file(data_URL); // Open the JSON file
    if (!file.is_open()) { // Check if the file is successfully opened
        std::cerr << "Erreur : Impossible d'ouvrir le fichier JSON." << std::endl;
        return intersections; // Return empty vector
    }

    json data;// JSON object to store data read from the file

    try {
        file >> data; // Read JSON data from the file

        // Iterate over each intersection data in the JSON
        for (json& inter_data : data["Intersections"]) {

            // Create a new intersection object
            intersection new_inter;


            // Extract intersection data
            new_inter.ID = inter_data["id"];
            new_inter.name = inter_data["name"];
            new_inter.location.lon = inter_data["location"]["lon"];
            new_inter.location.lat = inter_data["location"]["lat"];

            // Extract connected intersections
            for (auto& inter : inter_data["connected_to_intersections"]) {
                new_inter.intersection_linked.push_back(inter);
            }

            // Extract connected attractions
            for (auto& attraction_ID : inter_data["connected_to_attractions"]) {

                if (std::find(current_setting.full_ID_list.begin(), current_setting.full_ID_list.end(), attraction_ID) != current_setting.full_ID_list.end()) { // Check if the attraction ID is in the list of full IDs
                    new_inter.attraction_linked.push_back(attraction_ID); // Add connected attraction ID to the vector attraction_linked
                    new_inter.intersection_linked.push_back(attraction_ID * 1000); // Add the ID of the connected attraction (with 3 extra zeros at the end) to the vector intersection_linked.
                }
            }

            // Extract connected hotels
            for (auto& hotel : inter_data["connected_to_hotels"]) {
                new_inter.hotel_linked.push_back(hotel); // Add connected hotel ID to the vector hotel_linked
            }

            // Add the new intersection to the vector
            intersections.push_back(new_inter);
        }

        // Iterate over each attraction data in the JSON
        for (json& attr_data : data["Attractions"]) {

            // Check if the attraction ID is in the list of full IDs
            if (std::find(current_setting.full_ID_list.begin(), current_setting.full_ID_list.end(), attr_data["id"]) != current_setting.full_ID_list.end()) {

                // Create a new intersection object
                intersection new_inter;


                new_inter.ID = attr_data["id"] * 1000; // Generate a unique ID for the attraction's intersection

                // Extract attraction data
                new_inter.name = attr_data["name"];
                new_inter.location.lon = attr_data["location"]["lon"];
                new_inter.location.lat = attr_data["location"]["lat"];

                // Extract connected intersections from attraction data
                for (auto& intersection_ID : attraction_data[attr_data["id"]].intersection_linked) {
                    if (intersection_ID != new_inter.ID) {
                        // Add connected intersection ID to the vector intersection_linked
                        new_inter.intersection_linked.push_back(intersection_ID);

                    }
                }

                // Extract direct way to attractions from attraction data
                for (auto& attraction_ID : attr_data["direct_way_to_attractions"]) {
                    if (std::find(current_setting.full_ID_list.begin(), current_setting.full_ID_list.end(), attraction_ID) != current_setting.full_ID_list.end())
                    // Add direct way to attraction's intersection ID (with 3 extra zeros at the end) to the vector intersection_linked
                    new_inter.intersection_linked.push_back(attraction_ID * 1000); 
                }

                // Add the ID of its own attraction to its attraction_linked vector
                new_inter.attraction_linked.push_back(attr_data["id"]); 


                // Add the new intersection to the vector
                intersections.push_back(new_inter);

            }
        }
    }

    catch (std::exception& error) { // Catch any exceptions that occur during JSON parsing
        std::cerr << "Erreur lors de l'analyse du fichier JSON : " << error.what() << std::endl;
    }

    file.close(); 

    return intersections; // Return the vector of intersections
}

/**
 * Convert a vector of attractions into a map with attraction IDs as keys.
 *
 * @param attractions_list A vector containing attractions.
 *
 * @return A map with attraction IDs as keys and attractions as values.
 * 
 */
static std::map<int, attraction> attractionVectorToMapById(std::vector<attraction>& attractions_list) {
    std::map<int, attraction> attractions_map; // Create a map to store attractions

    // Iterate over each attraction in the vector
    for (attraction& attr : attractions_list) {
        attractions_map[attr.ID] = attr; // Assign the attraction to the map using its ID as the key
    }

    return attractions_map;
}

/**
 * Convert a vector of hotels into a map with hotel IDs as keys.
 *
 * @param hotels_list A vector containing hotels.
 *
 * @return A map with hotel IDs as keys and hotels as values.
 * 
 */
static std::map<int, hotel> hotelVectorToMapById(std::vector<hotel>& hotels_list) {
    std::map<int, hotel> hotels_map; // Create a map to store hotels

    // Iterate over each hotel in the vector
    for (hotel& hotel_to_add : hotels_list) {
        hotels_map[hotel_to_add.ID] = hotel_to_add; // Assign the hotel to the map using its ID as the key
    }

    return hotels_map;
}

/**
 * Convert a vector of intersections into a map with intersection IDs as keys.
 *
 * @param intersections_list A vector containing intersections.
 *
 * @return A map with intersection IDs as keys and intersections as values.
 * 
 */
static std::map<int, intersection> intersectionVectorToMapById(std::vector<intersection>& intersections_list) {
    std::map<int, intersection> intersections_map; // Create a map to store intersections

    // Iterate over each intersection in the vector
    for (intersection& inter : intersections_list) {
        intersections_map[inter.ID] = inter; // Assign the intersection to the map using its ID as the key
    }

    return intersections_map;
}


/**
 * Adds information on directly connected intersections to the attractions dataset.
 *
 * @param intersections_map The map containing intersections.
 * @param attraction_data The map containing attractions.
 * 
 */
static void addIntersectionDatatoAttraction(std::map<int, intersection>& intersections_map, std::map<int, attraction>& attraction_data) {
    // Iterate over each intersection in the intersections map
    for (auto& intersection_pair : intersections_map) {
        intersection& inter = intersection_pair.second; // Get the intersection (from an element on the map intersections_map)

        // Iterate over each attraction linked to the intersection
        for (int attraction_id : inter.attraction_linked) {

            // Find the attraction in the attractions map
            auto attr = attraction_data.find(attraction_id);

            // If the attraction is found, add the intersection ID to its list of linked intersections
            if (attr != attraction_data.end()) {
                attr->second.intersection_linked.push_back(inter.ID);
            }
        }
    }
}

/**
 * Adds information on directly connected intersections to hotels dataset.
 *
 * @param intersections_map The map containing intersections.
 * @param hotel_data The map containing hotels.
 * 
 */
static void addIntersectionDatatoHotel(std::map<int, intersection>& intersections_map, std::map<int, hotel>& hotel_data) {
    // Iterate over each intersection in the intersections map
    for (auto& intersection_pair : intersections_map) {

        intersection& inter = intersection_pair.second; // Get the intersection (from an element on the map intersections_map)

        // Iterate over each hotel linked to the intersection
        for (int hotel_id : inter.hotel_linked) {

            // Find the hotel in the hotel data map
            auto hotel_used = hotel_data.find(hotel_id);

            // If the hotel is found, add the intersection ID to its list of linked intersections
            if (hotel_used != hotel_data.end()) {
                hotel_used->second.intersection_linked.push_back(inter.ID);
            }
        }
    }
}

/**
 * Extracts the waiting times for each attraction in the specified CSV file.
 *
 * @param data_URL The access path to the database in CSV format.
 *
 * @return A map with the time of day as the key and another map with the attraction ID as the key and the waiting time as the content.
 * 
 */
static std::map<int, std::map<int, int>> attractionsOpenCSV(std::string& data_URL) {
    std::map<int, std::map<int, int>> data_map; // Map to store the extracted data

    std::ifstream fichier(data_URL); // Open the CSV file

    if (!fichier) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier." << std::endl;
        return data_map; // Return empty map if unable to open the file
    }

    std::string ligne;
    bool premiere_ligne = true; // Flag to skip the first line (headers)

    // Read each line of the CSV file
    while (std::getline(fichier, ligne)) {
        if (premiere_ligne) {
            premiere_ligne = false;
            continue; // Skip the first line (it containes the name of catergory)
        }

        std::istringstream stream(ligne); // Create a string stream to parse each line
        std::string cellule;
        int id;
        int colonne = 0; // Column index
         
        // Process each cell in the line
        while (std::getline(stream, cellule, ';')) {
            colonne++;

            if (colonne == 1) {
                id = std::stoi(cellule); // Extract attraction ID
                data_map[id] = std::map<int, int>{}; // Initialize map for the attraction ID
            }
            else if (colonne >= 3) { // Starting from the third column, which contains waiting times
                int heure = (colonne - 2) % 24; // Calculate the hour (time of day)
                int valeur;
                if (!cellule.empty()) {
                    valeur = std::stoi(cellule); // Convert cell value to integer (waiting time)
                }
                else {
                    valeur = 60; // Default waiting time (if cell is empty)
                }
                data_map[id][heure] = valeur; // Store waiting time in the nested map
            }
        }
    }

    fichier.close(); // Close the file

    return data_map;
}

/**
 * Adds information on waiting times to the attractions in the dataset.
 *
 * @param attractions The attraction data map.
 * @param wait_time_data The wait time data map (from attractionsOpenCSV function).
 * 
 */
static void addWaitTime(std::map<int, attraction>& attractions, std::map<int, std::map<int, int>>& wait_time_data) {
    // Iterate over each entry in the wait time data map
    for (auto& entry : wait_time_data) {
        int attraction_id = entry.first; // Extract the attraction ID from the map

        // Check if the attraction ID exists in the attraction data map
        auto attr = attractions.find(attraction_id);
        if (attr != attractions.end()) {
            // If found, update the wait time information for the attraction
            attr->second.wait_time = entry.second; 
        }
        else {
            // If not found, display an error message
            std::cerr << "Erreur : Aucune attraction correspondante trouvée pour l'ID : " << attraction_id << std::endl;
        }
    }

}

/**
 * Augments the attraction data with waiting time information for single rider lines.
 *
 * @param attraction_data The original attraction data.
 * @return A new map containing the updated attraction data.
 * 
 */
static std::map<int, attraction> addSingleRiderData(std::map<int, attraction>& attraction_data) {
    std::map<int, attraction> new_attraction_data;
    
    // Iterate over each attraction in the original data
    for (auto it = attraction_data.begin(); it != attraction_data.end(); ++it) {
        int id = it->first;// Extract the attraction ID

        // Check if the attraction ID is in the single rider list
        auto single_rider_it = std::find(current_setting.single_rider_list.begin(), current_setting.single_rider_list.end(), id);

        // If the attraction is **not** in the single rider list
        if (single_rider_it == current_setting.single_rider_list.end()) {
            attraction& attr = it->second; // Get the attraction object

            // Fetch the wait time data for the attraction's single rider line and assign it to the attraction
            attr.wait_time_sinlge_rider = attraction_data[attr.single_rider].wait_time; 

            new_attraction_data[attr.ID] = attr; // Add the updated attraction to the new attraction data map
        }
    }

    return new_attraction_data; // Return the updated attraction data map
}

////////////////////////
//-returned-functions-//
////////////////////////

std::string DATA_LINK_JSON = "data/data.json";
std::string DATA_LINK_CSV = "data/waiting_time.csv";

/**
 * The grouping of all the functions used to obtain the attractions map.
 *
 * @param link_to_data The link to the data file in JSON format.
 * 
 * @return The attractions map.
 * 
 */
std::map <int, attraction>getAttractionData(std::string& link_to_data) {
    
    // Open JSON file and extract attractions
    std::vector<attraction> attractions_vector = attractionsOpenJson(link_to_data);

    // Convert attractions vector to map indexed by ID
    std::map <int, attraction> attraction_data = attractionVectorToMapById(attractions_vector);

    // Open JSON file, extract intersections, and associate them with attractions
    std::vector<intersection> intersections_vector = intersectionOpenJson(link_to_data, attraction_data);
    std::map <int, intersection> intersection_data = intersectionVectorToMapById(intersections_vector);
    addIntersectionDatatoAttraction(intersection_data, attraction_data);

    // Open CSV file and extract waiting times for attractions
    std::map<int, std::map<int, int>> wait_time_data = attractionsOpenCSV(DATA_LINK_CSV);
    addWaitTime(attraction_data, wait_time_data);
    
    // Add single rider data to the attraction data
    attraction_data = addSingleRiderData(attraction_data);

    return attraction_data;
}

/**
 * The grouping of all the functions used to obtain the hotels map.
 *
 * @param link_to_data The link to the data file in JSON format.
 * 
 * @return The hotels map.
 * 
 */
std::map <int, hotel> getHotelData(std::string& link_to_data) {

    // Open JSON file and extract hotels
    std::vector<hotel> hotels_vector = hotelOpenJson(link_to_data);

    // Convert hotels vector to map indexed by ID
    std::map <int, hotel> hotel_data = hotelVectorToMapById(hotels_vector);

    // Obtain attraction data to associate with hotels
    std::map <int, attraction> attraction_data = getAttractionData(link_to_data);

    // Extract intersection data and associate with hotels
    std::vector<intersection> intersections_vector = intersectionOpenJson(link_to_data, attraction_data);
    std::map <int, intersection> intersection_data = intersectionVectorToMapById(intersections_vector);
    addIntersectionDatatoHotel(intersection_data, hotel_data);

    return hotel_data;
}

/**
 * The grouping of all the functions used to obtain the intersections map.
 *
 * @param link_to_data The link to the data file in JSON format.
 * @param current_setting_too_use The current settings to use.
 * 
 * @return The intersections map.
 * 
 */
std::map <int, intersection> getIntersectionData(std::string& link_to_data, setting& current_setting_too_use) {
    // Obtain attraction data to associate with intersections
    std::map <int, attraction> attraction_data = getAttractionData(link_to_data);

    // Extract intersection data and associate with attractions
    std::vector<intersection> intersections_vector = intersectionOpenJson(link_to_data, attraction_data);
    std::map <int, intersection> intersection_data = intersectionVectorToMapById(intersections_vector);

    // Obtain hotel data to add a special hotel intersection
    std::map  <int, hotel> hotel_data = getHotelData(link_to_data);

    // Create a special intersection representing the hotel
    intersection hotel_intersection;
    hotel hotel_to_add = hotel_data[current_setting_too_use.hotel_ID];
    hotel_intersection.ID = 0;
    hotel_intersection.name = hotel_to_add.name;
    hotel_intersection.intersection_linked = hotel_to_add.intersection_linked;
    hotel_intersection.location = hotel_to_add.location;

    // Add the fake intersection to the intersection data at ID 0
    intersection_data[0] = hotel_intersection;

    return intersection_data;
}
