#include "api_manager.h"

ApiManager::ApiManager() {}

nlohmann::json ApiManager::Search(const std::string& from,
                                  const std::string& to,
                                  const std::string& date) {
    try {
        cpr::Response r = cpr::Get(cpr::Url{url},
                                   cpr::Parameters{{"apikey", api_key_},
                                                   {"from", from},
                                                   {"to", to},
                                                   {"date", date},
                                                   {"transfers", "true"},
                                                   {"limit", "50"}},
                                   cpr::Timeout{30000});

        if (r.status_code != 200) {
            std::cerr << "HTTP Error: " << r.status_code << "\n";
            std::cerr << "Error message: " << r.error.message << "\n";
            return nullptr;
        }

        if (r.text.empty()) {
            std::cerr << "Empty response from server\n";
            return nullptr;
        }

        auto json_response = nlohmann::json::parse(r.text);
        if (json_response.is_null()) {
            std::cerr << "Invalid JSON response\n";
            return nullptr;
        }

        return json_response;

    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << "\n";
        return nullptr;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return nullptr;
    }
}