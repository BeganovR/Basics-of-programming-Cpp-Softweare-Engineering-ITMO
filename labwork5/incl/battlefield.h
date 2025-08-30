#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "game_info.h"

class BattleField {
   private:
    uint64_t width;
    uint64_t height;
    std::string shot_result;
    std::string result_match;
    struct Ship {
        int length;
        char type;
        uint64_t x, y;
    };
    std::vector<Ship> ships;

   public:
    std::vector<std::vector<char>> battlefield;

    BattleField(uint64_t width, uint64_t height);

    //================shoot===============
    std::string Shoot(uint64_t x, uint64_t y,
                      std::vector<std::vector<char>>& enemy_battlefield, GameInfo& game_info);
    //====================================

    //================finish===============
    bool CheckWin(std::vector<std::vector<char>>& enemy_battlefield);
    bool CheckLose();
    bool CheckFinished(std::vector<std::vector<char>>& enemy_battlefield);
    //====================================

    //=================put================
    bool PutShip(int length, char type, uint64_t x, uint64_t y,
                 std::vector<std::vector<char>>& target_field);
    //====================================

    //==============other==============
    bool IsShipSunk(uint64_t x, uint64_t y,
                    std::vector<std::vector<char>>& enemy_battlefield);

    void PrintBattlefield(std::vector<std::vector<char>>& enemy_battlefield);
    //=================================

    //===============io================
    bool SaveToFile(const std::string& filename);
    bool LoadFromFile(const std::string& filename, const std::string player,
                      GameInfo& game_info,
                      std::vector<std::vector<char>>& battlefield);
    //=================================

    // =================geters================
    uint64_t GetWidth() const { return width; }
    uint64_t GetHeight() const { return height; }
    //========================================
};