#pragma once
#include <string>

#include "../core/game_objects.h"

namespace ui {
  void ClearScreen();
  void InitConsole();
  void PrintColored(const std::string& text, int color);
  void PrintMultiline(const std::string& text, int color,
    bool is_action_description = false);
  void PrintRoomDescription(const Room& room);
  void PrintAvailableActions(Player& player,
    const std::vector<std::string>& action_ids,
    const std::map<std::string, Action>& action_map);
  void PrintPlayerStats(const Player& player);
  void WaitForEnter(const std::string& message = "");
  void ShowGameIntro();
  void ShowGameEnding(bool success);
  void ShowRoomActions(const std::vector<std::string>& actions);
}