////////////////////////
//-include-//--using--//
////////////////////////

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <algorithm>
#include "Include/nlohmann/json.hpp"

using json = nlohmann::json;



////////////////////////
//----code-setting----//
////////////////////////


int nb_gene = 30;
double selectivity = 0.1;
int nb_generation = 30;


////////////////////////
//-------struct-------//
////////////////////////

struct coordinate {
	float lon = 0.0;
	float lat = 0.0;
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
    std::vector<int> available_time = { 8,23 }; //(min: 8 | max:23)
    int available_days = 3;
    int hotel_ID = 0; //see Json file "data.json"
    int walking_speed = 5; //in km/h - (min: 2 | max: 8)
};



////////////////////////
//---data-functions---//
////////////////////////

std::vector<attraction> attractionsOpenJson(const std::string& data_URL) {
    std::vector<attraction> attractions;

    std::ifstream file(data_URL);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier JSON." << std::endl;
        return attractions;
    }

    json data;
    try {
        file >> data;

        for (const json& attraction_data : data["Attractions"]) {
            attraction new_attraction;
            new_attraction.ID = attraction_data["id"];
            new_attraction.name = attraction_data["name"];
            new_attraction.location.lon = attraction_data["location"]["lon"];
            new_attraction.location.lat = attraction_data["location"]["lat"];
            new_attraction.visited = attraction_data["visited"];
            if (attraction_data["single_rider"]==0)
                new_attraction.single_rider = attraction_data["id"];
            else
                new_attraction.single_rider = attraction_data["single_rider"];
            attractions.push_back(new_attraction);
        }
    }
    catch (const std::exception& error) {
        std::cerr << "Erreur lors de l'analyse du fichier JSON : " << error.what() << std::endl;
    }

    file.close();

    return attractions;
}

std::vector<hotel> hotelOpenJson(const std::string& data_URL) {
    std::vector<hotel> hotels;

    std::ifstream file(data_URL);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier JSON." << std::endl;
        return hotels;
    }

    json data;
    try {
        file >> data;

        for (const json& hotel_data : data["Hotels"]) {
            hotel new_hotel;
            new_hotel.ID = hotel_data["id"];
            new_hotel.name = hotel_data["name"];
            new_hotel.location.lon = hotel_data["location"]["lon"];
            new_hotel.location.lat = hotel_data["location"]["lat"];

            hotels.push_back(new_hotel);
        }
    }
    catch (const std::exception& error) {
        std::cerr << "Erreur lors de l'analyse du fichier JSON : " << error.what() << std::endl;
    }

    file.close();

    return hotels;
}

std::vector<intersection> intersectionOpenJson(const std::string& data_URL) {
    std::vector<intersection> intersections;

    std::ifstream file(data_URL);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier JSON." << std::endl;
        return intersections;
    }

    json data;
    try {
        file >> data;

        for (const json& inter_data : data["Intersections"]) {
            intersection new_inter;
            new_inter.ID = inter_data["id"];
            new_inter.name = inter_data["name"];
            new_inter.location.lon = inter_data["location"]["lon"];
            new_inter.location.lat = inter_data["location"]["lat"];
            for (const auto& inter : inter_data["connected_to_intersections"]) {
                new_inter.intersection_linked.push_back(inter);
            }
            for (const auto& attraction : inter_data["connected_to_attractions"]) {
                new_inter.attraction_linked.push_back(attraction);
            }
            for (const auto& hotel : inter_data["connected_to_hotels"]) {
                new_inter.hotel_linked.push_back(hotel);
            }
            intersections.push_back(new_inter);
        }
    }
    catch (const std::exception& error) {
        std::cerr << "Erreur lors de l'analyse du fichier JSON : " << error.what() << std::endl;
    }

    file.close();

    return intersections;
}

std::map <int, attraction> attractionVectorToMapById(std::vector <attraction>& attractions_list) {
    std::map <int, attraction> attractions_map;

    for (const attraction& attr : attractions_list) {
        attractions_map[attr.ID] = attr;
    };
    
    return attractions_map;
}

std::map <int, hotel> hotelVectorToMapById(std::vector <hotel>& hotels_list) {
    std::map <int, hotel> hotels_map;

    for (const hotel& hotel_to_add : hotels_list) {
        hotels_map[hotel_to_add.ID] = hotel_to_add;
    };

    return hotels_map;
}

std::map <int, intersection> intersectionVectorToMapById(std::vector <intersection>& intersections_list) {
    std::map <int, intersection> intersections_map;

    for (const intersection& inter : intersections_list) {
        intersections_map[inter.ID] = inter;
    };

    return intersections_map;
}

