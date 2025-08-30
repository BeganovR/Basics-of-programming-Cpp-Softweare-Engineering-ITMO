#include <iostream>
#include <string>

#include "incl/battlefield.h"
#include "incl/game_info.h"
#include "incl/strategy.h"
#include "incl/strategy_manager.h"

void PrintHelp() {
    std::cout << "Available commands:\n";
    std::cout << "  create master/slave - Create a game as master or slave\n";
    std::cout << "  start - Start the game\n";
    std::cout << "  stop - Stop the game\n";
    std::cout << "  set width <value> - Set the width of the battlefield\n";
    std::cout << "  set height <value> - Set the height of the battlefield\n";
    std::cout << "  set count <type> <count> - Set the count of ships of a "
                 "specific type\n";
    std::cout << "  set strategy ordered/custom - Set the strategy\n";
    std::cout << "  get width - Get the width of the battlefield\n";
    std::cout << "  get height - Get the height of the battlefield\n";
    std::cout
        << "  get count <type> - Get the count of ships of a specific type\n";
    std::cout << "  dump <filename> - Save the battlefield to a file\n";
    std::cout << "  load <filename> - Load the battlefield from a file\n";
    std::cout << "  shot <x> <y> - Shoot at the specified coordinates\n";
    std::cout << "  shot - Execute the strategy's shot\n";
    std::cout << "  finished - Check if the game is finished\n";
    std::cout << "  win - Check if you have won\n";
    std::cout << "  lose - Check if you have lost\n";
    std::cout << "  help - Show this help message\n";
    std::cout << "  exit - Exit the program\n";
}

