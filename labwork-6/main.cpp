#include <iostream>
#include <vector>

#include "cache/cache_func.h"
#include "main_p/api_manager.h"
#include "save_algo/save_route.h"

bool IsSegmentsKey(const std::pair<const std::string, nlohmann::json>& item) {
    return item.first == "segments";
}

int main() {
    ApiManager api_manager;
    std::string from = "c2";  // Saint-Petersburg
    std::string to = "c25";   // Pskov
    std::string date;
    std::string filename;
    std::string answer;
    std::vector<std::string> dates;
    std::vector<Route> routes;
    std::vector<Route> routes2;

    while (true) {
        //-----1-----
        std::cout << "What is date of your trip to Pskov? YYYY-MM-DD: ";
        std::cin >> date;
        std::cout << '\n';

        //-----2-----
        std::cout << "Name of file to save data: ";
        std::cin >> filename;
        std::cout << '\n';

        //-----Check for duplicates-----
        if (!dates.empty()) {
            std::cout << "First check: Memory\n";
            for (auto& date_v : dates) {
                if (date_v == date) {
                    std::cout << "You have already searched for this date!\n";
                    continue;
                }
            }
            std::cout << "Second check: File\n";
            if (CheckiInFile(filename, date)) {
                std::cout << "You have already searched for this date!\n";
                continue;
            } else {
                dates.push_back(date);
            }
        } else {
            dates.push_back(date);
        }

        //-----2.1-----
        nlohmann::json response = api_manager.Search(from, to, date);
        if (response.is_null()) {
            std::cout << "Failed to get route information. Please check your "
                         "internet connection and try again.\n";
            continue;
        }
        if (!response.contains("segments")) {
            std::cout << "No routes found for the specified date.\n";
            continue;
        }
        routes = ParseJson(response);
        if (routes.empty()) {
            std::cout << "No valid routes found for the specified date.\n";
            continue;
        }
        PrintRoutes(routes, filename);

        //-----2.2-----
        nlohmann::json response2 = api_manager.Search(to, from, date);
        if (response2.is_null()) {
            std::cout << "Failed to get route information. Please check your "
                         "internet connection and try again.\n";
            continue;
        }

        // begin 

        if (std::find_if(response2.begin(), response2.end(), IsSegmentsKey) ==
            response2.end()) {
            std::cout << "No routes found for the specified date.\n";
            continue;
        }
        
        // end

        routes2 = ParseJson(response2);
        if (routes2.empty()) {
            std::cout << "No valid routes found for the specified date.\n";
            continue;
        }
        PrintRoutes(routes2, filename);

        //-----3-----
        std::cout << "Do you want to clean this file? (y/n): ";
        std::cin >> answer;
        std::cout << '\n';
        if (answer == "y") {
            DeleteFromFile(filename);
            if (!dates.empty()) {
                dates.pop_back();
            }
        }

        //-----4-----
        std::cout << "Do you want to continue? (y/n): ";
        std::cin >> answer;
        std::cout << '\n';
        if (answer != "y") {
            break;
        }
    }
    return 0;
}