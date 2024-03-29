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
using matrix_3d = std::map<int, matrix_2d>;



////////////////////////
//--global--variable--//
////////////////////////

int nb_gene = 300;
int nb_generation = 300;

std::map<int, attraction> attraction_data;
std::map<int, hotel> hotel_data;
std::map<int, intersection> intersection_data;


////////////////////////
//---set-up--output---//
////////////////////////

std::ofstream outputFile;

void redirectOutputToFile(const std::string& filename) {
    outputFile.open(filename);
    if (outputFile.is_open()) {
        std::cout.rdbuf(outputFile.rdbuf());
    }
    else {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier de sortie." << std::endl;
    }
}

void restoreOutput() {
    outputFile.close();
}



////////////////////////
//--debug--functions--//
////////////////////////

/**
 * Debug function to print integer elements stored in a vector.
 * Debug function to print integer elements stored in a vector.
 *
 * @param vector_data The vector containing integer elements to debug.
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
 * Debug function to print time data stored in a map.
 *
 * @param time_data The map containing time data to debug.
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
    std::cout << "Connected with : ";
    vectorDebug(attr.intersection_linked);
    std::cout << "Visited: " << std::boolalpha << attr.visited << std::endl << std::endl;
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
 * Debug function to print a 3D matrix containing distance information.
 *
 * @param matrix The 3D matrix to be debugged.
 *
 */
void Matrix3dDebug(const matrix_3d& matrix) {
    for (const auto& layer : matrix) {
        std::cout << std::endl << std::endl << "Layer ID: " << layer.first << std::endl;
        for (const auto& row : layer.second) {
            std::cout << "--- Attraction ID: " << row.first << std::endl;
            for (const auto& col : row.second) {
                std::cout << "------ Time : " << col.first << " - Distance: " << col.second;
            }
        }
    }
}

void pathsDebug(const std::vector<std::pair<double, std::vector<int>>>& path_data) {
    // Copiez le vecteur path_data car nous voulons le trier sans modifier l'original
    std::vector<std::pair<double, std::vector<int>>> sorted_paths = path_data;

    // Triez les chemins en fonction de leur score
    std::sort(sorted_paths.begin(), sorted_paths.end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.first < rhs.first;
        });

    // Affichez les chemins tri�s
    for (const auto& entry : sorted_paths) {
        std::cout << "Score: " << entry.first << ", Path: ";
        const std::vector<int>& path = entry.second;
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
 * @param name The file name (without the extention)
 *
 * @return A gpx file in the "output" folder
 *
 */
void pathToGPX(const std::vector<int>& path, const std::string& name, setting& current_setting) {
    std::ofstream outputFile("Output/" + name + ".gpx");

    hotel current_hotel = hotel_data[current_setting.hotel_ID];
    if (!outputFile.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier GPX." << std::endl;
        return;
    }

    outputFile << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
    outputFile << "<gpx version=\"1.1\" xmlns=\"http://www.topografix.com/GPX/1/1\">" << std::endl;
    outputFile << "<trk>" << std::endl;
    outputFile << "<trkseg>" << std::endl;

    outputFile << "<trkpt lat=\"" << current_hotel.location.lat << "\" lon=\"" << current_hotel.location.lon << "\">" << std::endl;
    outputFile << "<name>" << current_hotel.name << "</name>" << std::endl;
    outputFile << "</trkpt>" << std::endl;

    for (size_t i = 1; i <= path.size(); ++i) {
        int current_id = path[i - 1];
        auto current_attraction = attraction_data.find(current_id);

        if (current_attraction != attraction_data.end()) {
            outputFile << "<trkpt lat=\"" << current_attraction->second.location.lat << "\" lon=\"" << current_attraction->second.location.lon << "\">" << std::endl;
            outputFile << "<name>" << current_attraction->second.name << "</name>" << std::endl;
            outputFile << "</trkpt>" << std::endl;
        }
    }

    outputFile << "<trkpt lat=\"" << current_hotel.location.lat << "\" lon=\"" << current_hotel.location.lon << "\">" << std::endl;
    outputFile << "<name>" << current_hotel.name << "</name>" << std::endl;
    outputFile << "</trkpt>" << std::endl;

    outputFile << "</trkseg>" << std::endl;
    outputFile << "</trk>" << std::endl;
    outputFile << "</gpx>" << std::endl;

    outputFile.close();
}



////////////////////////
//-distance-functions-//
////////////////////////

double getTimeTaken(double distance, double current_time, int attr_ID, setting& current_setting) {
    double travel_time = distance / current_setting.walking_speed / 1000;
    double waiting_time = 0;
    if (current_setting.consider_waiting_times)
        waiting_time = attraction_data[attr_ID].wait_time[(int)(current_time + travel_time) % 24] / 60.0;
    //std::cout <<"it's : " << current_time << "h and we are going to : " << attraction_data[attr_ID].name << "the opperation is (travel time + waiting time)" << travel_time * 60 << "  +  " << waiting_time * 60 << std::endl;
    double time_taken = travel_time + waiting_time;
    return time_taken; //time taken in h
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
        std::cerr << "Erreur : Aucun chemin trouv� de " << start_intersection.ID << " � " << end_intersection.ID << std::endl;
        intersectionDebug(start_intersection);
        intersectionDebug(end_intersection);
        return -1;
    }
}

