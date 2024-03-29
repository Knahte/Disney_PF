#include "import_data.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Include/nlohmann/json.hpp"

using json = nlohmann::json;

static setting current_setting;

////////////////////////
//---data-functions---//
////////////////////////

/**
 * Opens the Json file and extracts the attractions in order to store them in a variable type (type = [attraction]) that can be used by the program
 *
 * @param The access path to the database in json format
 *
 * @return vector with [attraction] type attractions inside
 *
 */
static std::vector<attraction> attractionsOpenJson(std::string& data_URL) {
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

/**
 * Opens the Json file and extracts the hotels in order to store them in a variable type (type = [hotel]) that can be used by the program
 *
 * @param The access path to the database in json format
 *
 * @return vector with [hotel] type hotels inside
 *
 */
static std::vector<hotel> hotelOpenJson(std::string& data_URL) {
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

/**
 * Opens the Json file and extracts the intersections in order to store them in a variable type (type = [intersection]) that can be used by the program
 *
 * @param The access path to the database in json format
 *
 * @return A vector with [intersection] type intersections inside
 *
 */
static std::vector<intersection> intersectionOpenJson(std::string& data_URL, std::map<int, attraction>& attraction_data) {
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
            for (auto& attraction_ID : inter_data["connected_to_attractions"]) {
                if (std::find(current_setting.full_ID_list.begin(), current_setting.full_ID_list.end(), attraction_ID) != current_setting.full_ID_list.end()) {
                    new_inter.attraction_linked.push_back(attraction_ID);
                    new_inter.intersection_linked.push_back(attraction_ID * 1000);
                }
            }
            for (auto& hotel : inter_data["connected_to_hotels"]) {
                new_inter.hotel_linked.push_back(hotel);
            }
            intersections.push_back(new_inter);
        }


        for (json& attr_data : data["Attractions"]) {
            if (std::find(current_setting.full_ID_list.begin(), current_setting.ID_list.end(), attr_data["id"]) != current_setting.full_ID_list.end()) {
                intersection new_inter;
                new_inter.ID = attr_data["id"] * 1000;
                new_inter.name = attr_data["name"];
                new_inter.location.lon = attr_data["location"]["lon"];
                new_inter.location.lat = attr_data["location"]["lat"];

                for (auto& intersection_ID : attraction_data[attr_data["id"]].intersection_linked) {
                    if (intersection_ID != new_inter.ID) {
                        new_inter.intersection_linked.push_back(intersection_ID);

                    }
                }
                for (auto& attraction_ID : attr_data["direct_way_to_attractions"]) {
                    //if (std::find(current_setting.full_ID_list.begin(), current_setting.full_ID_list.end(), attraction_ID) != current_setting.full_ID_list.end())
                    new_inter.intersection_linked.push_back(attraction_ID * 1000);
                }

                new_inter.attraction_linked.push_back(attr_data["id"]);

                intersections.push_back(new_inter);

            }
        }
    }

    catch (std::exception& error) {
        std::cerr << "Erreur lors de l'analyse du fichier JSON : " << error.what() << std::endl;
    }

    file.close();

    return intersections;
}

/**
 * Convert the sequential container [vector] into an association container [map] with the attraction ID as the key to the attraction
 *
 * @param A vector with [attraction] type attractions inside
 *
 * @return A map with : key = ID | content = attraction associated with this ID
 *
 */
static std::map <int, attraction> attractionVectorToMapById(std::vector <attraction>& attractions_list) {
    std::map <int, attraction> attractions_map;

    for (attraction& attr : attractions_list) {
        attractions_map[attr.ID] = attr;
    };

    return attractions_map;
}

/**
 * Convert the sequential container [vector] into an association container [map] with the hotel ID as the key to the hotel
 *
 * @param A vector with [hotel] type hotels inside
 *
 * @return A map with : key = ID | content = hotel associated with this ID
 *
 */
static std::map <int, hotel> hotelVectorToMapById(std::vector <hotel>& hotels_list) {
    std::map <int, hotel> hotels_map;

    for (hotel& hotel_to_add : hotels_list) {
        hotels_map[hotel_to_add.ID] = hotel_to_add;
    };

    return hotels_map;
}

/**
 * Convert the sequential container [vector] into an association container [map] with the attraction ID as the key to the intersection
 *
 * @param A vector with [intersection] type intersections inside
 *
 * @return A map with : key = ID | content = intersection associated with this ID
 *
 */
static std::map <int, intersection> intersectionVectorToMapById(std::vector <intersection>& intersections_list) {
    std::map <int, intersection> intersections_map;

    for (intersection& inter : intersections_list) {
        intersections_map[inter.ID] = inter;
    };

    return intersections_map;
}

/**
 * Adds information on directly connected intersections to attractions dataset.
 *
 * @param The vector with intersections
 * @param The map with ALL the attractions in the dataset
 *
 */
