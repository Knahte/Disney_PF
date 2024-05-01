////////////////////////
//-include-//--using--//
////////////////////////

#include "Generating_path.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <fstream>
#include <algorithm>

using matrix_2d = std::map<int, std::map<int, double>>;



////////////////////////
//--global--variable--//
////////////////////////

std::map<int, attraction> attraction_data; // A map providing access to all attractions using their IDs as keys.
std::map<int, hotel> hotel_data; // A map providing access to all hotels using their IDs as key
std::map<int, intersection> intersection_data; // A map providing access to all intersections using their IDs as keys.



////////////////////////
//---set-up--output---//
////////////////////////

std::ofstream outputFile; // Output file stream object

/**
 * Redirects the standard output to a file.
 *
 * @param filename The name of the file to redirect output to.
 * 
 */
void redirectOutputToFile(const std::string& filename) {
    outputFile.open(filename); // Opens the specified file
    if (outputFile.is_open()) { // Checks if the file is opened successfully
        std::cout.rdbuf(outputFile.rdbuf()); // Redirects standard output to the file
    }
    else {
        std::cout << "Erreur : Impossible d'ouvrir le fichier de sortie." << std::endl;
        // Prints an error message if the file cannot be opened
    }
}

/**
 * Restores standard output from the file.
 * 
 */
void restoreOutput() {
    outputFile.close(); // Closes the output file
}



////////////////////////
//--debug--functions--//
////////////////////////

/**
 * Debug function to print integer elements stored in a vector.
 *
 * @param vector_data The vector containing integer elements to debug.
 *
 */
void vectorDebug(std::vector<int>& vector_data) {
    std::cout << '{';
    if (not vector_data.empty()) { // Check if the vector is not empty
        for (size_t i = 0; i < vector_data.size() - 1; ++i) { // Loop through vector elements except the last one
            std::cout << vector_data[i] << " ; ";
        }
        std::cout << vector_data.back();
    }
    std::cout << '}' << std::endl;
}

/**
 * Debug function to print time data stored in a map.
 *
 * @param time_data The map containing time data to debug.
 * @param (Note: works with all std::map<int, int> types)
 *
 */
void timeDataDebug(std::map<int, int>& time_data) {
    std::cout << "{";
    for (auto& entry : time_data) { // Iterate through each key-value pair in the map
        std::cout << entry.first << ":" << entry.second << " | "; // Print the key-value pair
    }
    std::cout << "}" << std::endl;
}

/**
 * Debug function to print information about an attraction.
 *
 * @param attr The attraction object containing data to debug.
 *
 */
void attractionDebug(attraction& attr) {
    std::cout << "Attraction ID: " << attr.ID << std::endl;
    std::cout << "Name: " << attr.name << std::endl;
    std::cout << "Location (lat, lon): " << attr.location.lat << ", " << attr.location.lon << std::endl;
    std::cout << "Wait Time: ";
    timeDataDebug(attr.wait_time);
    std::cout << "Single Rider ID : " << attr.single_rider << std::endl;
    std::cout << "Single Rider Wait Time: ";
    timeDataDebug(attr.wait_time_sinlge_rider);
    std::cout << "Connected with : ";
    vectorDebug(attr.intersection_linked);
}

/**
 * Debug function to print information about a hotel.
 *
 * @param hotel_to_print The hotel object containing data to debug.
 *
 */
void hotelDebug(hotel& hotel_to_print) {
    std::cout << "Hotel ID: " << hotel_to_print.ID << std::endl;
    std::cout << "Name: " << hotel_to_print.name << std::endl;
    std::cout << "Location (lat, lon): " << hotel_to_print.location.lat << ", " << hotel_to_print.location.lon << std::endl;
    std::cout << "Connected with : ";
    vectorDebug(hotel_to_print.intersection_linked);
    std::cout << std::endl;
}

/**
 * Debug function to print information about an intersection.
 *
 * @param inter The intersection object containing data to debug.
 *
 */
void intersectionDebug(intersection& inter) {
    std::cout << "Intersction ID: " << inter.ID << std::endl;
    std::cout << "Name: " << inter.name << std::endl;
    std::cout << "Location (lat, lon): " << inter.location.lat << ", " << inter.location.lon << std::endl;
    std::cout << "Linked to attraction: ";
    vectorDebug(inter.attraction_linked);
    std::cout << "Linked to hotel: ";
    vectorDebug(inter.hotel_linked);
    std::cout << "Linked to intersection: ";
    vectorDebug(inter.intersection_linked);
    std::cout << std::endl;
}

