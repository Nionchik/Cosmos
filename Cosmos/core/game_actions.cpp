#include <algorithm>
#include <iostream>
#include <random>
#include <string>

#include "../ui/console_ui.h"
#include "game_actions.h"

void GameActions::ResetRoomFirstVisit(Player& player, GameWorld& world) {
  Room& room = world.GetCurrentRoomMutable();
  if (room.first_visit) {
    room.first_visit = false;
    room.completed_actions.clear();
  }
}

void GameActions::ProcessEffect(Player& player, const std::string& item_name,
  const std::vector<Effect>& effects) {
  auto it = std::find_if(effects.begin(), effects.end(),
    [&](const Effect& e) { return e.name == item_name; });

  if (it != effects.end()) {
    const Effect& effect = *it;
    if (effect.type == "базовый") {
      if (effect.name == "Деталь") {
        if (player.CanFindPart()) {
          player.ship_parts++;
          std::string key = std::to_string(player.current_floor) + "_" +
            std::to_string(player.current_room);
          player.found_parts[key] = true;
          ui::PrintColored("Найдена деталь корабля! (" +
            std::to_string(player.ship_parts) + "/7)\n", 11);
        }
        else {
          ui::PrintColored("УВЫ, но здесь Вы всё уже нашли!\n", 13);
        }
      }
      else if (effect.name == "Флакон Внезапной Силы") {
        player.sudden_strength_potions++;
        ui::PrintColored("Получен Флакон Внезапной Силы! (Теперь: " +
          std::to_string(player.sudden_strength_potions) + ")\n",
          13);
      }
      else if (effect.name == "Кристалл Жизни") {
        player.life_crystals++;
        ui::PrintColored("Получен Кристалл Жизни! (Теперь: " +
          std::to_string(player.life_crystals) + ")\n", 13);
      }
      else if (effect.health_effect > 0) {
        player.Heal(effect.health_effect);
        ui::PrintColored("Получено " + std::to_string(effect.health_effect) +
          " HP\n", 10);
      }
      else if (effect.health_effect < 0) {
        player.TakeDamage(-effect.health_effect);
        ui::PrintColored("Получено " + std::to_string(-effect.health_effect) +
          " урона\n", 12);
      }
    }
    else if (effect.type == "артефакт") {
      std::string key = std::to_string(player.current_floor) + "_" +
        std::to_string(player.current_room) + "_" + effect.name;

      if (player.found_artifacts.count(key)) {
        ui::PrintColored("УВЫ, но здесь Вы всё уже нашли!\n", 13);
        return;
      }

      player.found_artifacts[key] = true;
      int old_stats[4] = { player.strength, player.agility, player.intellect,
                          player.endurance };

      if (effect.strength_effect != 0) {
        player.strength =
          std::min(player.strength + effect.strength_effect, 80);
      }
      if (effect.agility_effect != 0) {
        player.agility = std::min(player.agility + effect.agility_effect, 80);
      }
      if (effect.intellect_effect != 0) {
        player.intellect =
          std::min(player.intellect + effect.intellect_effect, 80);
      }
      if (effect.endurance_effect != 0) {
        player.endurance =
          std::min(player.endurance + effect.endurance_effect, 80);
      }

      if (effect.fire_resistance > 0) {
        player.fire_resistance =
          std::min(player.fire_resistance + effect.fire_resistance, 80);
      }
      if (effect.poison_resistance > 0) {
        player.poison_resistance =
          std::min(player.poison_resistance + effect.poison_resistance, 80);
      }
      if (effect.earth_resistance > 0) {
        player.earth_resistance =
          std::min(player.earth_resistance + effect.earth_resistance, 80);
      }
      if (effect.water_resistance > 0) {
        player.water_resistance =
          std::min(player.water_resistance + effect.water_resistance, 80);
      }

      ui::PrintColored("\nНайден артефакт: " + effect.name + "\n", 6);
      if (effect.strength_effect != 0) {
        ui::PrintColored("СИЛ: " + std::to_string(old_stats[0]) + " > " +
          std::to_string(player.strength) + "\n", 14);
      }
      if (effect.agility_effect != 0) {
        ui::PrintColored("ЛОВ: " + std::to_string(old_stats[1]) + " > " +
          std::to_string(player.agility) + "\n", 14);
      }
      if (effect.intellect_effect != 0) {
        ui::PrintColored("ИНТ: " + std::to_string(old_stats[2]) + " > " +
          std::to_string(player.intellect) + "\n", 14);
      }
      if (effect.endurance_effect != 0) {
        ui::PrintColored("ВЫН: " + std::to_string(old_stats[3]) + " > " +
          std::to_string(player.endurance) + "\n", 14);
      }

      ui::PrintColored("\nТекущие характеристики:\n", 11);
      ui::PrintPlayerStats(player);
    }
  }
}

