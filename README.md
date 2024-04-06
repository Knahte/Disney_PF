# Disney Path Finder

The Disney Path Finder project aims to provide a tool for generating the most optimal route for a typical day at Disneyland Paris. This application is designed for visitors who want to efficiently navigate the theme park, making the most of their time and minimizing walking distances between attractions.

This tool will help you plan your itinerary, prioritize attractions, and customize your experience based on your preferences. With features like customizable simulation settings, single rider mode, and the ability to select starting points such as hotels or stations, users have full control over their Disneyland adventure.


## License

[![License: CC BY 4.0](https://img.shields.io/badge/License-CC%20BY%204.0-lightgrey.svg)](https://creativecommons.org/licenses/by/4.0/)
## Compatibility

![Windows 10/11](https://img.shields.io/badge/Windows%2010%2F11-tested-brightgreen)
![Linux (wine)]([https://img.shields.io/badge/Linux-not%20tested-red](https://img.shields.io/badge/Windows%2010%2F11-tested-brightgreen))
![MacOS](https://img.shields.io/badge/MacOS-not%20tested-red)
## Screenshots

![App Screenshot](https://raw.githubusercontent.com/Knahte/Disney_PF/main/Image/App_Screenshot.png)


## How does it work

In my project, I use the Lin–Kernighan heuristic to generate optimal paths for a typical day at DisneyLand Paris. The algorithm iteratively refines the path by swapping pairs of locations to improve the overall distance traveled.

![Algorithm Screenshot](https://raw.githubusercontent.com/Knahte/Disney_PF/main/Image/PF_Algorithm_Screenshot.png)

In this example, the original path is A-B-C-D with a journey time of 56 minutes. The point to be exchanged is chosen at random (here it is B). The algorithm will test B-A-C-D, A-B-C-D, A-C-B-D, A-D-C-B; it will calculate the time taken for each of these paths and choose the fastest to update the original path (here B-A-C-D is the fastest at 41min). It will repeat this process with the best path, and finally obtain a path that is relatively close to the optimal solution. 

Additionally, we employ Dijkstra's algorithm to efficiently find the shortest paths between locations and then put it in our distance matrix where each intersection or attraction is represented as a node, and it's the distances between them that are stored in this matrix. 

Furthermore, in calculating the time required for each path, we take into account various factors such as walking speed and wait times at attractions. The distance come directly from the generated distance matrix. The time taken can be found using this formula :

`total_time(h) = distance(m) / walking_speed(km/h) * (1000/60) + waiting_time(min) * 60`

If you want to explore my code the interesting parts are in [cMain.cpp](https://github.com/Knahte/Disney_PF/blob/main/Generating_path.cpp) and [Generating_path.cpp](https://github.com/Knahte/Disney_PF/blob/main/Generating_path.cpp)


## Roadmap

- Enhance generation quality and speed through optimization techniques.

- Incorporate restaurant data into the path generation process to provide more comprehensive itineraries.

- Improve the aesthetics and user experience of the graphical user interface (GUI) to enhance usability and appeal.

- Implement dynamic waiting times based on daily fluctuations and factors influencing crowd levels.


## Acknowledgements

- [nlohmann/json](https://github.com/nlohmann/json): A huge thank you to the team behind the nlohmann/json library for their exceptionally clear and powerful code. It greatly facilitated JSON data processing in this project.

- [wxWidgets](https://www.wxwidgets.org/): I want to express my gratitude to all the developers of wxWidgets for their outstanding work on this library. Their efforts have enabled the creation of a rich and intuitive user interface for this application.

- [queue-times.com](https://queue-times.com/parks/4): Special thanks to queue-times.com for providing valuable data regarding wait times at all attractions, which has greatly improved the accuracy of this application in terms of time (a somewhat important aspect). 
In addition, the attraction IDs are based on their IDs to facilitate the integration of their API in the future.

I am grateful to these open-source projects and their teams for making this project possible.




## 🚀 About Me
I am a French dedicated high school student with a passion for computer science, constantly seeking new challenges and opportunities to expand my skills. I devote a significant amount of time to honing my skills in programming languages such as C++ and Python. I undertake various projects in these languages, with the aim of deepening my understanding and skills in software development.
