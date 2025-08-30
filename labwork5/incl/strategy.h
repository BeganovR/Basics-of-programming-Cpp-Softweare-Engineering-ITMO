#pragma once
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "../incl/battlefield.h"
#include "../incl/game_info.h"

class Strategy {
   public:
    virtual void Execute() = 0;
    virtual ~Strategy() = default;
};

class OrderedStrategy : public Strategy {
   private:
    BattleField& battlefield;
    BattleField& enemy_battlefield;
    GameInfo& game_info;
    uint64_t current_x = 0;
    uint64_t current_y = 0;

   public:
    OrderedStrategy(BattleField& enemy_battlefield, BattleField& battlefield,
                    GameInfo& game_info)
        : battlefield(battlefield),
          enemy_battlefield(enemy_battlefield),
          game_info(game_info),
          current_x(0),
          current_y(0) {}

    void Execute() override;
};

class CustomStrategy : public Strategy {
   private:
    BattleField& battlefield;
    BattleField& enemy_battlefield;
    GameInfo& game_info;
   public:
    CustomStrategy(BattleField& enemy_battlefield, BattleField& battlefield, GameInfo& game_info)
        : enemy_battlefield(enemy_battlefield), battlefield(battlefield), game_info(game_info) {}

    void Execute() override;
};

bool PlaceEnemyShips(BattleField& enemy_battlefield, const uint64_t count[4]);