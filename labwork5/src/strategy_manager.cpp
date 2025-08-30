#include "../incl/strategy_manager.h"

Strategy* StrategyManager::current_strategy = nullptr;

Strategy* StrategyManager::GetStrategy() { return current_strategy; }

void StrategyManager::SetStrategy(Strategy* strategy) {
    current_strategy = strategy;
}