//console_ui.h
#pragma once
#include "../core/game_objects.h"
#include <string>

namespace UI {
    void initConsole();
    void printColored(const std::string& text, int color);
    void printMultiline(const std::string& text, int color, bool isActionDescription = false);
    void printRoomDescription(const Room& room);
    void printAvailableActions(Player& player,
        const std::vector<std::string>& actionIds,
        const std::map<std::string, Action>& actionMap);
    void printPlayerStats(const Player& player);
    void waitForEnter();
    void showGameIntro();
    void showGameEnding(bool success);
    void showRoomActions(const std::vector<std::string>& actions);
}