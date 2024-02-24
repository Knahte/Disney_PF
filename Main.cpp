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

using matrix_3d = std::map<int, std::map<int, std::map<int, int>>>;



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
int nb_generation = 300;

std::map<int, attraction> attraction_data = getAttractionData();
std::map<int, hotel> hotel_data = getHotelData();
std::map<int, intersection> intersection_data = getIntersectionData();

setting current_setting; //temporary



////////////////////////
//--debug--functions--//
////////////////////////

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

void timeDataDebug(std::map<int, int>& data_map) {
    std::cout << "{";
    for (auto& entry : data_map) {
        std::cout << entry.first << ":" << entry.second << " | ";
    }
    std::cout << "}" << std::endl;
}

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

void hotelDebug(hotel& hotel_to_print) {
    std::cout << "Hotel ID: " << hotel_to_print.ID << std::endl;
    std::cout << "Name: " << hotel_to_print.name << std::endl;
    std::cout << "Location (lat, lon): " << hotel_to_print.location.lat << ", " << hotel_to_print.location.lon << std::endl;
    std::cout << "Connected with : ";
    vectorDebug(hotel_to_print.intersection_linked);
    std::cout << std::endl;
}

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

void attractionsDebug(std::map<int, attraction>& attractions_data) {
    for (auto& pair : attractions_data) {
        attraction& attr = pair.second;
        attractionDebug(attr);
    }
}

void hotelsDebug(std::map<int, hotel>& hotels_data) {
    for (auto& pair : hotels_data) {
        hotel& hotel_to_print = pair.second;
        hotelDebug(hotel_to_print);
    }
}

void intersectionsDebug(std::map<int, intersection>& intersections_data) {
    for (auto& pair : intersections_data) {
        intersection& inter_to_print = pair.second;
        intersectionDebug(inter_to_print);
    }
}

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

void debugMatrix(const std::map<int, std::map<int, double>>& matrix) {
    for (const auto& row : matrix) {
        std::cout << "Attraction ID: " << row.first << std::endl;
        for (const auto& col : row.second) {
            std::cout << std::setw(10) << col.second << " ";
        }
        std::cout << std::endl;
    }
}



////////////////////////
//-------output-------//
////////////////////////

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

    std::cout << "Fichier GPX généré avec succès : output/path.gpx" << std::endl;
}



////////////////////////
//-distance-functions-//
////////////////////////

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

double simulation(std::vector<int>& path, matrix_3d& graph_matrix){
    int current_time = current_setting.available_time.first * 60;
    int max_time = current_setting.available_time.second * 60;
    
    
    for (int i = 0; i < path.size() - 1; ++i) {
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
            continue; // La section coupée est déjà remplie
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
        new_path_data = regenerationPath(path_data, graph_matrix);
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

    std::cout << "min time for this path before genetic :" << min_score / 60.0 << "h" << std::endl;
    std::cout << "min time for this path after  genetic :" << new_min_score / 60.0 << "h" << std::endl;
    return new_shortest_path;
}



////////////////////////
//--------main--------//
////////////////////////

int main() {
    srand(time(NULL));

    std::vector<int> path_generated = generatePath();

    pathDebug(path_generated);

    pathToGPX(path_generated);

    return 0;
}