int main() {
    GameInfo game_info;
    BattleField* battlefield = nullptr;
    BattleField* enemy_battlefield = nullptr;
    StrategyManager::SetStrategy(
        new CustomStrategy(*battlefield, *enemy_battlefield, game_info));
    game_info.strategy = "custom";
    while (true) {
        std::string cmd;
        std::getline(std::cin, cmd);

        size_t pos = cmd.find(' ');
        std::string command = cmd.substr(0, pos);
        std::string args =
            (pos == std::string::npos) ? "" : cmd.substr(pos + 1);

        if (command == "exit") {
            delete battlefield;
            delete enemy_battlefield;
            delete StrategyManager::GetStrategy();
            return 0;
        } else if (command == "ping") {
            std::cout << "pong" << '\n';
        } else if (command == "create") {
            if (args == "master") {
                std::cout << "ok" << '\n';
                game_info.opponent = "master";
                game_info.player = "slave";
                game_info.started = true;  // For starting game
            } else if (args == "slave") {
                std::cout << "ok" << '\n';
                game_info.opponent = "slave";
                game_info.player = "master";
                game_info.started = true;  // For starting game
            } else {
                std::cerr << '\n' << "Invalid name of player!" << '\n' << '\n';
            }
            if (game_info.player == "slave") {
                // For testing
                game_info.width = 10;
                game_info.height = 10;
                game_info.count[0] = 2;
                game_info.count[1] = 2;
                game_info.count[2] = 0;
                game_info.count[3] = 0;

            /*  For random values              
                game_info.height = 5 + rand() % (15 - 5 + 1);
                game_info.width = 5 + rand() % (15 - 5 + 1);
                game_info.count[0] = 0 + rand() % (5  + 1);
                game_info.count[1] = 0 + rand() % (5  + 1);
                game_info.count[2] = 0 + rand() % (5  + 1);
                game_info.count[3] = 0 + rand() % (5 +1 );
            */
            }
        } else if (command == "start") {
            if (game_info.started && !game_info.end_game) {
                std::cout << "ok" << '\n';
            }

            if (!battlefield) {
                battlefield =
                    new BattleField(game_info.width, game_info.height);
            }
            if (!enemy_battlefield) {
                enemy_battlefield =
                    new BattleField(game_info.width, game_info.height);
            }

            // Who is first
            if (game_info.player == "master") {
                if (game_info.load_from_file) {
                    if (battlefield->LoadFromFile(game_info.file_load,
                                                  game_info.player, game_info,
                                                  battlefield->battlefield)) {
                        std::cout << "ok" << '\n';
                    } else {
                        std::cerr << "Failed to load battlefield from file!"
                                  << '\n';
                    }
                }

                if (!PlaceEnemyShips(*enemy_battlefield,
                                     game_info.count)) {  // From strategy.cpp
                    std::cerr << "Failed to place enemy ships!" << '\n';
                } else {
                    std::cout << "Enemy ships placed successfully." << '\n';
                }
            } else if (game_info.player == "slave") {
                if (!PlaceEnemyShips(*enemy_battlefield,
                                     game_info.count)) {  // From strategy.cpp
                    std::cerr << "Failed to place enemy ships!" << '\n';
                } else {
                    std::cout << "Enemy ships placed successfully." << '\n';
                }

                if (game_info.load_from_file) {
                    if (battlefield->LoadFromFile(game_info.file_load,
                                                  game_info.player, game_info,
                                                  battlefield->battlefield)) {
                        std::cout << "ok" << '\n';
                    } else {
                        std::cerr << "Failed to load battlefield from file!"
                                  << '\n';
                    }
                }
            }

            // Set strategy
            if (game_info.strategy == "ordered") {
                StrategyManager::SetStrategy(
                    new OrderedStrategy(*battlefield, *enemy_battlefield, game_info));
            } else {
                StrategyManager::SetStrategy(
                    new CustomStrategy(*battlefield, *enemy_battlefield, game_info));
            }

            // Print battlefield
            battlefield->PrintBattlefield(enemy_battlefield->battlefield);
            continue;
        } else if (command == "stop") {
            std::cout << "ok" << '\n';
            game_info.end_game = true;
            delete battlefield;
            delete enemy_battlefield;
            battlefield = nullptr;
            enemy_battlefield = nullptr;

            // SET ARGUMENTS
        } else if (command == "set" && game_info.started) {
            size_t first_space = args.find(' ');
            if (first_space == std::string::npos) {
                std::cerr << '\n' << "failed" << '\n';
            }

            std::string key = args.substr(0, first_space);
            std::string value = args.substr(first_space + 1);

            if (key == "width" && game_info.player == "master") {
                if (std::stoull(value) > 0) {
                    game_info.width = std::stoull(value);
                    std::cout << "ok" << '\n';
                } else {
                    std::cerr << '\n'
                              << "Argument must be positive. Failed" << '\n';
                }
            } else if (key == "height" && game_info.player == "master") {
                if (std::stoull(value) > 0) {
                    game_info.height = std::stoull(value);
                    std::cout << "ok" << '\n';
                } else {
                    std::cerr << '\n'
                              << "Argument must be positive. Failed" << '\n';
                }

                // COUNT
            } else if (key == "count" && game_info.player == "master") {
                size_t second_space = value.find(' ');
                if (second_space == std::string::npos) {
                    std::cerr << "failed" << '\n' << '\n';
                }

                std::string type_str = value.substr(0, second_space);
                std::string count_str = value.substr(second_space + 1);

                int type = std::stoull(type_str);
                uint64_t count = std::stoull(count_str);

                if (type >= 1 && type <= 4 && count > 0) {
                    game_info.count[type - 1] = count;
                    std::cout << "ok" << '\n';
                } else {
                    std::cerr << '\n'
                              << "Argument must be positive. Failed" << '\n'
                              << '\n';
                }
            } else if (key == "strategy") {
                if (value == "ordered") {
                    std::cout << "ok" << '\n';
                    game_info.strategy = value;
                } else if (value == "custom") {
                    std::cout << "ok" << '\n';
                    game_info.strategy = value;
                }
            } else if (key == "result" && game_info.enemy_shot_done == true) {
                if (value == "hit") {
                    game_info.last_shot_result = "hit";
                } else if (value == "miss") {
                    game_info.last_shot_result = "miss";
                } else if (value == "kill") {
                    game_info.last_shot_result = "kill";
                } else {
                    std::cerr << '\n' << "Invalid argument. Failed" << '\n'
                              << '\n';
                }
            
            } else {
                std::cerr << '\n' << "Invalid argument. Failed" << '\n' << '\n';
            }

            // GET ARGUMENTS
        } else if (command == "get" && game_info.started) {
            size_t first_space = args.find(' ');

            std::string key = args.substr(0, first_space);

            if (key == "width") {
                std::cout << game_info.width << '\n';
            } else if (key == "height") {
                std::cout << game_info.height << '\n';
            } else if (key == "count") {
                std::string value = args.substr(first_space + 1);
                int type = std::stoi(value);

                if (type >= 1 && type <= 4) {
                    std::cout << game_info.count[type - 1] << '\n';
                } else {
                    std::cerr << '\n'
                              << "Argument must be positive. Failed" << '\n'
                              << '\n';
                }
            }

        } else if (command == "dump" && game_info.started) {
            if (battlefield && !args.empty()) {
                game_info.file_dump = args;
                std::cout << "ok" << '\n';
                battlefield->SaveToFile(game_info.file_dump);
            } else {
                std::cerr << '\n' << "Failed to dump" << '\n' << '\n';
            }

        } else if (command == "load" && game_info.started) {
            if (!args.empty()) {
                game_info.load_from_file = true;
                game_info.file_load = args;
                if (!battlefield) {
                    battlefield =
                        new BattleField(game_info.width, game_info.height);
                }

                if (!battlefield->LoadFromFile(game_info.file_load,
                                               game_info.player, game_info,
                                               battlefield->battlefield)) {
                    std::cerr << "Failed to load battlefield from file!"
                              << '\n';
                } else {
                    std::cout << "ok" << '\n';
                }
            } else {
                game_info.load_from_file = false;
                std::cerr << '\n'
                          << "File name is empty. Failed to load!" << '\n'
                          << '\n';
            }

        } else if (command == "shot" && game_info.started) {
            if (args == "") {  // EnemyShoot
                if (!battlefield || !enemy_battlefield) {
                    std::cerr << "Battlefields are not initialized!" << '\n';
                    continue;
                }
                Strategy* strategy = StrategyManager::GetStrategy();
                if (strategy) {
                    strategy->Execute();
                }
                battlefield->PrintBattlefield(enemy_battlefield->battlefield);
                game_info.enemy_shot_done = true;
            } else if (game_info.enemy_shot_done == true && args != "") {
                size_t first_space = args.find(' ');
                if (first_space == std::string::npos) {
                    std::cerr << "failed" << '\n';
                    continue;
                }
                std::string x_str = args.substr(0, first_space);
                std::string y_str = args.substr(first_space + 1);
                uint64_t x = std::stoull(x_str);
                uint64_t y = std::stoull(y_str);
                std::string result =
                    battlefield->Shoot(x, y, enemy_battlefield->battlefield, game_info);
                battlefield->PrintBattlefield(enemy_battlefield->battlefield);
                std::cout << '\n' << result << '\n';
                game_info.enemy_shot_done = false;
            }
        } else if (command == "finished") {
            std::cout << (battlefield && battlefield->CheckFinished(
                                             enemy_battlefield->battlefield))
                      << '\n';

        } else if (command == "win") {
            std::cout << (battlefield &&
                          battlefield->CheckWin(enemy_battlefield->battlefield))
                      << '\n';

        } else if (command == "lose") {
            std::cout << (battlefield && battlefield->CheckLose()) << '\n';

        } else if (command == "help") { 
            PrintHelp();
        } else {
            std::cerr
                << '\n'
                << "You can't start game! /\nYou need to create game first! /\n"
                << "If you slave you can't set parametrs! /" << '\n'
                << "Unknown command!" << '\n'
                << "Type 'help' to see available commands." << '\n'
                << '\n';
        }
    }

    return 0;
}
