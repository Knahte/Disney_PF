////////////////////////
//-include-//--using--//
////////////////////////

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <unordered_map>
#include <fstream>
#include "Include/custom/import_data.h"

using matrix_2d = std::map<int, std::map<int, int>>;
using matrix_3d = std::map<int, matrix_2d>;



////////////////////////
//-------struct-------//
////////////////////////

struct setting {
    bool single_rider = true;
    std::pair<int, int> available_time = { 8,23 }; //(min: 8 | max:23)
    int available_days = 3;
    int hotel_ID = 0; //see Json file "data.json"
    int walking_speed = 8; //in km/h - (min: 2 | max: 8)
};



////////////////////////
//--global--variable--//
////////////////////////

int nb_gene = 300;
double selectivity = 0.1;
int nb_generation = 0;

std::map<int, attraction> attraction_data = getAttractionData();
std::map<int, hotel> hotel_data = getHotelData();
std::map<int, intersection> intersection_data = getIntersectionData();

setting current_setting; //temporary 



////////////////////////
//--debug--functions--//
////////////////////////

/**
 * Display the used vector on the console
 *
 * @param A vector with int in it
 *
 */
void vectorDebug(std::vector<int>& vector_data) {
    std::cout << '{';
    if (not vector_data.empty()) {
        for (size_t i = 0; i < vector_data.size() - 1; ++i) {
            std::cout << vector_data[i] << " ; ";
        }
        std::cout << vector_data.back();
    }
    std::cout << '}' << std::endl;
}

/**
 * Display the used waiting time data for a specific attraction on the console
 *
 * @param A map with time data 
 * @param (Note: works with all std::map<int, int> types)
 *
 */
void timeDataDebug(std::map<int, int>& time_data) {
    std::cout << "{";
    for (auto& entry : time_data) {
        std::cout << entry.first << ":" << entry.second << " | ";
    }
    std::cout << "}" << std::endl;
}

/**
 * Display an attraction and its properties on the console
 *
 * @param An attraction
 *
 */
void attractionDebug(attraction& attr) {
    std::cout << "Attraction ID: " << attr.ID << std::endl;
    std::cout << "Name: " << attr.name << std::endl;
    std::cout << "Location (lat, lon): " << attr.location.lat << ", " << attr.location.lon << std::endl;
    std::cout << "Wait Time: ";
    timeDataDebug(attr.wait_time);
    std::cout << "Single Rider ID : " << attr.single_rider << std::endl;
    std::cout << "Connected with : ";
    vectorDebug(attr.intersection_linked);
    std::cout << "Visited: " << std::boolalpha << attr.visited << std::endl << std::endl;
}

/**
 * Display a hotel and its properties on the console
 *
 * @param A hotel
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
 * Display an intersection and its properties on the console
 *
 * @param An intersection
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
 * Display all attractions and their properties on the console
 *
 * @param The map of attractions
 *
 */
void attractionsDebug(std::map<int, attraction>& attractions_data) {
    for (auto& pair : attractions_data) {
        attraction& attr = pair.second;
        attractionDebug(attr);
    }
}

/**
 * Display all hotels and their properties on the console
 *
 * @param The map of hotels
 *
 */
void hotelsDebug(std::map<int, hotel>& hotels_data) {
    for (auto& pair : hotels_data) {
        hotel& hotel_to_print = pair.second;
        hotelDebug(hotel_to_print);
    }
}

/**
 * Display all intersections and their properties on the console
 *
 * @param The map of intersections
 *
 */
void intersectionsDebug(std::map<int, intersection>& intersections_data) {
    for (auto& pair : intersections_data) {
        intersection& inter_to_print = pair.second;
        intersectionDebug(inter_to_print);
    }
}

/**
 * Display all the steps in the path used in the console
 *
 * @param A path
 *
 */