bool GameActions::CheckActionSuccess(const Player& player,
  const std::string& stat_name) {
  int stat_value = 0;
  if (stat_name == "СИЛ") {
    stat_value = player.strength;
  }
  else if (stat_name == "ЛОВ") {
    stat_value = player.agility;
  }
  else if (stat_name == "ИНТ") {
    stat_value = player.intellect;
  }
  else if (stat_name == "ВЫН") {
    stat_value = player.endurance;
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(1, 100);
  return (dist(gen) <= stat_value);
}

void GameActions::HandleRoomAction(Player& player, GameWorld& world) {
  const FloorData& current_floor = world.GetCurrentFloor();
  Room& current_room = world.GetCurrentRoomMutable();

  if (current_room.first_visit) {
    current_room.first_visit = false;
    current_room.completed_actions.clear();
  }

  std::vector<std::string> full_action_ids;
  for (const auto& action_id : current_room.action_ids) {
    full_action_ids.push_back(
      std::to_string(world.GetCurrentFloorIndex() + 1) + "_" +
      std::to_string(current_room.id) + "_" +
      action_id.substr(action_id.find_last_of('_') + 1));
  }

  std::vector<std::string> available_actions;
  for (const auto& full_id : full_action_ids) {
    const Action& action = current_floor.action_map.at(full_id);
    bool can_perform = true;

    if (!action.success.item.empty()) {
      auto effect_it = std::find_if(
        current_floor.effects.begin(), current_floor.effects.end(),
        [&](const Effect& e) { return e.name == action.success.item; });

      if (effect_it != current_floor.effects.end()) {
        if (effect_it->type == "артефакт") {
          std::string key = std::to_string(player.current_floor) + "_" +
            std::to_string(current_room.id) + "_" +
            action.success.item;
          if (player.found_artifacts.count(key)) {
            can_perform = false;
          }
        }
        else if (effect_it->name == "Деталь") {
          std::string key = std::to_string(player.current_floor) + "_" +
            std::to_string(current_room.id);
          if (player.found_parts.count(key)) {
            can_perform = false;
          }
        }
      }
    }

    if (!action.success.item.empty() || !action.failure.item.empty()) {
      if (current_room.completed_actions.find(full_id) !=
        current_room.completed_actions.end()) {
        can_perform = false;
      }
    }

    if (can_perform) {
      available_actions.push_back(full_id);
    }
  }

  if (available_actions.empty()) {
    for (const auto& full_id : full_action_ids) {
      const Action& action = current_floor.action_map.at(full_id);
      if (action.success.item.empty() && action.failure.item.empty()) {
        current_room.completed_actions.erase(full_id);
        available_actions.push_back(full_id);
      }
    }
  }

  ui::PrintAvailableActions(player, available_actions, current_floor.action_map);
  std::cout << (available_actions.size() + 1) << ". ";
  ui::PrintColored("Перейти дальше", 7);
  std::cout << std::endl;

  size_t choice;
  if (!(std::cin >> choice)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    ui::PrintColored("Неверный ввод! Пожалуйста, введите число.\n", 12);
    return;
  }
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  if (choice == available_actions.size() + 1) {
    MoveToNextLocation(player, world);
    return;
  }

  if (choice > 0 && choice <= available_actions.size()) {
    size_t index = choice - 1;
    const Action& action = current_floor.action_map.at(available_actions[index]);
    bool success = GameActions::CheckActionSuccess(player, action.required_stat);

    ShowActionResults(player, action, success, current_floor, world);

    if (!action.success.item.empty() || !action.failure.item.empty()) {
      current_room.completed_actions.insert(available_actions[index]);
    }

    bool has_more_actions = false;
    for (const auto& full_id : full_action_ids) {
      const Action& a = current_floor.action_map.at(full_id);
      bool can_perform = true;

      if (!a.success.item.empty()) {
        auto effect_it = std::find_if(
          current_floor.effects.begin(), current_floor.effects.end(),
          [&](const Effect& e) { return e.name == a.success.item; });

        if (effect_it != current_floor.effects.end()) {
          if (effect_it->type == "артефакт") {
            std::string key = std::to_string(player.current_floor) + "_" +
              std::to_string(current_room.id) + "_" +
              a.success.item;
            if (player.found_artifacts.count(key)) {
              can_perform = false;
            }
          }
          else if (effect_it->name == "Деталь") {
            std::string key = std::to_string(player.current_floor) + "_" +
              std::to_string(current_room.id);
            if (player.found_parts.count(key)) {
              can_perform = false;
            }
          }
        }
      }

      if (can_perform &&
        current_room.completed_actions.find(full_id) ==
        current_room.completed_actions.end()) {
        has_more_actions = true;
        break;
      }
    }

    if (!has_more_actions) {
      ui::PrintColored("\nВы все проверили здесь!\n", 14);
      ui::WaitForEnter("Нажмите Enter ...");
      MoveToNextLocation(player, world);
    }
  }
  else {
    ui::PrintColored("Неверный выбор!\n", 12);
    ui::WaitForEnter();
  }
}

void GameActions::MoveToNextLocation(Player& player, GameWorld& world) {
  Room& current_room = world.GetCurrentRoomMutable();
  current_room.completed_actions.clear();

  player.current_room++;
  if (player.current_room >= world.GetCurrentFloor().rooms.size()) {
    player.current_room = 0;
    player.current_floor++;
    if (player.current_floor >= world.GetFloorCount()) {
      player.current_floor = 0;
    }
  }

  world.GetCurrentRoomMutable().first_visit = true;
  player.actions_taken = 0;
  world.SetCurrentPosition(player.current_floor, player.current_room);
  ui::ClearScreen();
}

void GameActions::ShowActionResults(Player& player, const Action& action,
  bool success, const FloorData& floor,
  GameWorld& world) {
  ui::PrintColored("=== " + std::to_string(player.current_floor + 1) +
    " этаж ===\n", 11);

  if (success) {
    bool already_found = false;
    if (!action.success.item.empty()) {
      auto effect_it = std::find_if(
        floor.effects.begin(), floor.effects.end(),
        [&](const Effect& e) { return e.name == action.success.item; });

      if (effect_it != floor.effects.end()) {
        if (effect_it->type == "артефакт") {
          std::string key = std::to_string(player.current_floor) + "_" +
            std::to_string(player.current_room) + "_" +
            action.success.item;
          if (player.found_artifacts.count(key)) {
            already_found = true;
          }
        }
        else if (effect_it->name == "Деталь") {
          std::string key = std::to_string(player.current_floor) + "_" +
            std::to_string(player.current_room);
          if (player.found_parts.count(key)) {
            already_found = true;
          }
        }
      }
    }

    if (already_found) {
      ui::PrintColored("\nУВЫ, но здесь Вы всё уже нашли!\n", 13);
    }
    else {
      ui::PrintColored("\n=== УСПЕХ ===\n", 10);
      ui::PrintMultiline(action.success.description, 15, true);
      std::cout << std::endl;

      if (!action.success.item.empty()) {
        GameActions::ProcessEffect(player, action.success.item, floor.effects);
      }
    }
    ui::WaitForEnter();
  }
  else {
    ui::PrintColored("\n=== ПРОВАЛ ===\n", 12);
    ui::PrintMultiline(action.failure.description, 15, true);
    std::cout << std::endl;
    ui::WaitForEnter("Нажмите Enter ...");

    if (!action.failure.item.empty()) {
      if (action.failure.item == "бой") {
        ui::PrintColored("\nПриготовьтесь к бою!\n", 12);
        ui::WaitForEnter();

        if (!floor.monsters.empty()) {
          std::random_device rd;
          std::mt19937 gen(rd());
          std::uniform_int_distribution<size_t> dist(0,
            floor.monsters.size() - 1);
          Monster monster = floor.monsters[dist(gen)];
          if (!CombatSystem::Fight(player, monster, floor.effects,
            world.GetWeapons(), world.GetCombatConfig())) {
            if (player.health <= 0) {
              ui::ShowGameEnding(false);
              exit(0);
            }
            return;
          }
          ui::WaitForEnter();
        }
      }
      else {
        ui::PrintColored("\nПоследствия: ", 13);
        ui::PrintColored(action.failure.item + "\n", 14);
        GameActions::ProcessEffect(player, action.failure.item, floor.effects);
        ui::WaitForEnter();
      }
    }
  }
}