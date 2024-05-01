#pragma once

#include "import_data.h"



/**
 * Generate a path through attractions using a genetic algorithm.
 *
 * @param current_setting The current settings.
 * @param number_of_generation The number of generations to run the genetic algorithm.
 * @param number_path The number of paths to generate initially.
 *
 * @return The shortest path found by the genetic algorithm.
 */
std::vector<int> generatePath(setting& current_setting, int number_of_generation, int number_path, std::string path_to_data);

/**
 * Redirects the standard output to a file.
 *
 * @param filename The name of the file to redirect output to.
 *
 */
void redirectOutputToFile(const std::string& filename);

/**
 * Restores standard output from the file.
 *
 */
void restoreOutput();

/**
 * Calculates the total time taken to travel to an attraction,
 * taking into account walking distance, waiting time, and current settings.
 *
 * @param distance The distance to the attraction (in meters).
 * @param current_time The current time (in hours).
 * @param attr_ID The ID of the attraction (the destination).
 * @param current_setting The settings object containing information about the current configuration.
 *
 * @return The total time taken to travel to the attraction (in hours).
 *
 */
double getTimeTaken(double distance, double current_time, int attr_ID, setting& current_setting);

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
double findShortestPath(intersection& start_intersection, intersection& end_intersection);

/**
 * Converts a given path to a GPX (GPS Exchange Format) file.
 *
 * @param path The path to be converted to GPX format.
 * @param name The file name (without the extension).
 * @param current_setting The settings object containing information about the current configuration.
 *
 * @return A GPX file in the "Output" folder.
 *
 */
void pathToGPX(const std::vector<int>& path, const std::string& name, setting& current_setting);


extern std::map<int, attraction> attraction_data; // A map providing access to all attractions using their IDs as keys.
extern std::map<int, intersection> intersection_data; // A map providing access to all intersections using their IDs as keys.
extern std::map<int, hotel> hotel_data; // A map providing access to all hotels using their IDs as keys.