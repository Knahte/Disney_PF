////////////////////////
//-include-//--using--//
////////////////////////



#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "Include/nlohmann/json.hpp"

using json = nlohmann::json;

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
    bool single_rider = false;
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
                if (it->second.single_rider != -1 && it->second.single_rider != attraction_id) {
                    auto it_single_rider = attraction_data.find(it->second.single_rider);
                    if (it_single_rider != attraction_data.end()) {
                        it_single_rider->second.intersection_linked.push_back(intersection.ID);
                    }
                }
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

void pathDebug(std::map<int, std::map<std::string, int>>& path_data) {
    for (const auto& pair : path_data) {
        const std::map<std::string, int>& step = pair.second;
        for (const auto& step_pair : step) {
            std::cout << step_pair.first << ": " << step_pair.second << std::endl;
        }
        std::cout << std::endl;
    }
}



////////////////////////
//-distance-functions-//
////////////////////////

std::map<std::string, std::map<int, std::vector<double>>> getGraph(const std::map<int, intersection>& intersection_data) {    //{std::string,std::map<int, std::vector<double>>,"hotel"}
    std::map<std::string, std::map<int, std::vector<double>>> data = {
        {"attraction", {}},
        {"hotel", {}},
        {"intersection", {}}
    };

    //The complicated code should go here... help.


    return data;
}

template<typename T1, typename T2>
int getDistance(const int& a_ID, const int& b_ID, const std::map<int, T1>& a_data, const std::map<int, T2>& b_data) {
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



////////////////////////
//simulation-functions//
////////////////////////

std::vector<int> generateRandomVectorWithList(std::vector<int> list) {
    srand(time(0));

    int n = list.size();
    for (int i = 0; i < n - 1; ++i) {
        int j = i + rand() % (n - i);
        int tmp = list[i];
        list[i] = list[j];
        list[j] = tmp;
    }

    return list;
}

int simulation(std::vector<int>& path){
    //now the fun begin !  :D
    return 0;
}

std::map<int, std::map<std::string, int>> generatePath(const std::map <int, attraction>& attractions_map, const setting& setting_data, const std::map <int, hotel>& sorted_hotels_data){
    std::vector<int> path = {};
    std::vector<int> id_list;
    if (setting_data.single_rider) {
        for (const auto& pair : attractions_map) {
            const attraction& attr = pair.second;
            id_list.push_back(attr.single_rider);
        }
    }
    else {
        for (const auto& pair : attractions_map) {
            const attraction& attr = pair.second;
            id_list.push_back(attr.ID);
        }
    }

    //vectorDebug(id_list);
    std::vector<int> randomised_path = generateRandomVectorWithList(id_list);
    //vectorDebug(randomised_path);
    int score = simulation(randomised_path);    


    std::map<int, std::map<std::string, int>> path_detailed = {};
    return path_detailed;
}

////////////////////////
//--------main--------//
////////////////////////



int main() {
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

    //attractionsDebug(sorted_attractions_data);
    //hotelsDebug(sorted_hotels_data);
    //intersectionsDebug(sorted_intersections_data);

    setting current_setting;


    std::map<int, std::map<std::string, int>> path_generated = generatePath(sorted_attractions_data, current_setting, sorted_hotels_data);

    //pathDebug(path_generated);

    return 0;
}

