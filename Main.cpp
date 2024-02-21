////////////////////////
//--include-//-using--//
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



struct coordiante {
	float lon = 0.0;
	float lat = 0.0;
};

struct attraction {
	int ID = -1;
	std::string name = "NONE";
	coordiante location;
    std::map<int, int> wait_time = {};
    int single_rider = -1;
	bool visited = true;
};

struct hotel {
    int ID = -1;
    std::string name = "NONE";
    coordiante location;
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

//Input for Json function

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


//input from css file function

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

//searching function 

attraction searchAttraction(int& ID, const std::map<int, attraction>& attractions) {
    auto it = attractions.find(ID);
    if (it != attractions.end()) {
        return it->second;
    }
    else {
        std::cerr << "Attraction non trouvée pour l'ID : " << ID << std::endl;
        return attraction();
    }
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
    std::cout << '}';
}

void timeDataDebug(const std::map<int, int>& data_map) {
    std::cout << "{";
    for (const auto& entry : data_map) {
        std::cout << entry.first << ":" << entry.second << " | ";
    }
    std::cout << "\b\b\b   \b\b\b" << "}" << std::endl;
}

void attractionDebug(const attraction& attr) {
    std::cout << "Attraction ID: " << attr.ID << std::endl;
    std::cout << "Name: " << attr.name << std::endl;
    std::cout << "Location (lat, lon): " << attr.location.lat << ", " << attr.location.lon << std::endl;
    std::cout << "Wait Time: ";
    timeDataDebug(attr.wait_time);
    std::cout << "Single Rider ID : " << attr.single_rider << std::endl;
    std::cout << "Visited: " << std::boolalpha << attr.visited << std::endl << std::endl;
}

void hotelDebug(const hotel& hotel_to_print) {
    std::cout << "Hotel ID: " << hotel_to_print.ID << std::endl;
    std::cout << "Name: " << hotel_to_print.name << std::endl;
    std::cout << "Location (lat, lon): " << hotel_to_print.location.lat << ", " << hotel_to_print.location.lon << std::endl << std::endl;
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


int getDistance(const int& IDa, const int& IDb) {

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

    vectorDebug(id_list);
    std::vector<int> randomised_path = generateRandomVectorWithList(id_list);
    vectorDebug(randomised_path);
    int score = simulation(randomised_path);    


    std::map<int, std::map<std::string, int>> path_detailed = {};
    return path_detailed;
}

////////////////////////
//--------main--------//
////////////////////////



int main() {
    //std::string API_LINK = "https://queue-times.com/parks/4/queue_times.json";

    std::string DATA_LINK = "data/data.json";

    std::vector<attraction> attractions_vector = attractionsOpenJson(DATA_LINK);
    std::vector<hotel> hotels_vector = hotelOpenJson(DATA_LINK);

    std::map <int, attraction> sorted_attractions_data = attractionVectorToMapById(attractions_vector);
    std::map <int, hotel> sorted_hotels_data = hotelVectorToMapById(hotels_vector);

    std::map<int, std::map<int, int>> wait_time_data = attractionsOpenCSV("data/waiting_time.csv");

    inputWaitTime(sorted_attractions_data, wait_time_data);

    //attractionsDebug(sorted_attractions_data);
    //hotelsDebug(sorted_hotels_data);

    setting current_setting;


    std::map<int, std::map<std::string, int>> path_generated = generatePath(sorted_attractions_data, current_setting, sorted_hotels_data);

    pathDebug(path_generated);


    return 0;
}

