#include "cache_func.h"

void SaveToFile(std::string& filename, const std::string& data) {
    std::ofstream file;
    file.open(filename, std::ios::app);

    if (!file.is_open()) {
        std::cerr << "File could not be opened!\n";
        return;
    }

    file << data << '\n';
    file.close();
}

bool CheckiInFile(const std::string& filename, std::string& data) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "File could not be opened!\n";
        return false;
    }

    std::string line;
    bool found = false;
    while (std::getline(file, line)) {
        if (line.find(data) != std::string::npos) {
            found = true;
            break;
        }
    }

    if (found) {
        return true;
    } else {
        return false;
    }
}

void DeleteFromFile(std::string& filename) {
    std::ofstream file;
    file.open(filename, std::ofstream::out | std::ios::trunc);
    file.close();
}

