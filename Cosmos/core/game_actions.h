// game_actions.h
#pragma once
#include "game_world.h"
#include "combat_system.h"
#include "../ui/console_ui.h"
#include <string>
#include <random>
#include <iostream> // for std::cin

class GameActions {
public:
    static void handleRoomAction(Player& player, GameWorld& world);
    static void moveToNextLocation(Player& player, GameWorld& world);
    static void processEffect(Player& player, const std::string& itemName,
        const std::vector<Effect>& effects);
    static bool checkActionSuccess(const Player& player, const std::string& statName);

private:
    static void showActionResults(Player& player, const Action& action,
        bool success, const FloorData& floor);
};