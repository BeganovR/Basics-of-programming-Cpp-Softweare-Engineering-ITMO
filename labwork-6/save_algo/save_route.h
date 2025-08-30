#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "../cache/cache_func.h"
using json = nlohmann::json;

struct Route {
    std::string from;
    std::string to;
    std::string departure;
    std::string arrival;
    std::string thread_title;
    std::string thread_number;
    std::string transport_type;
    bool is_transfer;
    int price;
    int transfer_count;
};

std::vector<Route> ParseJson(const json& data);
std::string GetStationName(const json& segment, const std::string& type);
std::string GetTransportInfo(const json& segment);
int GetSegmentPrice(const json& segment);
Route CreateSingleRoute(const json& segment);
void PrintRoutes(const std::vector<Route>& routes, std::string& filename);