static void addIntersectionDatatoAttraction(std::map <int, intersection>& intersections_map, std::map<int, attraction>& attraction_data) {
    for (auto& intersection_pair : intersections_map) {
        intersection& inter = intersection_pair.second;
        for (int attraction_id : inter.attraction_linked) {
            auto attr = attraction_data.find(attraction_id);
            if (attr != attraction_data.end()) {
                attr->second.intersection_linked.push_back(inter.ID);
            }
        }
    }
}

/**
 * Adds information on directly connected intersections to hotels dataset.
 *
 * @param The vector with intersections
 * @param The map with ALL the hotels in the dataset
 *
 */
static void addIntersectionDatatoHotel(std::map <int, intersection>& intersections_map, std::map<int, hotel>& hotel_data) {
    for (auto& intersection_pair : intersections_map) {
        intersection& inter = intersection_pair.second;
        for (int hotel_id : inter.hotel_linked) {
            auto hotel_used = hotel_data.find(hotel_id);
            if (hotel_used != hotel_data.end()) {
                hotel_used->second.intersection_linked.push_back(inter.ID);
            }
        }
    }
}

/**
 * Extracts the waiting times for each attraction in the specified CSV file
 *
 * @param The access path to the database in CSV format
 *
 * @return A map with the time of day as the key and another map with the attraction ID as the key and the waiting time as the content
 *
 */
static std::map<int, std::map<int, int>> attractionsOpenCSV(std::string& data_URL) {
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
                int heure = (colonne - 2) % 24;
                int valeur;
                if (!cellule.empty()) {
                    valeur = std::stoi(cellule);
                }
                else {
                    valeur = 60;
                }
                data_map[id][heure] = valeur;
            }
        }
    }

    fichier.close();

    return data_map;
}

/**
 * Adds information on waiting times to the attraction in the dataset
 *
 * @param The attraction data map
 * @param The wait time data map (from attractionsOpenCSV function)
 *
 */
static void inputWaitTime(std::map<int, attraction>& attractions, std::map<int, std::map<int, int>>& wait_time_data) {
    for (auto& entry : wait_time_data) {
        int attraction_id = entry.first;
        //if (std::find(current_setting.full_ID_list.begin(), current_setting.full_ID_list.end(), attraction_id) != current_setting.full_ID_list.end()) {
        auto attr = attractions.find(attraction_id);
        if (attr != attractions.end()) {
            attr->second.wait_time = entry.second;
        }
        else {
            std::cerr << "Erreur : Aucune attraction correspondante trouvée pour l'ID : " << attraction_id << std::endl;
        }
        //}
    }

}

////////////////////////
//-returned-functions-//
////////////////////////

std::string DATA_LINK_JSON = "data/data.json";
std::string DATA_LINK_CSV = "data/waiting_time.csv";

/**
 * The grouping of all the functions used to obtain the attractions map.
 *
 * @return the attractions map
 *
 */
std::map <int, attraction>getAttractionData(std::string& link_to_data) {

    std::vector<attraction> attractions_vector = attractionsOpenJson(link_to_data);
    std::map <int, attraction> attraction_data = attractionVectorToMapById(attractions_vector);


    std::vector<intersection> intersections_vector = intersectionOpenJson(link_to_data, attraction_data);
    std::map <int, intersection> intersection_data = intersectionVectorToMapById(intersections_vector);
    addIntersectionDatatoAttraction(intersection_data, attraction_data);

    std::map<int, std::map<int, int>> wait_time_data = attractionsOpenCSV(DATA_LINK_CSV);
    inputWaitTime(attraction_data, wait_time_data);

    return attraction_data;
}

/**
 * The grouping of all the functions used to obtain the hotels map.
 *
 * @return the hotels map
 *
 */
std::map <int, hotel>getHotelData(std::string& link_to_data) {
    std::vector<hotel> hotels_vector = hotelOpenJson(link_to_data);
    std::map <int, hotel> hotel_data = hotelVectorToMapById(hotels_vector);

    std::map <int, attraction> attraction_data = getAttractionData(link_to_data);
    std::vector<intersection> intersections_vector = intersectionOpenJson(link_to_data, attraction_data);
    std::map <int, intersection> intersection_data = intersectionVectorToMapById(intersections_vector);
    addIntersectionDatatoHotel(intersection_data, hotel_data);

    return hotel_data;
}

/**
 * The grouping of all the functions used to obtain the intersections map.
 *
 * @return the intersections map
 *
 */
std::map <int, intersection> getIntersectionData(std::string& link_to_data) {
    std::map <int, attraction> attraction_data = getAttractionData(link_to_data);
    std::vector<intersection> intersections_vector = intersectionOpenJson(link_to_data, attraction_data);
    std::map <int, intersection> intersection_data = intersectionVectorToMapById(intersections_vector);
    std::map  <int, hotel> hotel_data = getHotelData(link_to_data);
    intersection hotel_intersection;
    hotel hotel_to_add = hotel_data[current_setting.hotel_ID];
    hotel_intersection.ID = 0;
    hotel_intersection.name = hotel_to_add.name;
    hotel_intersection.intersection_linked = hotel_to_add.intersection_linked;
    hotel_intersection.location = hotel_to_add.location;

    intersection_data[0] = hotel_intersection;

    return intersection_data;
}
