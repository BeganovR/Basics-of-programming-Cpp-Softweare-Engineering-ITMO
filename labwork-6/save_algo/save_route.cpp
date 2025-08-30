#include "save_route.h"

std::vector<Route> ParseJson(const json& data) {
    std::vector<Route> routes;

    if (!data.contains("segments")) {
        std::cerr << "Invalid JSON format: no 'segments' field.\n";
        return routes;
    }

    for (const auto& segment : data["segments"]) {
        if (segment.contains("has_transfers") &&
            segment["has_transfers"].get<bool>()) {
            Route route;

            if (segment.contains("departure_from")) {
                route.from = GetStationName(segment["departure_from"], "");
            }
            if (segment.contains("arrival_to")) {
                route.to = GetStationName(segment["arrival_to"], "");
            }

            route.departure = segment.value("departure", "Unknown");
            route.arrival = segment.value("arrival", "Unknown");
            route.transport_type = "Combined";
            route.is_transfer = true;

            if (segment.contains("transfers")) {
                route.transfer_count = segment["transfers"].size();
            }

            std::string route_info = "Route with transfers:\n";
            int total_price = 0;
            int segment_num = 1;

            if (segment.contains("details")) {
                for (const auto& detail : segment["details"]) {
                    if (detail.contains("is_transfer") &&
                        detail["is_transfer"].get<bool>()) {
                        continue;
                    }

                    if (!detail.contains("thread") ||
                        !detail["thread"].contains("transport_type") ||
                        !detail.contains("from") || !detail.contains("to")) {
                        continue;
                    }

                    std::string transport_type =
                        detail["thread"]["transport_type"].get<std::string>();
                    std::string from = GetStationName(detail["from"], "");
                    std::string to = GetStationName(detail["to"], "");
                    std::string departure =
                        detail.value("departure", "Unknown");
                    std::string arrival = detail.value("arrival", "Unknown");

                    route_info += std::to_string(segment_num++) + ". " +
                                  transport_type + " " + from + " — " + to +
                                  " (" + departure + " -> " + arrival + ")\n";

                    if (detail.contains("tickets_info")) {
                        total_price += GetSegmentPrice(detail);
                    }
                }
            }

            route.thread_title = route_info;
            route.price = total_price;
            routes.push_back(route);
        } else {
            routes.push_back(CreateSingleRoute(segment));
        }
    }

    return routes;
}

std::string GetStationName(const json& segment, const std::string& type) {
    const json* station_data = nullptr;

    if (!type.empty() && segment.contains(type)) {
        station_data = &segment[type];
    } else {
        station_data = &segment;
    }

    if (station_data->contains("title") &&
        !(*station_data)["title"].is_null()) {
        return (*station_data)["title"].get<std::string>();
    }
    if (station_data->contains("popular_title") &&
        !(*station_data)["popular_title"].is_null()) {
        return (*station_data)["popular_title"].get<std::string>();
    }
    if (station_data->contains("short_title") &&
        !(*station_data)["short_title"].is_null()) {
        return (*station_data)["short_title"].get<std::string>();
    }

    return "Unknown";
}

std::string GetTransportInfo(const json& segment) {
    std::string transport_type =
        segment.contains("thread") &&
                segment["thread"].contains("transport_type")
            ? segment["thread"]["transport_type"].get<std::string>()
            : "Unknown";

    std::string title =
        segment.contains("thread") && segment["thread"].contains("title")
            ? segment["thread"]["title"].get<std::string>()
        : segment.contains("thread") &&
                segment["thread"].contains("short_title")
            ? segment["thread"]["short_title"].get<std::string>()
            : "Unknown";

    std::string departure = segment.value("departure", "Unknown");
    std::string arrival = segment.value("arrival", "Unknown");

    std::string from = GetStationName(segment["from"], "");
    std::string to = GetStationName(segment["to"], "");

    return transport_type + " " + from + " — " + to + " (" + departure +
           " -> " + arrival + ")";
}

int GetSegmentPrice(const json& segment) {
    if (segment.contains("tickets_info") &&
        segment["tickets_info"].contains("places") &&
        !segment["tickets_info"]["places"].empty() &&
        segment["tickets_info"]["places"][0].contains("price") &&
        segment["tickets_info"]["places"][0]["price"].contains("whole")) {
        return segment["tickets_info"]["places"][0]["price"]["whole"];
    }
    return 0;
}

Route CreateSingleRoute(const json& segment) {
    return Route{
        GetStationName(segment, "from"),
        GetStationName(segment, "to"),
        segment.value("departure", "Unknown"),
        segment.value("arrival", "Unknown"),
        segment.contains("thread") && segment["thread"].contains("title")
            ? segment["thread"]["title"]
            : "Unknown",
        segment.contains("thread") && segment["thread"].contains("number")
            ? segment["thread"]["number"]
            : "Unknown",
        segment.contains("thread") &&
                segment["thread"].contains("transport_type")
            ? segment["thread"]["transport_type"]
            : "Unknown",
        false,
        GetSegmentPrice(segment),
        0};
}

void PrintRoutes(const std::vector<Route>& routes, std::string& filename) {
    for (const auto& route : routes) {
        std::string message;
        if (route.is_transfer) {
            message =
                "=== Route with transfers ===\n"
                "From: " +
                route.from +
                "\n"
                "To: " +
                route.to +
                "\n"
                "Departure: " +
                route.departure +
                "\n"
                "Arrival: " +
                route.arrival +
                "\n"
                "Transfers: " +
                std::to_string(route.transfer_count) +
                "\n"
                "Total Price: " +
                (route.price != -1 ? std::to_string(route.price) + " RUB"
                                   : "Unknown") +
                "\n"
                "Details:\n" +
                route.thread_title + "\n";
        } else {
            message =
                "=== Direct Route ===\n"
                "From: " +
                route.from +
                "\n"
                "To: " +
                route.to +
                "\n"
                "Departure: " +
                route.departure +
                "\n"
                "Arrival: " +
                route.arrival +
                "\n"
                "Transport: " +
                route.transport_type + " (" + route.thread_title +
                ")\n"
                "Price: " +
                (route.price != -1 ? std::to_string(route.price) + " RUB"
                                   : "Unknown") +
                "\n";
        }
        message += "----------------------\n";
        SaveToFile(filename, message);
    }
}