void addIntersectionData(const std::vector<intersection>& intersections_list, std::map<int, attraction>& attraction_data, std::map<int, hotel>& hotel_data) {
    for (const auto& intersection : intersections_list) {
        for (int attraction_id : intersection.attraction_linked) {
            auto it = attraction_data.find(attraction_id);
            if (it != attraction_data.end()) {
                it->second.intersection_linked.push_back(intersection.ID);
            }
        }
        for (int hotel_id : intersection.hotel_linked) {
            auto it = hotel_data.find(hotel_id);
            if (it != hotel_data.end()) {
                it->second.intersection_linked.push_back(intersection.ID);
            }
        }
    }
}

std::map<int, std::map<int, int>> attractionsOpenCSV(const std::string& data_URL) {
    std::map<int, std::map<int, int>> data_map;

    std::ifstream fichier(data_URL); 

    if (!fichier) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier." << std::endl;
        return data_map;
    }

    std::string ligne;
    bool premiere_ligne = true;

    while (std::getline(fichier, ligne)) {
        if (premiere_ligne) {
            premiere_ligne = false;
            continue;
        }

        std::istringstream stream(ligne);
        std::string cellule;
        int id;
        int colonne = 0;

        while (std::getline(stream, cellule, ';')) {
            colonne++;

            if (colonne == 1) {
                id = std::stoi(cellule);
                data_map[id] = std::map<int, int>();
            }
            else if (colonne >= 3) {
                int heure = colonne + 5;
                int valeur = cellule.empty() ? -1 : std::stoi(cellule);
                data_map[id][heure] = valeur;
            }
        }
    }

    fichier.close();

    return data_map;
}

attraction inputWaitTime(std::map<int, attraction>& attractions, std::map<int, std::map<int, int>>& wait_time_data) {
    for (const auto& entry : wait_time_data) {
        int attraction_id = entry.first;
        auto it = attractions.find(attraction_id);

        if (it != attractions.end()) {
            it->second.wait_time = entry.second;
        }
        else {
            std::cerr << "Erreur : Aucune attraction correspondante trouvée pour l'ID : " << attraction_id << std::endl;
        }
    }

    return attractions.begin()->second;

}



////////////////////////
//-distance-functions-//
////////////////////////

template<typename T1, typename T2>
double getColseDistance(const int& a_ID, const int& b_ID, const std::map<int, T1>& a_data, const std::map<int, T2>& b_data) {
    auto a_item = a_data.find(a_ID);
    auto b_item = b_data.find(b_ID);

    if (a_item == a_data.end() || b_item == b_data.end()) {
        return -1; 
        
    }
    const double a_lon = (a_item->second.location.lon) * 0.0174533; // angle[deg] * 0,0174533 = angle[rad] 
    const double b_lon = (b_item->second.location.lon) * 0.0174533;
    const double a_lat = (a_item->second.location.lat) * 0.0174533;
    const double b_lat = (b_item->second.location.lat) * 0.0174533;


    double distance = acos(sin(a_lat) * sin(b_lat) + cos(a_lat) * cos(b_lat) * cos(b_lon - a_lon)) * 6371000; //6371000 = Radius of the Earth (conventional)
    return distance;
}