/**
 * Debug function to print information about attractions.
 *
 * @param attractions_data The map containing attraction data.
 *
 */
void attractionsDebug(std::map<int, attraction>& attractions_data) {
    for (auto& pair : attractions_data) {
        attraction& attr = pair.second;
        attractionDebug(attr);
    }
}

/**
 * Debug function to print information about hotels.
 *
 * @param hotels_data The map containing hotel data.
 *
 */
void hotelsDebug(std::map<int, hotel>& hotels_data) {
    for (auto& pair : hotels_data) {
        hotel& hotel_to_print = pair.second;
        hotelDebug(hotel_to_print);
    }
}

/**
 * Debug function to print information about intersections.
 *
 * @param intersections_data The map containing intersection data.
 *
 */
void intersectionsDebug(std::map<int, intersection>& intersections_data) {
    for (auto& pair : intersections_data) {
        intersection& inter_to_print = pair.second;
        intersectionDebug(inter_to_print);
    }
}

/**
 * Debug function to print a 2D matrix containing distance information.
 *
 * @param matrix The 2D matrix to be debugged.
 *
 */
void Matrix2dDebug(const matrix_2d& matrix) {
    for (const auto& row : matrix) {
        std::cout << "Attraction ID: " << row.first << std::endl;
        for (const auto& col : row.second) {
            std::cout << "--- Time ID: " << col.first << " - Distance: " << std::setw(10) << col.second << std::endl;
        }
    }
}

/**
 * Debug function to print information about paths including their scores and sequences of attractions.
 *
 * @param path_data A vector containing pairs of scores and paths to debug.
 *
 */
