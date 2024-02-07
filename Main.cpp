////////////////////////
//--include-//-using--//
////////////////////////



#include <iostream>
#include <string>
#include <fstream>
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
	int ID = 0;
	std::string name = "NONE";
	coordiante location;
	bool is_open = false;
	int wait = 0;
	bool visited = true;
};


////////////////////////
//------function------//
////////////////////////

std::vector<attraction> attractionsOpenJson(const std::string& data_URL) {
    std::vector<attraction> attractions;

    // Ouvrir le fichier JSON
    std::ifstream file(data_URL);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier JSON." << std::endl;
        return attractions;
    }

    // Analyser le contenu JSON
    json data;
    try {
        file >> data;

        // Parcourir toutes les attractions
        for (const json& attraction_data : data["Attractions"]) {
            attraction new_attraction;
            new_attraction.ID = attraction_data["id"];
            new_attraction.name = attraction_data["name"];
            new_attraction.location.lon = attraction_data["location"]["lon"];
            new_attraction.location.lat = attraction_data["location"]["lat"];
            new_attraction.visited = attraction_data["visited"];

            // Ajouter l'attraction à la liste
            attractions.push_back(new_attraction);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur lors de l'analyse du fichier JSON : " << e.what() << std::endl;
    }

    // Fermer le fichier
    file.close();

    return attractions;
}

std::map <int, attraction> vectorToMapById(std::vector <attraction> attractions_list) {
    std::map <int, attraction> attractions_map;

    for (const attraction& attr : attractions_list) {
        attractions_map[attr.ID] = attr;
    };
    
    return attractions_map;
}


void attractionsDebug(const std::map<int, attraction>& attractions_data) {
    for (const auto& pair : attractions_data) {
        const attraction& attr = pair.second;
        std::cout << "Attraction ID: " << attr.ID << std::endl;
        std::cout << "Name: " << attr.name << std::endl;
        std::cout << "Location (lat, lon): " << attr.location.lat << ", " << attr.location.lon << std::endl;
        std::cout << "Is Open: " << std::boolalpha << attr.is_open << std::endl;
        std::cout << "Wait Time: " << attr.wait << std::endl;
        std::cout << "Visited: " << std::boolalpha << attr.visited << std::endl << std::endl;
    }
}

int main() {
	//std::string API_LINK = "https://queue-times.com/parks/4/queue_times.json";

    std::string DATA_LINK = "data/data.json";
    std::vector<attraction> attractions_vector = attractionsOpenJson(DATA_LINK);

    std::map <int, attraction> sorted_attractions_data = vectorToMapById(attractions_vector);

    // Afficher les attractions pour vérifier
    attractionsDebug(sorted_attractions_data);

    return 0;
}