void pathDebug(std::vector<int>& path) {
    int current_time = current_setting.available_time.first * 60;
    int distance = 0;
    int distance_traveled = 0;
    std::cout << "Attractions in the path:" << std::endl;
    for (size_t i = 1; i < path.size(); ++i) {
        int current_id = path[i - 1];
        auto current_attraction = attraction_data.find(current_id);

        std::cout << "Attraction ID: " << current_attraction->first << ", Name: " << current_attraction->second.name << std::endl;
    }
    std::cout << std::endl;
}

/**
 * Display the used 2D matrix on the console
 *
 * @param A 2D matrix with int in it
 *
 */
void debug2dMatrix(const matrix_2d & matrix) {
    for (const auto& row : matrix) {
        std::cout << "Attraction ID: " << row.first << std::endl;
        for (const auto& col : row.second) {
            std::cout << std::setw(10) << col.second << " ";
        }
        std::cout << std::endl;
    }
}

/**
 * Display the used 3D matrix on the console
 *
 * @param A 3D matrix with int in it
 *
 */
void debug3dMatrix(const matrix_3d& matrix) {
    for (const auto& layer : matrix) {
        std::cout << std::endl << std::endl << "Layer ID: " << layer.first << std::endl;
        for (const auto& row : layer.second) {
            std::cout << "Attraction ID: " << row.first << std::endl;
            for (const auto& col : row.second) {
                std::cout << "Time ID: " << col.first << " - Distance: " << std::setw(10) << col.second << std::endl;
            }
        }
    }
}



////////////////////////
//-------output-------//
////////////////////////

/**
 * Export the path to a GPX file
 *
 * @param A VALID path
 *
 * @return A gpx file named "path.gpx" in the "output" folder
 * @return WARNING: OVERWRITES THE FILE EACH TIME IT GENERATES A NEW FILE
 * 
 */
void pathToGPX(const std::vector<int>& path) {
    std::ofstream outputFile("Output/path.gpx");

    if (!outputFile.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier GPX." << std::endl;
        return;
    }

    outputFile << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
    outputFile << "<gpx version=\"1.1\" xmlns=\"http://www.topografix.com/GPX/1/1\">" << std::endl;
    outputFile << "<trk>" << std::endl;
    outputFile << "<trkseg>" << std::endl;

    for (size_t i = 1; i < path.size(); ++i) {
        int current_id = path[i - 1];
        auto current_attraction = attraction_data.find(current_id);

        if (current_attraction != attraction_data.end()) {
            outputFile << "<trkpt lat=\"" << current_attraction->second.location.lat << "\" lon=\"" << current_attraction->second.location.lon << "\">" << std::endl;
            outputFile << "<name>" << current_attraction->second.name << "</name>" << std::endl;
            outputFile << "</trkpt>" << std::endl;
        }
    }

    outputFile << "</trkseg>" << std::endl;
    outputFile << "</trk>" << std::endl;
    outputFile << "</gpx>" << std::endl;

    outputFile.close();
}



////////////////////////
//-distance-functions-//
////////////////////////

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
    auto a_item = a_data.find(a_ID);
    auto b_item = b_data.find(b_ID);

    if (a_item == a_data.end() || b_item == b_data.end()) {
        return -1; 
        
    }
    double a_lon = (a_item->second.location.lon) * 0.0174533; // angle[deg] * 0,0174533 = angle[rad] 
    double b_lon = (b_item->second.location.lon) * 0.0174533;
    double a_lat = (a_item->second.location.lat) * 0.0174533;
    double b_lat = (b_item->second.location.lat) * 0.0174533;


    double distance = acos(sin(a_lat) * sin(b_lat) + cos(a_lat) * cos(b_lat) * cos(b_lon - a_lon)) * 6371000; //6371000 = Radius of the Earth (conventional)
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
    std::map<int, bool> visited;
    std::map<int, double> distance;

    for (auto& item_inter : intersection_data) {
        int id = item_inter.first;
        distance[id] = std::numeric_limits<double>::infinity();
        visited[id] = false;
    }
    distance[start_intersection.ID] = 0;

    while (true) {
        int position_index = -1;
        double min_distance = std::numeric_limits<double>::infinity();

        for (auto& item_distance : distance) {
            int id = item_distance.first;
            double dist = item_distance.second;
            
            if (!visited[id] && dist < min_distance) {
                position_index = id;
                min_distance = dist;
                
            }
        }


        if (position_index == end_intersection.ID) {
            visited[position_index] = true;
            break;
        }

        if (position_index == -1)
            break;

        visited[position_index] = true;

        for (int next_position_index : intersection_data.at(position_index).intersection_linked) {
            double distance_to_add = getColseDistance(position_index, next_position_index, intersection_data, intersection_data);
            if (!visited[next_position_index] && distance[next_position_index] > distance[position_index] + distance_to_add) {
                distance[next_position_index] = distance[position_index] + distance_to_add;
            }
        }
    }


    if (visited[end_intersection.ID]) {
        //std::cout << "|||| Distance from " << start_intersection.ID << " to " << end_intersection.ID << " = " << distance[end_intersection.ID] << std::endl;
        return distance[end_intersection.ID];
    }
    else {
        std::cerr << "Erreur : Aucun chemin trouvé de " << start_intersection.ID << " à " << end_intersection.ID << std::endl;
        return -1;
    }
}

