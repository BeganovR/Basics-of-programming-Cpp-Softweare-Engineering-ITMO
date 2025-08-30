#include "../incl/battlefield.h"

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

//==============Сonstructor===============
BattleField::BattleField(uint64_t width, uint64_t height)
    : width(width),
      height(height),
      battlefield(height, std::vector<char>(width, '.')) {}
//========================================
//============Check kill/not==============
bool BattleField::IsShipSunk(
    uint64_t x, uint64_t y, std::vector<std::vector<char>>& enemy_battlefield) {
    uint64_t start_x = x;
    while (start_x > 0 && (enemy_battlefield[y][start_x - 1] == 'S' ||
                           enemy_battlefield[y][start_x - 1] == 'X')) {
        --start_x;
    }
    uint64_t end_x = x;
    while (end_x < enemy_battlefield[0].size() - 1 &&
           (enemy_battlefield[y][end_x + 1] == 'S' ||
            enemy_battlefield[y][end_x + 1] == 'X')) {
        ++end_x;
    }
    for (uint64_t i = start_x; i <= end_x; ++i) {
        if (enemy_battlefield[y][i] == 'S') {
            return false;
        }
    }

    uint64_t start_y = y;
    while (start_y > 0 && (enemy_battlefield[start_y - 1][x] == 'S' ||
                           enemy_battlefield[start_y - 1][x] == 'X')) {
        --start_y;
    }
    uint64_t end_y = y;
    while (end_y < enemy_battlefield.size() - 1 &&
           (enemy_battlefield[end_y + 1][x] == 'S' ||
            enemy_battlefield[end_y + 1][x] == 'X')) {
        ++end_y;
    }
    for (uint64_t i = start_y; i <= end_y; ++i) {
        if (enemy_battlefield[i][x] == 'S') {
            return false;
        }
    }

    return true;
}
//========================================
//===============shoot================
std::string BattleField::Shoot(
    uint64_t x, uint64_t y, std::vector<std::vector<char>>& enemy_battlefield, GameInfo& game_info) {
    if (x >= width || y >= height) {
        throw std::invalid_argument("Shot coordinates are out of bounds.");
    }

    if (enemy_battlefield[y][x] == 'S') {
        enemy_battlefield[y][x] = 'X';
        if (game_info.last_shot_result == "kill") { //!!!!!!!!!!!!!!!!!!!
            shot_result = "kill";
        } else if (game_info.last_shot_result == "hit") {
            shot_result = "hit";
        }

    } else if (enemy_battlefield[y][x] == '.') {
        enemy_battlefield[y][x] = '*';
        shot_result = "miss";
    } else {
        shot_result = "already shot";
    }

    return shot_result;
}
//====================================

//===============checking win/lose===============
// Check win
bool BattleField::CheckWin(std::vector<std::vector<char>>& enemy_battlefield) {
    for (uint64_t y = 0; y < height; ++y) {
        for (uint64_t x = 0; x < width; ++x) {
            if (enemy_battlefield[y][x] == 'S') {
                return false;
            }
        }
    }
    result_match = "win";
    std::cout << "You win!" << '\n';
    return true;
}

// Check lose
bool BattleField::CheckLose() {
    for (uint64_t y = 0; y < height; ++y) {
        for (uint64_t x = 0; x < width; ++x) {
            if (battlefield[y][x] == 'S') {
                return false;
            }
        }
    }
    std::cout << "You lose!" << '\n';
    result_match = "lose";
    return true;
}

// Check finished
bool BattleField::CheckFinished(
    std::vector<std::vector<char>>& enemy_battlefield) {
    if (CheckWin(enemy_battlefield) || CheckLose()) {
        std::cout << "Finished!" << '\n';
        result_match = "finished";
        return true;
    }
    return false;
}
//================================================

//===============put===============
bool BattleField::PutShip(int length, char type, uint64_t x, uint64_t y,
                          std::vector<std::vector<char>>& target_field) {
    if (type != 'h' && type != 'v') {
        std::cerr << '\n' << "Invalid type of ship!" << '\n' << '\n';
        return false;
    }

    if (x >= width || y >= height) {
        std::cerr << '\n' << "Coordinates are out of bounds!" << '\n' << '\n';
        return false;
    }

    if ((type == 'h' && x + length > width) ||
        (type == 'v' && y + length > height)) {
        std::cerr << '\n' << "Coordinates are out of bounds!" << '\n' << '\n';
        return false;
    }

    for (uint64_t i = 0; i < length; ++i) {
        uint64_t nx = (type == 'h') ? x + i : x;
        uint64_t ny = (type == 'h') ? y : y + i;

        if (target_field[ny][nx] != '.') {
            std::cerr << '\n' << "Invalid ship placement!" << '\n' << '\n';
            return false;
        }
    }

    // Put the ship
    for (uint64_t i = 0; i < length; ++i) {
        uint64_t nx = (type == 'h') ? x + i : x;
        uint64_t ny = (type == 'h') ? y : y + i;

        target_field[ny][nx] = 'S';
    }

    Ship new_ship = {length, type, x, y};
    ships.push_back(new_ship);
    return true;
}
//=================================

