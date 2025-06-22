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
    ui::ClearScreen();

    bool was_last_room = false;

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
        std::string floor_text = "=== " +
          std::to_string(world.GetCurrentFloorIndex() + 1) +
          " этаж ===\n";
        ui::PrintColored(floor_text, 11);
        ui::PrintRoomDescription(current_room);
        ui::WaitForEnter("Нажмите Enter ...");
        current_room.first_visit = false;
        continue;
      }

      GameActions::HandleRoomAction(player, world);

      const int current_floor_index = world.GetCurrentFloorIndex();
      const int current_room_index = world.GetCurrentRoomIndex();
      const int floor_count = world.GetFloorCount();
      const int room_count = static_cast<int>(
        world.GetCurrentFloor().rooms.size());

      if (current_floor_index == floor_count - 1 &&
        current_room_index == room_count - 1) {
        was_last_room = true;
      }
      else if (was_last_room &&
        current_floor_index == 0 &&
        current_room_index == 0) {
        if (player.ship_parts < 7) {
          ui::PrintColored("\nВы исследовали всю станцию, "
            "но не нашли все детали!\n", 14);
          ui::PrintColored("Возвращаемся на первый этаж...\n\n", 14);
          ui::WaitForEnter("Нажмите Enter ...");
          ui::ClearScreen();
        }
        was_last_room = false;
      }
    }
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
  if (!world.LoadConfig("game_config.txt")) {
    throw std::runtime_error("Ошибка загрузки конфигурации");
  }

  if (!world.LoadAllFloors(player)) {
    throw std::runtime_error("Ошибка загрузки данных игры");
  }
}