/**
 * Calculates the distance between two attractions considering the walking distance and waiting times.
 *
 * @param start_ID The ID of the starting attraction.
 * @param end_ID The ID of the destination attraction.
 * @param current_time The current time.
 *
 * @return The total distance between the two attractions.
 * @return -1 if either of the attractions is not accessible or if an error occurs during calculation.
 */
double getLongDistance(int& start_ID, int& end_ID, int& current_time){
    double min_distance = std::numeric_limits<double>::infinity();
    int min_time = std::numeric_limits<int>::max();
    double max_time = current_setting.available_time.first * 60;

    auto start_attraction = attraction_data.find(start_ID);
    auto end_attraction = attraction_data.find(end_ID);

    int time_taken = 0;

    if(!(start_attraction->second.intersection_linked.empty() || end_attraction->second.intersection_linked.empty()))
    {
        for (int start_intersection_id : start_attraction->second.intersection_linked) {

            for (int end_intersection_id : end_attraction->second.intersection_linked) {
                double distance = findShortestPath(intersection_data.at(start_intersection_id), intersection_data.at(end_intersection_id)) + getColseDistance(start_ID, start_intersection_id, attraction_data, intersection_data) + getColseDistance(end_ID, end_intersection_id, attraction_data, intersection_data);
                time_taken = distance / current_setting.walking_speed * 60 / 1000  + (attraction_data[end_intersection_id].wait_time[(current_time * 60) % 24]);
                if (distance <= -1) {
                    std::cerr << "(var)'distance' in getLongDistance return -1";
                    return -1;
                }
                else if (time_taken < min_time) {
                    min_distance = distance;
                    min_time = time_taken;
                }

            }
        }
    }
    else {
        std::cerr << "no attraction accessible in " << start_ID << " or " << end_ID << " attraction" << std::endl;
        attractionDebug(attraction_data[start_ID]);
        attractionDebug(attraction_data[end_ID]);
        return -1;
    }
    current_time += min_time;

    return min_distance;
}

/**
 * Generates a 3D matrix containing distances between attractions for different times.
 *
 * @param id_list A list of attraction IDs for which distances need to be calculated.
 *
 * @return A 3D matrix containing distances between attractions at different times.
 */
matrix_3d getMatrix(std::vector<int>& id_list) {
    matrix_3d distance_matrix;
    for (int current_time = 8; current_time <= 23; ++current_time) {
        std::map<int, std::map<int, int>> distance_matrix_at_current_time = {};
        for (int attraction_id1 : id_list) {
            attraction& attraction1 = attraction_data[attraction_id1];
            distance_matrix_at_current_time[attraction_id1] = std::map<int, int>();
            for (int attraction_id2 : id_list) {
                attraction& attraction2 = attraction_data[attraction_id2];
                double distance = getLongDistance(attraction_id1, attraction_id2, current_time);
                distance_matrix_at_current_time[attraction_id1][attraction_id2] = distance;
            }
        }
        distance_matrix[current_time] = distance_matrix_at_current_time;

    }
    return distance_matrix;
}



