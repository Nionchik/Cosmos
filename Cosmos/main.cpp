// main.cpp
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <windows.h>

#include "core/game_actions.h"
#include "core/game_world.h"
#include "ui/console_ui.h"

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif

void InitGame(GameWorld& world, Player& player);

int main() {
    ui::InitConsole();
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    try {
        GameWorld world;
        Player player;
        InitGame(world, player);

        ui::ShowGameIntro();

        ui::PrintColored("\n=== Ваши начальные характеристики ===\n", 11);
        ui::PrintPlayerStats(player);

        ui::WaitForEnter("Нажмите Enter ...");
        system("cls");

        bool was_last_room = false;
        bool first_room = true;

        while (true) {
            if (player.health <= 0) {
                ui::ShowGameEnding(false);
                exit(0);
            }

            if (player.ship_parts >= 7) {
                ui::ShowGameEnding(true);
                exit(0);
            }

            Room& current_room = world.GetCurrentRoomMutable();

            if (current_room.first_visit) {
                ui::PrintColored("=== " +
                    std::to_string(world.GetCurrentFloorIndex() + 1) +
                    " этаж ===\n", 11);
                ui::PrintRoomDescription(current_room);
                ui::WaitForEnter("Нажмите Enter ...");
                current_room.first_visit = false;
                continue;
            }

            GameActions::HandleRoomAction(player, world);

            if (world.GetCurrentFloorIndex() == world.GetFloorCount() - 1 &&
                world.GetCurrentRoomIndex() ==
                world.GetCurrentFloor().rooms.size() - 1) {
                was_last_room = true;
            }
            else if (was_last_room && world.GetCurrentFloorIndex() == 0 &&
                world.GetCurrentRoomIndex() == 0) {

                if (player.ship_parts < 7) {
                    ui::PrintColored("\nВы исследовали всю станцию, "
                        "но не нашли все детали!\n", 14);
                    ui::PrintColored("Возвращаемся на первый этаж...\n\n", 14);
                    ui::WaitForEnter("Нажмите Enter ...");
                    system("cls");
                }
                was_last_room = false;
                first_room = true;
            }
        }              
        ui::WaitForEnter("Нажмите Enter ...");
    }
    catch (const std::exception& e) {
        ui::PrintColored("\nОшибка: " + std::string(e.what()) + "\n", 12);
        ui::WaitForEnter("Нажмите Enter ...");
        return 1;
    }
    catch (...) {
        ui::PrintColored("\nНеизвестная ошибка!\n", 12);
        ui::WaitForEnter("Нажмите Enter ...");
        return 1;
    }

    return 0;
}

void InitGame(GameWorld& world, Player& player) {
    const int num_floors = 4;
    if (!world.LoadAllFloors(num_floors, player)) {
        throw std::runtime_error("Ошибка загрузки данных игры");
    }
}