/**
 * Generates a 3D matrix containing distances between attractions for different times.
 *
 * @param id_list A list of attraction IDs for which distances need to be calculated.
 *
 * @return A 3D matrix containing distances between attractions at different times.
 */
matrix_3d getMatrix(std::vector<int>& id_list, setting& current_setting) {
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

    attraction_data[0] = attr_hotel;

    matrix_3d distance_matrix;
    std::cout << "cr�ation de la matrice" << std::endl;
    for (int current_time = 0; current_time < 24; ++current_time) {
        matrix_2d distance_matrix_at_current_time = {};
        for (int attraction_id1 : id_list) {
            attraction& attraction1 = attraction_data[attraction_id1];
            distance_matrix_at_current_time[attraction_id1] = std::map<int, double>();
            for (int attraction_id2 : id_list) {
                attraction& attraction2 = attraction_data[attraction_id2];
                double distance = findShortestPath(intersection_data[attraction_id1 * 1000], intersection_data[attraction_id2 * 1000]);
                double time_taken = getTimeTaken(distance, current_time, attraction_id2, current_setting);
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
 * Debug function to print attractions along a path.
 *
 * @param path The path containing attraction IDs.
 *
 */
void pathInfoDisplay(std::vector<int>& path, setting& current_setting) {
    double current_time = current_setting.entry_time;
    std::cout << (double)current_time << std::endl;
    double total_distance = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        int current_id = path[i];
        auto current_attraction = attraction_data.find(current_id);
        int next_id = path[i + 1];
        auto next_attraction = attraction_data.find(next_id);

        double distance_to_next = findShortestPath(intersection_data[current_id * 1000], intersection_data[next_id * 1000]);
        current_time += getTimeTaken(distance_to_next, current_time, next_id, current_setting);

        if (distance_to_next == -1) {
            std::cerr << "Calculation interrupted between Attraction ID " << current_id << " and " << next_id << std::endl;
            return;
        }
        total_distance += distance_to_next;
        std::cout << "ID : " << current_attraction->first << ", Name : " << current_attraction->second.name << std::endl;
        if (current_setting.consider_waiting_times) std::cout << "--- Time at exit : " << (int)current_time % 24 << " h " << ((current_time - (int)current_time) * 60) << " min" << std::endl;
        std::cout << "--- Distance to next : " << distance_to_next << " m" << std::endl;
    }
    if (path.size() != 0) {
        std::cout << "ID : " << attraction_data[path.back()].ID << ", Name : " << attraction_data[path.back()].name << std::endl;

    }
    std::cout << "Total distance traveled: " << total_distance << " meters" << std::endl;
    std::cout << std::endl;
}

/**
 * Generates a random permutation of the elements in the input vector using the Fisher-Yates shuffle.
 *
 * @param list The input vector containing elements to be shuffled.
 *
 * @return A randomly shuffled vector.
 *
 */
std::vector<int> generateRandomVectorWithList(std::vector<int> list) {
    size_t n = list.size();
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
double simulation(std::vector<int>& path, matrix_3d& graph_matrix, setting& current_setting) {
    double current_time = current_setting.entry_time;

    //vectorDebug(path);
    for (int i = 0; i < path.size() - 1; ++i) {

        int current_intersection_ID = path[i];
        int next_intersection_ID = path[i + 1];


        double distance = graph_matrix[(int)(current_time) % 24][current_intersection_ID][next_intersection_ID];
        double time_taken = getTimeTaken(distance, current_time, next_intersection_ID, current_setting);
        if (distance == -1) {
            std::cerr << "Calcule du chemin interompu de " << current_intersection_ID << " � " << next_intersection_ID << std::endl;
            current_time = -1;
            break;
        }
        //std::cout << "dist = " << distance << std::endl;
        current_time += time_taken;
        //std::cout << "dist total = " << total_distance << std::endl;
    }
    current_time += graph_matrix[(int)(current_time) % 24][path.front()][0] / current_setting.walking_speed / 1000 * 60;
    current_time += graph_matrix[(int)(current_time) % 24][path.back()][0] / current_setting.walking_speed / 1000 * 60;
    return current_time;
}

std::vector<int> changeGene(std::vector<int> gene, matrix_3d& attraction_matrix, setting& current_setting) {
    int index1 = rand() % gene.size();

    int best_index = index1;

    double best_time = std::numeric_limits<double>::infinity();
    std::vector<int> best_gene = gene;
    int number_of_attraction = gene.size();
    for (int index2 = 0; index2 <= number_of_attraction - 1; index2++) {
        std::vector<int> gene_tmp = gene;
        std::swap(gene_tmp[index1], gene_tmp[index2]);
        double time_taken = simulation(gene_tmp, attraction_matrix, current_setting);

        if (time_taken < best_time) {
            best_time = time_taken;
            best_index = index2;
        }
    };

    std::swap(best_gene[index1], best_gene[best_index]);

    return best_gene;
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
std::pair<std::vector<std::pair<double, std::vector<int>>>, bool> regenerationPath(std::vector<std::pair<double, std::vector<int>>> path_data, matrix_3d& graph_matrix, setting& current_setting) {

    std::vector<std::pair<double, std::vector<int>>> childs_gene = {};


    int unique_gene = nb_gene;

    bool can_be_ended = false;
    for (auto& path : path_data) {
        std::vector<int> mixed_gene = changeGene(path.second, graph_matrix, current_setting);
        double score = simulation(mixed_gene, graph_matrix, current_setting);
        childs_gene.push_back({ score, mixed_gene });
    }


    if (unique_gene == 0) can_be_ended = true;

    //pathsDebug(childs_gene);
    return { childs_gene, can_be_ended };
}


/**
 * Generate a path through attractions using a genetic algorithm.
 *
 * @return The shortest path found by the genetic algorithm.
 */
std::vector<int> generatePath(setting& current_setting, int number_of_generation, int number_path) {

    nb_generation = number_of_generation;
    nb_gene = number_path;

    attraction_data = getAttractionData(DATA_LINK_JSON);
    hotel_data = getHotelData(DATA_LINK_JSON);
    intersection_data = getIntersectionData(DATA_LINK_JSON);
    attractionsDebug(attraction_data);
    srand(time(NULL));
    std::vector<int> path = {};
    std::vector<int> id_list = current_setting.ID_list;

    std::cout << "attraction used : " << std::endl;
    vectorDebug(id_list);
    std::cout << std::endl;

    std::vector<std::pair<double, std::vector<int>>> path_data = {};
    matrix_3d graph_attraction_matrix = getMatrix(id_list, current_setting);

    for (int i = 0; i < nb_gene; i++) {
        std::vector<int> randomised_path = generateRandomVectorWithList(id_list);
        double score = simulation(randomised_path, graph_attraction_matrix, current_setting); // score_taken = time in h
        path_data.push_back({ score, randomised_path });

    }


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

    std::vector<bool> completion_rate = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
    std::pair<std::vector<std::pair<double, std::vector<int>>>, bool> new_result = regenerationPath(path_data, graph_attraction_matrix, current_setting);
    std::vector<std::pair<double, std::vector<int>>> new_path_data = new_result.first;

    for (int i = 0; i < (nb_generation - 1); i++) {
        new_result = regenerationPath(new_path_data, graph_attraction_matrix, current_setting);
        std::cout << i << " gen | ";
        pathsDebug(new_path_data);
        std::cout << std::endl << std::endl;
        new_path_data = new_result.first;
        if (new_result.second) {
            std::cout << "premature stop: all genes are unique" << std::endl;
            break;
        }
    }


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

    //vectorDebug(new_shortest_path);

    pathInfoDisplay(new_shortest_path, current_setting);


    return new_shortest_path;
}