////////////////////////
//simulation-functions//
////////////////////////

/**
 * Generates a random permutation of the elements in the input vector using the Fisher-Yates shuffle.
 *
 * @param list The input vector containing elements to be shuffled.
 *
 * @return A randomly shuffled vector.
 * 
 */
std::vector<int> generateRandomVectorWithList(std::vector<int> list) {
    int n = list.size();
    for (int i = 0; i < n - 1; ++i) {
        int j = i + rand() % (n - i);
        int tmp = list[i];
        list[i] = list[j];
        list[j] = tmp;
    }

    return list;
}

/**
 * Simulates the traversal of a path and calculates the total time taken.
 *
 * @param path The path to be traversed.
 * @param graph_matrix The 3D matrix representing distances between intersections at different times.
 *
 * @return The total time taken to traverse the path, in minutes.
 * @return -1 if the calculation of the path is interrupted.
 * 
 */
double simulation(std::vector<int>& path, matrix_3d& graph_matrix){
    int current_time = current_setting.available_time.first * 60;
    int max_time = current_setting.available_time.second * 60;
    
    
    for (int i = 0; i < path.size() - 1; ++i) {
        if (!(current_time < max_time)) {
            std::cout << "elapsed time" << std::endl;
        }
        int current_intersection_ID = path[i];
        int next_intersection_ID = path[i + 1];

        int distance = graph_matrix[current_time][current_intersection_ID][next_intersection_ID];
        int time_taken = distance / current_setting.walking_speed * 60 / 1000 + (attraction_data[next_intersection_ID].wait_time[(current_time * 60) % 24]);
        if (distance == -1) {
            std::cerr << "Calcule du chemin interompu de " << current_intersection_ID << " à " << next_intersection_ID << std::endl;
            current_time = -1;
            break;
        }
        //std::cout << "dist = " << distance << std::endl;
        current_time += time_taken;
        //std::cout << "dist total = " << total_distance << std::endl;
    }
    current_time += getColseDistance(current_setting.hotel_ID, path.front(), hotel_data, attraction_data) / current_setting.walking_speed * 60 / 1000;
    current_time += getColseDistance(current_setting.hotel_ID, path.back(), hotel_data, attraction_data) / current_setting.walking_speed * 60 / 1000;
    return current_time;
}

/**
 * Performs crossover between two parent genes to generate a child gene.
 *
 * @param gene1 The first parent gene.
 * @param gene2 The second parent gene.
 *
 * @return The child gene resulting from the crossover operation.
 */
std::vector<int> mixingGene(std::vector<int> gene1, std::vector<int> gene2) {
    int size = gene1.size();
    std::vector<int> child(size, -1);
    int point1 = rand() % size;
    int point2 = rand() % size;
    if (point1 > point2) {
        std::swap(point1, point2);
    }
    for (int i = point1; i <= point2; ++i) {
        child[i] = gene1[i];
    }
    std::unordered_map<int, int> gene1Map;
    for (int i = point1; i <= point2; ++i) {
        gene1Map[gene1[i]] = i;
    }

    for (int i = 0; i < size; ++i) {
        if (i >= point1 && i <= point2) {
            continue;
        }
        int currentGene = gene2[i];
        while (true) {
            if (gene1Map.find(currentGene) == gene1Map.end()) {
                if (child[i] == -1) {
                    child[i] = currentGene;
                    break;
                }
                else {
                    currentGene = gene2[gene1Map[child[i]]];
                }
            }
            else {
                currentGene = gene2[gene1Map[currentGene]];
            }
        }
    }

    return child;
}

/**
 * Regenerates a new set of paths based on the previous paths and their scores.
 *
 * @param path_data The previous paths and their corresponding scores.
 * @param graph_matrix The matrix containing distance information.
 *
 * @return The regenerated set of paths with new scores.
 * 
 */
