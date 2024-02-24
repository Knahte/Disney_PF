#include "import_data.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "../nlohmann/json.hpp"

using json = nlohmann::json;



////////////////////////
//---data-functions---//
////////////////////////

std::vector<attraction> attractionsOpenJson(std::string& data_URL) {
    std::vector<attraction> attractions;

    std::ifstream file(data_URL);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier JSON." << std::endl;
        return attractions;
    }

    json data;
    try {
        file >> data;

        for (json& attraction_data : data["Attractions"]) {
            attraction new_attraction;
            new_attraction.ID = attraction_data["id"];
            new_attraction.name = attraction_data["name"];
            new_attraction.location.lon = attraction_data["location"]["lon"];
            new_attraction.location.lat = attraction_data["location"]["lat"];
            new_attraction.visited = attraction_data["visited"];
            if (attraction_data["single_rider"] == 0)
                new_attraction.single_rider = attraction_data["id"];
            else
                new_attraction.single_rider = attraction_data["single_rider"];
            attractions.push_back(new_attraction);
        }
    }
    catch (std::exception& error) {
        std::cerr << "Erreur lors de l'analyse du fichier JSON : " << error.what() << std::endl;
    }

    file.close();

    return attractions;
}

std::vector<hotel> hotelOpenJson(std::string& data_URL) {
    std::vector<hotel> hotels;

    std::ifstream file(data_URL);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier JSON." << std::endl;
        return hotels;
    }

    json data;
    try {
        file >> data;

        for (json& hotel_data : data["Hotels"]) {
            hotel new_hotel;
            new_hotel.ID = hotel_data["id"];
            new_hotel.name = hotel_data["name"];
            new_hotel.location.lon = hotel_data["location"]["lon"];
            new_hotel.location.lat = hotel_data["location"]["lat"];

            hotels.push_back(new_hotel);
        }
    }
    catch (std::exception& error) {
        std::cerr << "Erreur lors de l'analyse du fichier JSON : " << error.what() << std::endl;
    }

    file.close();

    return hotels;
}

std::vector<intersection> intersectionOpenJson(std::string& data_URL) {
    std::vector<intersection> intersections;

    std::ifstream file(data_URL);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier JSON." << std::endl;
        return intersections;
    }

    json data;
    try {
        file >> data;

        for (json& inter_data : data["Intersections"]) {
            intersection new_inter;
            new_inter.ID = inter_data["id"];
            new_inter.name = inter_data["name"];
            new_inter.location.lon = inter_data["location"]["lon"];
            new_inter.location.lat = inter_data["location"]["lat"];
            for (auto& inter : inter_data["connected_to_intersections"]) {
                new_inter.intersection_linked.push_back(inter);
            }
            for (auto& attraction : inter_data["connected_to_attractions"]) {
                new_inter.attraction_linked.push_back(attraction);
            }
            for (auto& hotel : inter_data["connected_to_hotels"]) {
                new_inter.hotel_linked.push_back(hotel);
            }
            intersections.push_back(new_inter);
        }
    }
    catch (std::exception& error) {
        std::cerr << "Erreur lors de l'analyse du fichier JSON : " << error.what() << std::endl;
    }

    file.close();

    return intersections;
}

std::map <int, attraction> attractionVectorToMapById(std::vector <attraction>& attractions_list) {
    std::map <int, attraction> attractions_map;

    for (attraction& attr : attractions_list) {
        attractions_map[attr.ID] = attr;
    };

    return attractions_map;
}

std::map <int, hotel> hotelVectorToMapById(std::vector <hotel>& hotels_list) {
    std::map <int, hotel> hotels_map;

    for (hotel& hotel_to_add : hotels_list) {
        hotels_map[hotel_to_add.ID] = hotel_to_add;
    };

    return hotels_map;
}

std::map <int, intersection> intersectionVectorToMapById(std::vector <intersection>& intersections_list) {
    std::map <int, intersection> intersections_map;

    for (intersection& inter : intersections_list) {
        intersections_map[inter.ID] = inter;
    };

    return intersections_map;
}

void addIntersectionDatatoAttraction(std::vector<intersection>& intersections_list, std::map<int, attraction>& attraction_data) {
    for (auto& intersection : intersections_list) {
        for (int attraction_id : intersection.attraction_linked) {
            auto it = attraction_data.find(attraction_id);
            if (it != attraction_data.end()) {
                it->second.intersection_linked.push_back(intersection.ID);
            }
        }
    }
}

void addIntersectionDatatoHotel(std::vector<intersection>& intersections_list, std::map<int, hotel>& hotel_data) {
    for (auto& intersection : intersections_list) {
        for (int hotel_id : intersection.hotel_linked) {
            auto it = hotel_data.find(hotel_id);
            if (it != hotel_data.end()) {
                it->second.intersection_linked.push_back(intersection.ID);
            }
        }
    }
}


std::map<int, std::map<int, int>> attractionsOpenCSV(std::string& data_URL) {
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
                data_map[id] = std::map<int, int>{};
            }
            else if (colonne >= 3) {
                int heure = colonne + 5;
                int valeur;
                if (!cellule.empty()) {
                    valeur = std::stoi(cellule);
                }
                else {
                    valeur = 99999;
                }
                data_map[id][heure] = valeur;
            }
        }
    }

    fichier.close();

    return data_map;
}

attraction inputWaitTime(std::map<int, attraction>& attractions, std::map<int, std::map<int, int>>& wait_time_data) {
    for (auto& entry : wait_time_data) {
        int attraction_id = entry.first;
        auto it = attractions.find(attraction_id);

        if (it != attractions.end()) {
            it->second.wait_time = entry.second;
        }
        else {
            std::cerr << "Erreur : Aucune attraction correspondante trouv�e pour l'ID : " << attraction_id << std::endl;
        }
    }

    return attractions.begin()->second;

}

////////////////////////
//-returned-functions-//
////////////////////////

std::string DATA_LINK = "data/data.json";
std::string WAITING_TIME_LINK = "data/waiting_time.csv";



std::map <int, attraction>getAttractionData() {
    std::vector<attraction> attractions_vector = attractionsOpenJson(DATA_LINK);
    std::map <int, attraction> attraction_data = attractionVectorToMapById(attractions_vector);


    std::vector<intersection> intersections_vector = intersectionOpenJson(DATA_LINK);
    addIntersectionDatatoAttraction(intersections_vector, attraction_data);

    std::map<int, std::map<int, int>> wait_time_data = attractionsOpenCSV(WAITING_TIME_LINK);

    inputWaitTime(attraction_data, wait_time_data);

    return attraction_data;
}

std::map <int, hotel>getHotelData() {
    std::vector<hotel> hotels_vector = hotelOpenJson(DATA_LINK);
    std::map <int, hotel> hotel_data = hotelVectorToMapById(hotels_vector);


    std::vector<intersection> intersections_vector = intersectionOpenJson(DATA_LINK);
    addIntersectionDatatoHotel(intersections_vector, hotel_data);

    return hotel_data;
}

std::map <int, intersection> getIntersectionData() {
    std::vector<intersection> intersections_vector = intersectionOpenJson(DATA_LINK);
    std::map <int, intersection> intersection_data = intersectionVectorToMapById(intersections_vector);

    return intersection_data;
}