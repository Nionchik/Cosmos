#pragma once
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <map>

#include "../ui/console_ui.h"
#include "combat_system.h"
#include "game_world.h"

class GameActions {
public:
  static void HandleRoomAction(Player& player, GameWorld& world);
  static void MoveToNextLocation(Player& player, GameWorld& world);
  static void ProcessEffect(Player& player, const std::string& item_name,
    const std::vector<Effect>& effects);
  static bool CheckActionSuccess(const Player& player,
    const std::string& stat_name);
  static void ResetRoomFirstVisit(Player& player, GameWorld& world);

private:
  static void ShowActionResults(Player& player, const Action& action,
    bool success, const FloorData& floor,
    GameWorld& world);
};