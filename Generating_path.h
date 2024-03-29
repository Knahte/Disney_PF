#pragma once

#include "import_data.h"

std::vector<int> generatePath(setting& current_setting, int number_of_generation, int number_path);
void redirectOutputToFile(const std::string& filename);
void restoreOutput();
double getTimeTaken(double distance, double current_time, int attr_ID, setting& current_setting);
double findShortestPath(intersection& start_intersection, intersection& end_intersection);
void pathToGPX(const std::vector<int>& path, const std::string& name, setting& current_setting);
extern std::map<int, attraction> attraction_data;
extern std::map<int, intersection> intersection_data;
extern std::map<int, hotel> hotel_data;