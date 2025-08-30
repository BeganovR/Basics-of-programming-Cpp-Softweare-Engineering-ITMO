#pragma once

#include "strategy.h"

class StrategyManager {
   public:
    static Strategy* GetStrategy();
    static void SetStrategy(Strategy* strategy);

   private:
    GameInfo game_info;  // Maybe need
    static Strategy* current_strategy;
};