// Print battlefield
void BattleField::PrintBattlefield(
    std::vector<std::vector<char>>& enemy_battlefield) {
    int width = battlefield[0].size();
    int height = battlefield.size();

    std::cout << "Your Battlefield:               Enemy's Battlefield:"
              << std::endl;

    std::cout << "     ";
    for (int i = 0; i < width; ++i) {
        std::cout << std::setw(2) << i << " ";
    }
    std::cout << "     ";
    for (int i = 0; i < width; ++i) {
        std::cout << std::setw(2) << i << " ";
    }
    std::cout << std::endl;

    for (int y = 0; y < height; ++y) {
        std::cout << std::setw(2) << y << " ";
        for (int x = 0; x < width; ++x) {
            std::cout << std::setw(2) << battlefield[y][x] << " ";
        }

        std::cout << "     " << std::setw(2) << y << " ";
        for (int x = 0; x < width; ++x) {
            if (enemy_battlefield[y][x] == 'X' ||
                enemy_battlefield[y][x] == '*') {
                std::cout << std::setw(2) << enemy_battlefield[y][x] << " ";
            } else {
                std::cout << std::setw(2) << '.' << " ";
            }
        }
        std::cout << std::endl;
    }
}

//=====================io=====================
bool BattleField::SaveToFile(const std::string& filename) {
    std::ofstream out_file(filename);
    if (!out_file.is_open()) {
        std::cerr << "Failed to open file!" << '\n';
        return false;
    }

    out_file << width << " " << height << '\n';

    for (size_t i = 0; i < ships.size(); ++i) {
        const Ship& ship = ships[i];
        out_file << ship.length << " " << ship.type << " " << ship.x << " "
                 << ship.y << '\n';
    }

    out_file.close();
    std::cout << "Saved successfully!" << '\n';
    return true;
}

bool BattleField::LoadFromFile(const std::string& filename,
                               const std::string player, GameInfo& game_info,
                               std::vector<std::vector<char>>& battlefield) {
    std::ifstream in_file(filename);
    if (!in_file.is_open()) {
        std::cerr << "Failed to open file!" << '\n';
        return false;
    }

    if (player == "master") {
        if (game_info.width == 0 || game_info.height == 0) {
            if (!(in_file >> game_info.width >> game_info.height) ||
                game_info.width <= 0 || game_info.height <= 0) {
                std::cerr << "Failed to read valid width and height!" << '\n';
                return false;
            }
        }
        width = game_info.width;
        height = game_info.height;
    } else if (player == "slave") {
        if (game_info.width == 0 || game_info.height == 0) {
            std::cerr
                << "Field dimensions must be set before loading for slave!"
                << '\n';
            return false;
        }
        width = game_info.width;
        height = game_info.height;
    } else {
        std::cerr << "Unknown player type!" << '\n';
        return false;
    }

    if (!battlefield.empty()) {
        battlefield.clear();
    }

    battlefield = std::vector<std::vector<char>>(
        game_info.height, std::vector<char>(game_info.width, '.'));
    std::cout << "Battlefield initialized: width=" << width
              << ", height=" << height << '\n';

    std::vector<std::pair<uint64_t, uint64_t>> occupied_coords;

    uint64_t length;
    char orientation;
    uint64_t x, y;

    while (in_file >> length >> orientation >> x >> y) {
        bool valid_placement = true;

        for (uint64_t i = 0; i < length; ++i) {
            uint64_t nx = (orientation == 'h') ? x + i : x;
            uint64_t ny = (orientation == 'h') ? y : y + i;

            if (nx >= width || ny >= height) {
                std::cerr << "Invalid ship placement: out of bounds!\n";
                valid_placement = false;
                break;
            }

            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    uint64_t adj_x = nx + dx;
                    uint64_t adj_y = ny + dy;

                    if (adj_x < width && adj_y < height &&
                        std::find(occupied_coords.begin(),
                                  occupied_coords.end(),
                                  std::make_pair(adj_x, adj_y)) !=
                            occupied_coords.end()) {
                        std::cerr
                            << "Invalid ship placement: ships are too close!\n";
                        valid_placement = false;
                        break;
                    }
                }
                if (!valid_placement) break;
            }
        }

        if (!valid_placement) {
            std::cerr << "Failed to place ship: " << length << " "
                      << orientation << " " << x << " " << y << '\n';
            return false;
        }

        if (PutShip(length, orientation, x, y, battlefield)) {
            for (uint64_t i = 0; i < length; ++i) {
                uint64_t nx = (orientation == 'h') ? x + i : x;
                uint64_t ny = (orientation == 'h') ? y : y + i;
                occupied_coords.emplace_back(nx, ny);
            }
        } else {
            std::cerr << "Failed to place ship: " << length << " "
                      << orientation << " " << x << " " << y << '\n';
            return false;
        }
    }

    in_file.close();
    std::cout << "Loaded successfully!" << '\n';
    return true;
}

//============================================