double findShortestPath(const intersection& start_intersection, const intersection& end_intersection, const std::map<int, intersection>& intersection_data) {
    std::map<int, bool> visited;
    std::map<int, double> distance;

    for (const auto& item_inter : intersection_data) {
        int id = item_inter.first;
        distance[id] = std::numeric_limits<double>::infinity();
        visited[id] = false;
    }
    distance[start_intersection.ID] = 0;

    while (true) {
        int position_index = -1;
        double min_distance = std::numeric_limits<double>::infinity();

        for (const auto& item_distance : distance) {
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

double getLongDistance(const int& start_ID, const int& end_ID, const std::map<int, intersection>& intersection_data, const std::map<int, attraction>& attraction_data) {
    double min_distance = std::numeric_limits<double>::infinity();;

    auto start_attraction = attraction_data.find(start_ID);
    auto end_attraction = attraction_data.find(end_ID);

    if(!(start_attraction->second.intersection_linked.empty() || end_attraction->second.intersection_linked.empty()))
    {
        for (int start_intersection_id : start_attraction->second.intersection_linked) {

            for (int end_intersection_id : end_attraction->second.intersection_linked) {
                //std::cout << start_intersection_id << " -> " << end_intersection_id << std::endl;
                double distance = findShortestPath(intersection_data.at(start_intersection_id), intersection_data.at(end_intersection_id), intersection_data) + getColseDistance(start_ID, start_intersection_id, attraction_data, intersection_data) + getColseDistance(end_ID, end_intersection_id, attraction_data, intersection_data);
                if (distance <= -1) {
                    return -1;
                }
                else
                    if (distance < min_distance) {
                        min_distance = distance;
                    }
            }
        }
    }
    else {
        std::cerr << "no attraction accessible in " << start_ID << " or " << end_ID << " attraction" << std::endl;
        return -1;
    }

    return min_distance;
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

double simulation(std::vector<int>& path, const std::map <int, attraction>& attraction_data, const std::map <int, intersection>& intersection_data, const setting& setting_data, const std::map <int, hotel>& hotel_data){
    double total_distance = 0;

    for (size_t i = 0; i < path.size() - 1; ++i) {
        int current_intersection_ID = path[i];
        int next_intersection_ID = path[i + 1];

        double distance = getLongDistance(current_intersection_ID, next_intersection_ID, intersection_data, attraction_data) ;
        if (distance == -1) {
            std::cerr << "Calcule du chemin interompu de " << current_intersection_ID << " à " << next_intersection_ID << std::endl;
            total_distance = -1;
            break;
        }
        //std::cout << "dist = " << distance << std::endl;
        total_distance += distance;
        //std::cout << "dist total = " << total_distance << std::endl;
    }
    total_distance += getColseDistance(setting_data.hotel_ID, path.front(), hotel_data, attraction_data);
    total_distance += getColseDistance(setting_data.hotel_ID, path.back(), hotel_data, attraction_data);
    return total_distance;
}

std::vector<int> mixingGene(std::vector<int> gene1, std::vector<int> gene2) {
    std::vector<int> mixed_gene;
    while (!gene1.empty() && !gene2.empty()) {
        bool choose_gene1 = rand() % 2 == 0;

        if (choose_gene1) {
            mixed_gene.push_back(gene1.front());
            gene2.erase(std::find(gene2.begin(), gene2.end(), gene1.front()));
            gene1.erase(gene1.begin());
        }
        else {
            mixed_gene.push_back(gene2.front());
            gene1.erase(std::find(gene1.begin(), gene1.end(), gene2.front()));
            gene2.erase(gene2.begin());
        }      
    }
    return mixed_gene;
}

std::vector<std::pair<int, std::vector<int>>> regenerationPath(std::vector<std::pair<int, std::vector<int>>> path_data, const std::map <int, attraction>& attraction_data, const std::map <int, intersection>& intersection_data, const std::map <int, hotel>& hotel_data, const setting setting_data) {
    
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
        //std::cout << "parent size : " << parents_gene.size() << " | index given : " << index_parent1 << "  " << index_parent2 << std::endl;
        mixed_gene = mixingGene(parents_gene[index_parent1].second, parents_gene[index_parent2].second);
        double score = simulation(mixed_gene, attraction_data, intersection_data, setting_data, hotel_data);
        childs_gene.push_back({ score, mixed_gene });
    }

    return childs_gene;

}

std::vector<int> generatePath(const std::map <int, attraction>& attraction_data, const setting& setting_data, const std::map <int, intersection>& intersection_data, const std::map <int, hotel>& hotel_data){
    std::vector<int> path = {};
    std::vector<int> id_list = {};
    if (setting_data.single_rider) {
        for (const auto& pair : attraction_data) {
            const attraction& attr = pair.second;
            if (attr.single_rider == attr.ID) {
                id_list.push_back(attr.single_rider);
            }
        }
    }
    else {
        for (const auto& pair : attraction_data) {
            const attraction& attr = pair.second;
            id_list.push_back(attr.ID);
        }
    }

    std::vector<std::pair<int, std::vector<int>>> path_data = {};
    for (int i = 0; i < nb_gene; i++) {
        std::vector<int> randomised_path = generateRandomVectorWithList(id_list);
        double score = simulation(randomised_path, attraction_data, intersection_data, setting_data, hotel_data);
        path_data.push_back({score, randomised_path});
    }

    int min_score = std::numeric_limits<int>::max();
    std::vector<int> shortest_path;

    for (const auto& entry : path_data) {
        int score = entry.first;
        const std::vector<int>& path = entry.second;

        if (score < min_score) {
            min_score = score;
            shortest_path = path;
        }
    }

    std::vector<bool> completion_rate = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };

    std::vector<std::pair<int, std::vector<int>>> new_path_data = regenerationPath(path_data, attraction_data, intersection_data, hotel_data, setting_data);
    for (int i = 0; i < (nb_generation - 1); i++) {
        new_path_data = regenerationPath(path_data, attraction_data, intersection_data, hotel_data, setting_data);
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

    for (const auto& entry : new_path_data) {
        int new_score = entry.first;
        const std::vector<int>& path = entry.second;

        if (new_score < new_min_score) {
            new_min_score = new_score;
            new_shortest_path = path;
        }
    }

    std::cout << "min distance for this path before genetic :" << min_score << std::endl;
    std::cout << "min distance for this path after  genetic :" << new_min_score << std::endl;
    return new_shortest_path;
}



////////////////////////
//--debug--functions--//
////////////////////////

void vectorDebug(const std::vector<int>& vector_data) {
    std::cout << '{';
    if (not vector_data.empty()) {
        for (size_t i = 0; i < vector_data.size() - 1; ++i) {
            std::cout << vector_data[i] << " ; ";
        }
        std::cout << vector_data.back();
    }
    std::cout << '}' << std::endl;
}

void timeDataDebug(const std::map<int, int>& data_map) {
    std::cout << "{";
    for (const auto& entry : data_map) {
        std::cout << entry.first << ":" << entry.second << " | ";
    }
    std::cout << "}" << std::endl;
}

void attractionDebug(const attraction& attr) {
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

void hotelDebug(const hotel& hotel_to_print) {
    std::cout << "Hotel ID: " << hotel_to_print.ID << std::endl;
    std::cout << "Name: " << hotel_to_print.name << std::endl;
    std::cout << "Location (lat, lon): " << hotel_to_print.location.lat << ", " << hotel_to_print.location.lon << std::endl;
    std::cout << "Connected with : ";
    vectorDebug(hotel_to_print.intersection_linked);
    std::cout << std::endl;
}

void intersectionDebug(const intersection& inter) {
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

void attractionsDebug(const std::map<int, attraction>& attractions_data) {
    for (const auto& pair : attractions_data) {
        const attraction& attr = pair.second;
        attractionDebug(attr);
    }
}

void hotelsDebug(const std::map<int, hotel>& hotels_data) {
    for (const auto& pair : hotels_data) {
        const hotel& hotel_to_print = pair.second;
        hotelDebug(hotel_to_print);
    }
}

void intersectionsDebug(const std::map<int, intersection>& intersections_data) {
    for (const auto& pair : intersections_data) {
        const intersection& inter_to_print = pair.second;
        intersectionDebug(inter_to_print);
    }
}

void pathDebug(const std::vector<int>& path, const std::map<int, attraction>& attraction_data, const std::map<int, intersection>& intersection_data) {
    double total_distance = 0;
    std::cout << "Attractions in the path:" << std::endl;
    for (size_t i = 1; i < path.size(); ++i) {
        int current_id = path[i - 1];
        int next_id = path[i]; 

        auto current_attraction = attraction_data.find(current_id);
        auto next_attraction = attraction_data.find(next_id);

        if (current_attraction != attraction_data.end() && next_attraction != attraction_data.end()) {
            double distance = getLongDistance(current_id, next_id, intersection_data, attraction_data); 
            total_distance += distance; 
            std::cout << "Attraction ID: " << current_attraction->first << ", Name: " << current_attraction->second.name << ", Distance to next: " << distance << ", Distance tot: " << total_distance << std::endl;
        }
    }
    std::cout << "Total distance traveled: " << total_distance/1000 << "km" << std::endl;
}



////////////////////////
//--------main--------//
////////////////////////

int main() {
    srand(time(NULL));

    //std::string API_LINK = "https://queue-times.com/parks/4/queue_times.json";

    const std::string DATA_LINK = "data/data.json";

    std::vector<attraction> attractions_vector = attractionsOpenJson(DATA_LINK);
    std::vector<hotel> hotels_vector = hotelOpenJson(DATA_LINK);
    

    std::map <int, attraction> sorted_attractions_data = attractionVectorToMapById(attractions_vector);
    std::map <int, hotel> sorted_hotels_data = hotelVectorToMapById(hotels_vector);
    

    std::vector<intersection> intersections_vector = intersectionOpenJson(DATA_LINK);
    std::map <int, intersection> sorted_intersections_data = intersectionVectorToMapById(intersections_vector);

    addIntersectionData(intersections_vector, sorted_attractions_data, sorted_hotels_data);

    std::map<int, std::map<int, int>> wait_time_data = attractionsOpenCSV("data/waiting_time.csv");

    inputWaitTime(sorted_attractions_data, wait_time_data);

    attractionDebug(sorted_attractions_data[8]);

    //attractionsDebug(sorted_attractions_data);
    //hotelsDebug(sorted_hotels_data);
    //intersectionsDebug(sorted_intersections_data);

    setting current_setting;


    std::vector<int> path_generated = generatePath(sorted_attractions_data, current_setting, sorted_intersections_data, sorted_hotels_data);

    pathDebug(path_generated, sorted_attractions_data, sorted_intersections_data);

    return 0;
}