std::vector<std::pair<int, std::vector<int>>> regenerationPath(std::vector<std::pair<int, std::vector<int>>> path_data, matrix_3d& graph_matrix) {
    
    for (size_t i = 0; i < path_data.size() - 1; ++i) {
        for (size_t j = 0; j < path_data.size() - i - 1; ++j) {
            if (path_data[j].first > path_data[j + 1].first) {
                std::pair<int, std::vector<int>> tmp = path_data[j];
                path_data[j] = path_data[j + 1];
                path_data[j + 1] = tmp;
            }
        }
    }
    std::vector<std::pair<int, std::vector<int>>> parents_gene = {};
    for (int i = 0; i < (path_data.size() * selectivity); ++i) {
        parents_gene.push_back(path_data[i]);
    }

    std::vector<std::pair<int, std::vector<int>>> childs_gene = {};
    for (int i = 0; i < nb_gene; ++i) {
        std::vector<int> mixed_gene = {};
        int index_parent1 = rand() % parents_gene.size();
        int index_parent2 = rand() % parents_gene.size();
        mixed_gene = mixingGene(parents_gene[index_parent1].second, parents_gene[index_parent2].second);
        double score = simulation(mixed_gene, graph_matrix);
        childs_gene.push_back({ score, mixed_gene });
    }

    return childs_gene;

}

/**
 * Generate a path through attractions using a genetic algorithm.
 *
 * @return The shortest path found by the genetic algorithm.
 */
std::vector<int> generatePath(){
    std::vector<int> path = {};
    std::vector<int> id_list = {};
    if (current_setting.single_rider) {
        for (auto& pair : attraction_data) {
            attraction& attr = pair.second;
            if (attr.single_rider == attr.ID) {
                id_list.push_back(attr.single_rider);
            }
        }
    }
    else {
        for (auto& pair : attraction_data) {
            attraction& attr = pair.second;
            id_list.push_back(attr.ID);
        }
    }

    std::vector<std::pair<int, std::vector<int>>> path_data = {};
    matrix_3d graph_matrix = getMatrix(id_list);

    for (int i = 0; i < nb_gene; i++) {
        std::vector<int> randomised_path = generateRandomVectorWithList(id_list);
        double score = simulation(randomised_path, graph_matrix);
        path_data.push_back({score, randomised_path});
    }

    int min_score = std::numeric_limits<int>::max();
    std::vector<int> shortest_path;

    for (auto& entry : path_data) {
        int score = entry.first;
        std::vector<int>& path = entry.second;

        if (score < min_score) {
            min_score = score;
            shortest_path = path;
        }
    }

    std::vector<bool> completion_rate = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };

    std::vector<std::pair<int, std::vector<int>>> new_path_data = regenerationPath(path_data, graph_matrix);
    for (int i = 0; i < (nb_generation - 1); i++) {
        new_path_data = regenerationPath(new_path_data, graph_matrix);
        for (int j = 0; j < 20; j++) {
            if (i  > (nb_generation - 1) * 0.05 * j && !completion_rate[j]) {
                std::cout << 5 * j << "%" << std::endl;
                completion_rate[j] = true;
            }
        }
    }
    std::cout << "100%" << std::endl;

    int new_min_score = std::numeric_limits<int>::max();
    std::vector<int> new_shortest_path;

    for (auto& entry : new_path_data) {
        int new_score = entry.first;
        std::vector<int>& path = entry.second;

        if (new_score < new_min_score) {
            new_min_score = new_score;
            new_shortest_path = path;
        }
    }

    std::cout << "min time for this path before genetic :" << min_score / 60.0 << "h and distance : " << min_score / 60 * current_setting.walking_speed << "km" << std::endl;
    std::cout << "min time for this path after  genetic :" << new_min_score / 60.0 << "h and distance : " << new_min_score / 60 * current_setting.walking_speed << "km" << std::endl;
    return new_shortest_path;
}



////////////////////////
//--------main--------//
////////////////////////


/**
 * Well... This is the main function...
 *
 * @return 0
 */
int main() {
    srand(time(NULL));

    std::vector<int> path_generated = generatePath();

    pathDebug(path_generated);

    pathToGPX(path_generated);

    return 0;
}