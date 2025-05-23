//main.cpp
#include "core/game_world.h"
#include "core/game_actions.h"
#include "ui/console_ui.h"
#include <windows.h>
#include <stdexcept>
#include <iostream>

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif

void initGame(GameWorld& world) {
    const int numFloors = 4;
    if (!world.loadAllFloors(numFloors)) {
        throw std::runtime_error("Ошибка загрузки данных игры");
    }
}

int main() {
    UI::initConsole();
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    try {
        GameWorld world;
        initGame(world);
        Player player;

        UI::showGameIntro();
        UI::waitForEnter();

        bool wasLastRoom = false;
        bool shouldRestartFromBeginning = false;

        while (true) {
            if (player.health <= 0 || player.shipParts >= 7) break;

            GameActions::handleRoomAction(player, world);

            // Проверяем, были ли мы в последней комнате
            if (world.getCurrentFloorIndex() == world.getFloorCount() - 1 &&
                world.getCurrentRoomIndex() == world.getCurrentFloor().rooms.size() - 1) {
                wasLastRoom = true;
            }
            // Проверяем, вернулись ли на первый этаж после последней комнаты
            else if (wasLastRoom && world.getCurrentFloorIndex() == 0 &&
                world.getCurrentRoomIndex() == 0) {

                if (player.shipParts < 7) {
                    UI::printColored("\nВы исследовали всю станцию, но не нашли все детали!\n", 14);
                    UI::printColored("Возвращаемся на первый этаж...\n\n", 14);
                    UI::waitForEnter();
                    system("cls");
                }
                wasLastRoom = false;
            }

            if (player.health > 0) {
                UI::waitForEnter();
            }
        }

        // Обработка завершения игры
        if (player.shipParts >= 7) {
            UI::showGameEnding(true);  // Победа
        }
        else if (player.health <= 0) {
            UI::showGameEnding(false); // Поражение
        }

        UI::waitForEnter();
    }
    catch (const std::exception& e) {
        UI::printColored("\nОшибка: " + std::string(e.what()) + "\n", 12);
        UI::waitForEnter();
        return 1;
    }
    catch (...) {
        UI::printColored("\nНеизвестная ошибка!\n", 12);
        UI::waitForEnter();
        return 1;
    }

    return 0;
}