void pathsDebug(const std::vector<std::pair<double, std::vector<int>>>& path_data) {
    std::vector<std::pair<double, std::vector<int>>> sorted_paths = path_data;

    // Sort the paths based on their scores in ascending order
    std::sort(sorted_paths.begin(), sorted_paths.end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.first < rhs.first; // Sort based on the first element of the pair (score)
        }); 


    for (const auto& entry : sorted_paths) { // Iterate through the sorted paths and print information about each path
        std::cout << "Score: " << entry.first << ", Path: ";
        const std::vector<int>& path = entry.second; // Get the path sequence from the pair
        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << path[i];
            if (i != path.size() - 1) {
                std::cout << "->";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}



////////////////////////
//-------output-------//
////////////////////////

/**
 * Converts a given path to a GPX (GPS Exchange Format) file.
 *
 * @param path The path to be converted to GPX format.
 * @param name The file name (without the extension).
 * @param current_setting The settings object containing information about the current configuration.
 *
 * @return A GPX file in the "Output" folder.
 *
 */
void pathToGPX(const std::vector<int>& path, const std::string& name, setting& current_setting) {
    // Open the output file for writing
    std::ofstream outputFile("Output/" + name + ".gpx");

    // Retrieve information about the current hotel
    hotel current_hotel = hotel_data[current_setting.hotel_ID];

    if (!outputFile.is_open()) { // Check if the output file is successfully opened
        std::cout << "Erreur : Impossible d'ouvrir le fichier GPX." << std::endl;
        return;
    }

    // Write the XML declaration and GPX opening tags
    outputFile << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
    outputFile << "<gpx version=\"1.1\" xmlns=\"http://www.topografix.com/GPX/1/1\">" << std::endl;
    outputFile << "<trk>" << std::endl;
    outputFile << "<trkseg>" << std::endl;

    // Write the starting point (current hotel) to the GPX file
    outputFile << "<trkpt lat=\"" << current_hotel.location.lat << "\" lon=\"" << current_hotel.location.lon << "\">" << std::endl;
    outputFile << "<name>" << current_hotel.name << "</name>" << std::endl;
    outputFile << "</trkpt>" << std::endl;

    // Write each attraction in the path to the GPX file
    for (size_t i = 1; i <= path.size(); ++i) {
        int current_id = path[i - 1];
        auto current_attraction = attraction_data.find(current_id);

        // Check if the attraction exists in the data
        if (current_attraction != attraction_data.end()) {
            // Write the attraction's location and name to the GPX file
            outputFile << "<trkpt lat=\"" << current_attraction->second.location.lat << "\" lon=\"" << current_attraction->second.location.lon << "\">" << std::endl;
            outputFile << "<name>" << current_attraction->second.name << "</name>" << std::endl;
            outputFile << "</trkpt>" << std::endl;
        }
    }

    // Write the ending point (current hotel) to the GPX file
    outputFile << "<trkpt lat=\"" << current_hotel.location.lat << "\" lon=\"" << current_hotel.location.lon << "\">" << std::endl;
    outputFile << "<name>" << current_hotel.name << "</name>" << std::endl;
    outputFile << "</trkpt>" << std::endl;

    // Write closing tags for the GPX file
    outputFile << "</trkseg>" << std::endl;
    outputFile << "</trk>" << std::endl;
    outputFile << "</gpx>" << std::endl;

    // Close the output file
    outputFile.close();
}



////////////////////////
//-distance-functions-//
////////////////////////

/**
 * Calculates the total time taken to travel to an attraction,
 * taking into account walking distance, waiting time, and current settings.
 *
 * @param distance The distance to the attraction (in meters).
 * @param current_time The current time (in hours).
 * @param attr_ID The ID of the attraction (the destination).
 * @param current_setting The settings object containing information about the current configuration.
 *
 * @return The total time taken to travel to the attraction (in hours).
 *
 */
double getTimeTaken(double distance, double current_time, int attr_ID, setting& current_setting) {
    // Calculate travel time based on walking speed and distance
    double travel_time = distance / current_setting.walking_speed / 1000;

    double waiting_time = 0;

    // Check if single rider option is enabled in settings
    if (current_setting.single_rider) {
        // If single rider option is enabled and waiting times are considered
        if (current_setting.consider_waiting_times)
            waiting_time = attraction_data[attr_ID].wait_time_sinlge_rider[(int)(current_time + travel_time) % 24] / 60.0;
    }
    else {
        // If single rider option is not enabled and waiting times are considered
        if (current_setting.consider_waiting_times)
            waiting_time = attraction_data[attr_ID].wait_time[(int)(current_time + travel_time) % 24] / 60.0;
    }

    // Calculate total time taken as sum of travel time and waiting time
    double time_taken = travel_time + waiting_time;

    return time_taken; // Return the total time taken to travel to the attraction (in hours)
}

/**
 * Calculate the closest distance between two locations given their IDs.
 *
 * @param a_ID The ID of the first location
 * @param b_ID The ID of the second location
 * @param a_data A map containing location data for the first type of location
 * @param b_data A map containing location data for the second type of location
 *
 * @tparam T1 The type of data stored in the map a_data
 * @tparam T2 The type of data stored in the map b_data
 *
 * @return The closest distance between the two locations, in meters
 * @return -1 if either of the locations cannot be found in their respective maps
 *
 */
template<typename T1, typename T2>
double getColseDistance(int& a_ID, int& b_ID, std::map<int, T1>& a_data, std::map<int, T2>& b_data) {

    // Find the locations in the respective maps
    auto a_item = a_data.find(a_ID);
    auto b_item = b_data.find(b_ID);

    // Check if either location is not found
    if (a_item == a_data.end() || b_item == b_data.end()) {
        return -1;

    }

    // Convert latitude and longitude from degrees to radians
    double a_lon = (a_item->second.location.lon) * 0.0174533; // 0.0174532925... = pi / 180
    double b_lon = (b_item->second.location.lon) * 0.0174533;
    double a_lat = (a_item->second.location.lat) * 0.0174533;
    double b_lat = (b_item->second.location.lat) * 0.0174533;

    // Calculate the distance using Haversine formula (with 6371000 = Radius of the Earth)
    double distance = acos(sin(a_lat) * sin(b_lat) + cos(a_lat) * cos(b_lat) * cos(b_lon - a_lon)) * 6371000;


    return distance;
}

/**
 * Finds the shortest path between two intersections using Dijkstra's algorithm.
 *
 * @param start_intersection The starting intersection.
 * @param end_intersection The destination intersection.
 * @param intersection_data The intersection data containing connectivity information.
 *
 * @return The shortest distance between the two intersections.
 * @return -1 if no path is found between the two intersections.
 *
 */
double findShortestPath(intersection& start_intersection, intersection& end_intersection) {
    // Initialize maps to store visited intersections and their distances from the start intersection
    std::map<int, bool> visited;
    std::map<int, double> distance;

    // Initialize distances to infinity and mark all intersections as unvisited
    for (auto& item_inter : intersection_data) {
        int id = item_inter.first;
        distance[id] = std::numeric_limits<double>::infinity();
        visited[id] = false;
    }

    // Set distance from start intersection to itself at 0m (obviously)
    distance[start_intersection.ID] = 0;

    // Iterate until all intersections are visited or no path is found
    while (true) {
        int position_index = -1; // Index of the intersection with the minimum distance
        double min_distance = std::numeric_limits<double>::infinity(); // Minimum distance found so far

        // Find the unvisited intersection with the minimum distance
        for (auto& item_distance : distance) {
            int id = item_distance.first;
            double dist = item_distance.second;

            if (!visited[id] && dist < min_distance) {
                position_index = id;
                min_distance = dist;

            }
        }

        // If the destination intersection is reached or no more unvisited intersections are left, exit the loop
        if (position_index == end_intersection.ID) {
            visited[position_index] = true; // Mark the destination intersection as visited
            break;
        }

        if (position_index == -1)
            break;

        visited[position_index] = true; // Mark the current intersection as visited

        // Update distances to neighboring intersections
        for (int next_position_index : intersection_data.at(position_index).intersection_linked) {
            double distance_to_add = getColseDistance(position_index, next_position_index, intersection_data, intersection_data);
            if (!visited[next_position_index] && distance[next_position_index] > distance[position_index] + distance_to_add) {
                distance[next_position_index] = distance[position_index] + distance_to_add;
            }
        }
    }

    // Check if the destination intersection is reachable
    if (visited[end_intersection.ID]) {
        return distance[end_intersection.ID]; // Return the shortest distance to the destination intersection
    }
    else {
        // Print an detailed error message if no path is found
        std::cout << "Erreur : Aucun chemin trouvé de " << start_intersection.ID << " à " << end_intersection.ID << std::endl;
        // Debugging information for the intersection causing problems
        intersectionDebug(start_intersection); 
        intersectionDebug(end_intersection);
        return -1; // Return -1 indicating no path found
    }
}

/**
 * Generates a 3D matrix containing distances between attractions for different times.
 *
 * @param id_list A list of attraction IDs for which distances need to be calculated.
 * @param current_setting The current settings.
 *
 * @return A 3D matrix containing distances between attractions at different times.
 *
 */
matrix_2d getMatrix(std::vector<int>& id_list, setting& current_setting) {
   
    // Create a fake attraction for the hotel to include in the distance calculation
    attraction attr_hotel;
    hotel source_hotel = hotel_data[current_setting.hotel_ID];
    attr_hotel.ID = 0;
    attr_hotel.name = source_hotel.name;
    attr_hotel.intersection_linked = source_hotel.intersection_linked;
    attr_hotel.location = source_hotel.location;
    for (auto& intersection_extracted : intersection_data) {
        intersection inter = intersection_extracted.second;
        for (int id_hotel_tested : inter.hotel_linked) {
            if (id_hotel_tested == source_hotel.ID) {
                inter.attraction_linked.push_back(0);
            }
        }
    }

    
    attraction_data[0] = attr_hotel; // Add the hotel as an attraction with ID 0

    // Initialize the distance matrix
    matrix_2d distance_matrix = {};

    // Iterate over each attraction ID in the list
    for (int attraction_id1 : id_list) {
        attraction& attraction1 = attraction_data[attraction_id1];
        distance_matrix[attraction_id1] = std::map<int, double>();

        // Calculate distances from attraction1 to all other attractions in the list
        for (int attraction_id2 : id_list) {
            attraction& attraction2 = attraction_data[attraction_id2];

            double distance = findShortestPath(intersection_data[attraction_id1 * 1000], intersection_data[attraction_id2 * 1000]);

            distance_matrix[attraction_id1][attraction_id2] = distance;
        }
    }

    return distance_matrix;
}



////////////////////////
//simulation-functions//
////////////////////////

/**
 * Debug function to print attractions along a path.
 *
 * @param path The path containing attraction IDs.
 * @param current_setting The current settings.
 *
 */
void pathInfoDisplay(std::vector<int>& path, setting& current_setting) {
    
    // Initialize the current time with the entry time (from the settings)
    double current_time = current_setting.entry_time;

    double total_distance = 0;

    // Iterate over each attraction ID in the path
    for (size_t i = 0; i < path.size() - 1; ++i) {
        int current_id = path[i];
        auto current_attraction = attraction_data.find(current_id);
        int next_id = path[i + 1];
        auto next_attraction = attraction_data.find(next_id);

        // Find the shortest path distance to the next attraction
        double distance_to_next = findShortestPath(intersection_data[current_id * 1000], intersection_data[next_id * 1000]);

        // Check if the distance calculation was interrupted
        if (distance_to_next == -1) {
            std::cout << "Calculation interrupted between Attraction ID " << current_id << " and " << next_id << std::endl;
            return; // Stop.
        }

        // Update the current time by the time taken to travel to the next attraction
        current_time += getTimeTaken(distance_to_next, current_time, next_id, current_setting);

        // Accumulate the total distance traveled
        total_distance += distance_to_next;

        // Print information about the current attraction
        std::cout << "ID : " << current_attraction->first << ", Name : " << current_attraction->second.name << std::endl;
        if (current_setting.consider_waiting_times) std::cout << "--- Time at exit : " << (int)current_time % 24 << " h " << ((current_time - (int)current_time) * 60) << " min" << std::endl; //only if the simulation prioritises time over distance
        std::cout << "--- Distance to next : " << distance_to_next << " m" << std::endl;
    }

    // Print information about the last attraction in the path
    if (path.size() != 0) {
        std::cout << "ID : " << attraction_data[path.back()].ID << ", Name : " << attraction_data[path.back()].name << std::endl;
    }

    std::cout << "Total distance traveled: " << total_distance << " meters" << std::endl;
    std::cout << std::endl;
}


/**
 * Simulates the traversal of a path and calculates the total time taken.
 *
 * @param path The path to be traversed.
 * @param graph_matrix The 3D matrix representing distances between intersections at different times.
 * @param current_setting The current settings.
 *
 * @return The total time taken to traverse the path, in **hours**.
 * @return -1 if the calculation of the path is interrupted.
 *
 */
double simulation(std::vector<int>& path, matrix_2d& graph_matrix, setting& current_setting) {
    // Initialize the current time with the entry time from the settings
    double current_time = current_setting.entry_time;

    // Iterate over each attraction in the path
    for (int i = 0; i < path.size() - 1; ++i) {

        int current_attraction_ID = path[i];
        int next_attraction_ID = path[i + 1];

        // Get the distance between the current and next attraction from the graph matrix
        double distance = graph_matrix[current_attraction_ID][next_attraction_ID];

        // Calculate the time taken to travel the distance and update the current time
        double time_taken = getTimeTaken(distance, current_time, next_attraction_ID, current_setting);

        // Check if the distance calculation was interrupted
        if (distance == -1) {
            std::cout << "Calcule du chemin interompu de " << current_attraction_ID << " à " << next_attraction_ID << std::endl;
            current_time = -1;
            break;
        }
        current_time += time_taken;
    }

    // Add the time taken to travel from the starting point to the first attraction and from the last attraction to the hotel (= attarction : id=0)
    current_time += graph_matrix[path.front()][0] / current_setting.walking_speed / 1000 * 60;
    current_time += graph_matrix[path.back()][0] / current_setting.walking_speed / 1000 * 60;

    return current_time;
}


/**
 * Randomly swaps two elements in the gene and checks if the resulting gene yields a shorter traversal time.
 *
 * @param gene The gene representing the path to be modified.
 * @param attraction_matrix The 3D matrix representing distances between attractions at different times.
 * @param current_setting The current settings.
 *
 * @return The modified gene with the best permutation found.
 *
 */
std::vector<int> changeGene(std::vector<int> gene, matrix_2d& attraction_matrix, setting& current_setting) {
    // Initialize variables to track the best permutation found
    int best_index_1 = 0;
    int best_index_2 = 0;
    double best_time = std::numeric_limits<double>::infinity();
    std::vector<int> best_gene = gene;

    // Get the number of attractions in the gene
    int number_of_attraction = gene.size();

    // Iterate for a fixed number of iterations (I found that 5 was an excellent quality/speed ratio)
    for (int i = 0; i <= 5 ; i++) {
        // Randomly select the first index to swap
        int index1 = rand() % gene.size();

        // Iterate over all possible second indices
        for (int index2 = 0; index2 <= number_of_attraction - 1; index2++) {
            // Create a copy of the gene and swap elements at index1 and index2
            std::vector<int> gene_tmp = gene;
            std::swap(gene_tmp[index1], gene_tmp[index2]);

            // Calculate the time taken to traverse the modified gene
            double time_taken = simulation(gene_tmp, attraction_matrix, current_setting);

            // Update the best time and indices if a shorter traversal time is found
            if (time_taken < best_time) {
                best_time = time_taken;
                best_index_1 = index1;
                best_index_2 = index2;
            }
        }
    };

    // Apply the best permutation found to the original gene
    std::swap(best_gene[best_index_1], best_gene[best_index_2]);

    // Return the modified gene with the best permutation
    return best_gene;
}

/**
 * Regenerates a new set of paths based on the previous paths and their scores.
 *
 * @param path_data The previous paths and their corresponding scores.
 * @param graph_matrix The matrix containing distance information.
 * @param current_setting The current settings.
 *
 * @return A pair containing the regenerated set of paths with new scores and a boolean indicating whether the regeneration can be ended.
 *
 */
std::vector<std::pair<double, std::vector<int>>> regenerationPath(std::vector<std::pair<double, std::vector<int>>> path_data, matrix_2d& graph_matrix, setting& current_setting) {
    // Initialize a vector to store the newly generated paths
    std::vector<std::pair<double, std::vector<int>>> childs_gene = {};

    // Iterate over each path in the previous paths
    for (auto& path : path_data) {
        // Generate a new gene (path) by applying fitting operations
        std::vector<int> mixed_gene = changeGene(path.second, graph_matrix, current_setting);

        // Simulate the newly generated gene to calculate its score
        double score = simulation(mixed_gene, graph_matrix, current_setting);

        // Add the newly generated gene and its score to the list of regenerated paths
        childs_gene.push_back({ score, mixed_gene });
    }

    return childs_gene;
}


/**
 * Generate a path through attractions using a genetic algorithm.
 *
 * @param current_setting The current settings.
 * @param number_of_generation The number of generations to run the genetic algorithm.
 * @param number_path The number of paths to generate initially.
 * @param the path to the data folder used
 *
 * @return The shortest path found by the genetic algorithm.
 */
std::vector<int> generatePath(setting& current_setting, int number_of_generation, int number_path, std::string path_to_data) {
    // Seed the random number generator
    srand(time(NULL));
    
    // Set the number of generations and number of paths
    int nb_generation = number_of_generation;
    int nb_gene = number_path;

    std::string data_link = current_setting.path_to_data;

    // Load attraction, hotel, and intersection data
    attraction_data = getAttractionData(data_link, current_setting);
    hotel_data = getHotelData(data_link, current_setting);
    intersection_data = getIntersectionData(data_link, current_setting);

    attractionsDebug(attraction_data);

    // Generate matrix containing distances between attractions
    std::vector<int> id_list = current_setting.ID_list;
    matrix_2d graph_attraction_matrix = getMatrix(id_list, current_setting);


    std::vector<int> path = {};
    std::vector<std::pair<double, std::vector<int>>> path_data = {};


    std::vector<int> initial_path = current_setting.ID_list;
    for (int i = 0; i < nb_gene; i++) {
        int nb_permutation = 30; // Number of permutations, can be modified without too much impact

        std::vector<int> initial_path_modified = initial_path;

        // Apply permutations to the initial path
        for(int j = 0; j < nb_permutation; j++)  {
            int index_1, index_2;
            index_1 = rand() % initial_path_modified.size();
            index_2 = rand() % initial_path_modified.size();
            std::swap(initial_path_modified[index_1], initial_path_modified[index_2]);
        }

        // Calculate the score of the modified initial path and add it to path data
        double initial_score_modified = simulation(initial_path_modified, graph_attraction_matrix, current_setting);
        path_data.push_back({ initial_score_modified, initial_path_modified });
    }

    // Find the shortest path among the initial paths
    double min_score = std::numeric_limits<double>::infinity();
    std::vector<int> shortest_path;

    for (std::pair<double, std::vector<int>>& entry : path_data) {

        double score = entry.first;
        std::vector<int>& path = entry.second;

        if (score < min_score) {
            min_score = score;
            shortest_path = path;
        }
    }

    
    // Regenerate paths over multiple generations
    std::vector<std::pair<double, std::vector<int>>> new_path_data = regenerationPath(path_data, graph_attraction_matrix, current_setting);
    for (int i = 0; i < nb_generation; i++) {
        new_path_data = regenerationPath(new_path_data, graph_attraction_matrix, current_setting);
    }

    // Find the shortest path among the regenerated paths
    double new_min_score = std::numeric_limits<double>::infinity();
    std::vector<int> new_shortest_path;

    for (auto& entry : new_path_data) {

        double new_score = entry.first;
        std::vector<int>& path = entry.second;

        if (new_score < new_min_score) {
            new_min_score = new_score;
            new_shortest_path = path;
        }
    }

    return new_shortest_path;
}