#pragma once

#include <cpr/cpr.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

class ApiManager {
   public:
    ApiManager();
    nlohmann::json Search(const std::string& from, const std::string& to,
                          const std::string& date);

   private:
    const std::string api_key_ = "6b43fa85-ea9d-45a3-b2ce-640e6523d827";
    const std::string url = "https://api.rasp.yandex.net/v3.0/search/";
};
