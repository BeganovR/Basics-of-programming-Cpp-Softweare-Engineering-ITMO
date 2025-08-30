#pragma once
#include <string>

struct GameInfo {
    std::string player;
    std::string opponent;
    std::string strategy;
    bool started = false;
    bool end_game = false;
    bool load_from_file = false;
    bool enemy_shot_done = false;
    uint64_t width = 0;
    uint64_t height = 0;
    uint64_t count[4] = {0};
    std::string file_load = "";
    std::string file_dump = "";
    std::string last_shot_result = "";
};
