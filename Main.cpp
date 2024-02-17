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
	int ID = 0;
	std::string name = "NONE";
	coordiante location;
	bool is_open = false;
    std::vector<int> wait_time = {};
	bool visited = true;
};


////////////////////////
//------function------//
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

            attractions.push_back(new_attraction);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur lors de l'analyse du fichier JSON : " << e.what() << std::endl;
    }

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


//input from css file function

std::map<int, std::vector<int>> attractionsOpenCSV(const std::string& data_URL) {
    std::map<int, std::vector<int>> data_map;

    std::ifstream fichier(data_URL); // Ouvre le fichier CSV en lecture

    if (!fichier) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier." << std::endl;
        return data_map; 
    }

    std::string ligne;
    bool premiere_ligne = true;

    // Lire chaque ligne du fichier
    while (std::getline(fichier, ligne)) {
        if (premiere_ligne) {
            premiere_ligne = false;
            continue; 
        }

        std::istringstream stream(ligne);
        std::string cellule;
        std::vector<int> ligne_csv;
        int id;
        bool first = true;
        int colonne = 0;

        while (std::getline(stream, cellule, ';')) {
            colonne++;

            if (colonne == 1) {
                id = std::stoi(cellule);
            }
            else if (colonne > 3) {
                if (cellule.empty())
                    ligne_csv.push_back(9999);
                else
                    ligne_csv.push_back(std::stoi(cellule));
            }
        }

        if (colonne >= 2)
            data_map[id] = ligne_csv;
    }

    fichier.close();

    return data_map;
}

attraction inputWaitTime(std::map<int, attraction>& attractions, std::map<int, std::vector<int>>& wait_time_data) {
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



//debug function

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

void attractionsDebug(const std::map<int, attraction>& attractions_data) {
    for (const auto& pair : attractions_data) {
        const attraction& attr = pair.second;
        std::cout << "Attraction ID: " << attr.ID << std::endl;
        std::cout << "Name: " << attr.name << std::endl;
        std::cout << "Location (lat, lon): " << attr.location.lat << ", " << attr.location.lon << std::endl;
        std::cout << "Is Open: " << std::boolalpha << attr.is_open << std::endl;
        std::cout << "Wait Time: ";
        vectorDebug(attr.wait_time);
        std::cout << std::endl;
        std::cout << "Visited: " << std::boolalpha << attr.visited << std::endl << std::endl;
    }
}

int main() {
	//std::string API_LINK = "https://queue-times.com/parks/4/queue_times.json";

    std::string DATA_LINK = "data/data.json";
    std::vector<attraction> attractions_vector = attractionsOpenJson(DATA_LINK);

    std::map <int, attraction> sorted_attractions_data = vectorToMapById(attractions_vector);

    std::map<int, std::vector<int>> wait_time_data = attractionsOpenCSV("data/waiting_time.csv");

    inputWaitTime(sorted_attractions_data, wait_time_data);

    attractionsDebug(sorted_attractions_data);

    return 0;
}

