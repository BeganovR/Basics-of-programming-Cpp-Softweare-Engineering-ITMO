#include "../incl/strategy.h"

#include <cassert>
#include <iostream>
#include <string>

#include "../incl/battlefield.h"

//================ordered================
void OrderedStrategy::Execute() {
    if (current_y >= enemy_battlefield.GetHeight()) {
        std::cout << "OrderedStrategy: All possible shots are completed."
                  << '\n';
        return;
    }

    std::cout << "OrderedStrategy: shoots at: (" << current_x << ", "
              << current_y << ")\n";

    enemy_battlefield.Shoot(current_x, current_y,
                            enemy_battlefield.battlefield, game_info);
    ++current_x;
    if (current_x >= enemy_battlefield.GetWidth()) {
        current_x = 0;
        ++current_y;
    }
}

//======================================

//================custom================

void CustomStrategy::Execute() {
    static bool hit = false;
    static uint64_t last_x = 0;
    static uint64_t last_y = 0;

    uint64_t x, y;

    if (hit) {
        bool found = false;
        for (int dx = -1; dx <= 1 && !found; ++dx) {
            for (int dy = -1; dy <= 1 && !found; ++dy) {
                if (dx == 0 && dy == 0) continue;
                uint64_t nx = last_x + dx;
                uint64_t ny = last_y + dy;
                if (nx < enemy_battlefield.GetWidth() &&
                    ny < enemy_battlefield.GetHeight() &&
                    enemy_battlefield.battlefield[ny][nx] != 'X' &&
                    enemy_battlefield.battlefield[ny][nx] != '*') {
                    x = nx;
                    y = ny;
                    found = true;
                }
            }
        }
        if (!found) {
            hit = false;
        }  
    }

    if (!hit) {
        do {
            x = rand() % enemy_battlefield.GetWidth();
            y = rand() % enemy_battlefield.GetHeight();
        } while (enemy_battlefield.battlefield[y][x] == 'X' ||
                 enemy_battlefield.battlefield[y][x] == '*');
    }

    enemy_battlefield.Shoot(x, y, enemy_battlefield.battlefield, game_info);
    std::cout << "CustomStrategy: shoots at: (" << x << ", " << y << ')' << '\n';

    if (game_info.last_shot_result == "hit") {
        hit = true;
        last_x = x;
        last_y = y;
    } else {
        hit = false;
    }
}

bool PlaceEnemyShips(BattleField& enemy_battlefield, const uint64_t count[4]) {
    srand(static_cast<unsigned>(time(0)));

    for (int length = 1; length <= 4; ++length) {
        for (uint64_t i = 0; i < count[length - 1]; ++i) {
            bool placed = false;
            while (!placed) {
                uint64_t x = rand() % enemy_battlefield.GetWidth();
                uint64_t y = rand() % enemy_battlefield.GetHeight();
                bool horizontal =
                    rand() % 2 ==
                    0; 

                if (horizontal) {
                    if (x + length <= enemy_battlefield.GetWidth()) {
                        placed = enemy_battlefield.PutShip(
                            length, 'h', x, y, enemy_battlefield.battlefield);
                    }
                } else {
                    if (y + length <= enemy_battlefield.GetHeight()) {
                        placed = enemy_battlefield.PutShip(
                            length, 'v', x, y, enemy_battlefield.battlefield);
                    }
                }
            }
        }
    }
    return true;
}

//======================================
