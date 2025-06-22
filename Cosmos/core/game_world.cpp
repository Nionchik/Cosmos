#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

#include "../ui/console_ui.h"
#include "game_world.h"

std::string GameWorld::GetDataPath(const std::string& filename) {
  return "data/" + filename;
}

bool GameWorld::LoadPlayerStats(const std::string& filename, Player& player) {
  std::ifstream file(GetDataPath(filename));
  if (!file.is_open()) {
    std::cerr << "Ошибка открытия файла статистики игрока: " << filename
      << std::endl;
    return false;
  }

  std::string line;
  std::getline(file, line);

  if (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string value;
    std::vector<int> stats;

    while (std::getline(ss, value, ',')) {
      stats.push_back(std::stoi(value));
    }

    if (stats.size() >= 12) {
      player.health = stats[0];
      player.ship_parts = stats[1];
      player.strength = stats[2];
      player.agility = stats[3];
      player.intellect = stats[4];
      player.endurance = stats[5];
      player.sudden_strength_potions = stats[6];
      player.life_crystals = stats[7];
      player.earth_resistance = stats[8];
      player.fire_resistance = stats[9];
      player.poison_resistance = stats[10];
      player.water_resistance = stats[11];
      return true;
    }
  }
  return false;
}

bool GameWorld::LoadWeapons(const std::string& filename) {
  weapons_.clear();
  std::ifstream file(GetDataPath(filename));
  if (!file.is_open()) {
    std::cerr << "Ошибка открытия файла оружия: " << filename << std::endl;
    return false;
  }

  std::string line;
  std::getline(file, line); // Пропускаем заголовок

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, ',')) {
      tokens.push_back(token);
    }

    if (tokens.size() != 7) {
      std::cerr << "Неверный формат строки в файле оружия: ожидается 7 полей, получено "
        << tokens.size() << ": " << line << std::endl;
      continue;
    }

    std::string name = tokens[0];
    Element element;
    if (tokens[1] == "EARTH")
      element = Element::kEarth;
    else if (tokens[1] == "FIRE")
      element = Element::kFire;
    else if (tokens[1] == "POISON")
      element = Element::kPoison;
    else if (tokens[1] == "WATER")
      element = Element::kWater;
    else
      element = Element::kNone;

    try {
      int min_neutral = std::stoi(tokens[2]);
      int max_neutral = std::stoi(tokens[3]);
      int min_weak = std::stoi(tokens[4]);
      int max_weak = std::stoi(tokens[5]);
      int crit_chance = std::stoi(tokens[6]);

      weapons_.emplace_back(name, element, min_neutral, max_neutral,
        min_weak, max_weak, crit_chance);
    }
    catch (const std::exception& e) {
      std::cerr << "Ошибка парсинга данных оружия: " << line << "\n"
        << "Причина: " << e.what() << std::endl;
    }
    catch (...) {
      std::cerr << "Неизвестная ошибка парсинга данных оружия: " << line
        << std::endl;
    }
  }

  if (weapons_.empty()) {
    std::cerr << "Предупреждение: не загружено ни одного оружия" << std::endl;
  }

  return !weapons_.empty();
}

bool GameWorld::LoadRooms(const std::string& filename,
  std::vector<Room>& rooms, int floor_number) {
  std::ifstream file(GetDataPath(filename));
  if (!file.is_open()) {
    std::cerr << "Ошибка открытия файла комнат: " << filename << std::endl;
    return false;
  }

  std::unordered_set<int> room_ids;
  std::string line;
  Room current_room(-1, "", "", {});
  std::string* current_field = nullptr;

  while (std::getline(file, line)) {
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t") + 1);
    if (line.empty()) continue;

    if (line[0] == '[') {
      if (current_room.id != -1) {
        if (room_ids.count(current_room.id)) {
          std::cerr << "Ошибка: дубликат ID комнаты " << current_room.id
            << std::endl;
          return false;
        }
        room_ids.insert(current_room.id);
        rooms.push_back(current_room);
      }

      current_room = Room(-1, "", "", {});
      current_field = nullptr;

      std::string id_str = line.substr(1, line.find(']') - 1);
      size_t underscore = id_str.find('_');
      if (underscore != std::string::npos) {
        try {
          std::string floor_str = id_str.substr(0, underscore);
          std::string room_str = id_str.substr(underscore + 1);
          int file_floor_number = std::stoi(floor_str) - 1;
          if (file_floor_number == floor_number) {
            current_room.id = std::stoi(room_str);
          }
        }
        catch (...) {
          current_room.id = -1;
        }
      }
      continue;
    }

    size_t sep_pos = line.find('=');
    if (sep_pos != std::string::npos) {
      std::string key = line.substr(0, sep_pos);
      std::string value = line.substr(sep_pos + 1);

      if (key == "Название") {
        current_room.name = value;
        current_field = &current_room.name;
      }
      else if (key == "Описание") {
        current_room.description = value;
        current_field = &current_room.description;
      }
      else if (key == "Действия") {
        std::stringstream ss(value);
        std::string action_id;
        while (std::getline(ss, action_id, ',')) {
          action_id.erase(0, action_id.find_first_not_of(" \t"));
          action_id.erase(action_id.find_last_not_of(" \t") + 1);
          if (!action_id.empty()) {
            current_room.action_ids.push_back(action_id);
          }
        }
        current_field = nullptr;
      }
    }
    else if (current_field) {
      *current_field += "\n" + line;
    }
  }

  if (current_room.id != -1) {
    if (room_ids.count(current_room.id)) {
      std::cerr << "Ошибка: дубликат ID комнаты "
        << current_room.id << std::endl;
      return false;
    }
    rooms.push_back(current_room);
  }

  return !rooms.empty();
}

bool GameWorld::LoadActions(
  const std::vector<std::string>& action_files,
  const std::vector<std::string>& result_files,
  std::vector<Action>& actions,
  std::map<std::string, Action>& action_map) {
  for (const auto& filename : action_files) {
    std::ifstream file(GetDataPath(filename));
    if (!file.is_open()) {
      std::cerr << "Ошибка открытия файла действий: " << filename << std::endl;
      continue;
    }

    std::string line;
    std::string current_section;

    while (std::getline(file, line)) {
      line.erase(0, line.find_first_not_of(" \t"));
      line.erase(line.find_last_not_of(" \t") + 1);
      if (line.empty()) continue;

      if (line[0] == '[') {
        current_section = line.substr(1, line.find(']') - 1);
        continue;
      }

      if (current_section == "Действия") {
        size_t sep_pos = line.find('=');
        if (sep_pos != std::string::npos) {
          std::string full_id = line.substr(0, sep_pos);
          std::string value = line.substr(sep_pos + 1);

          size_t stat_pos = value.find_last_of(';');
          if (stat_pos != std::string::npos) {
            std::string description = value.substr(0, stat_pos);
            std::string stat = value.substr(stat_pos + 1);

            Action new_action(full_id, description, stat);
            actions.push_back(new_action);
            action_map[full_id] = new_action;
          }
        }
      }
    }
  }

  for (const auto& filename : result_files) {
    std::ifstream file(GetDataPath(filename));
    if (!file.is_open()) {
      std::cerr << "Ошибка открытия файла результатов: "
        << filename << std::endl;
      continue;
    }

    std::string line;
    std::string current_section;
    ActionResult* current_result = nullptr;
    std::string current_full_id;

    while (std::getline(file, line)) {
      line.erase(0, line.find_first_not_of(" \t"));
      line.erase(line.find_last_not_of(" \t") + 1);
      if (line.empty()) continue;

      if (line[0] == '[') {
        std::string section = line.substr(1, line.find(']') - 1);
        if (section == "Результаты") {
          current_section = section;
          continue;
        }

        size_t last_underscore = section.rfind('_');
        if (last_underscore != std::string::npos) {
          std::string result_type = section.substr(last_underscore + 1);
          current_full_id = section.substr(0, last_underscore);

          auto map_it = action_map.find(current_full_id);
          if (map_it != action_map.end()) {
            current_result = (result_type == "success") ?
              &map_it->second.success : &map_it->second.failure;
          }
        }
        continue;
      }

      if (current_result) {
        size_t eq_pos = line.find('=');
        if (eq_pos != std::string::npos) {
          std::string key = line.substr(0, eq_pos);
          std::string value = line.substr(eq_pos + 1);

          if (key == "Описание") {
            size_t pos = 0;
            while ((pos = value.find("\\n", pos)) != std::string::npos) {
              value.replace(pos, 2, "\n");
              pos += 1;
            }
            current_result->description = value;
          }
          else if (key == "Предмет") {
            current_result->item = value;
          }
        }
      }
    }
  }
  return true;
}

bool GameWorld::LoadEffects(const std::string& filename,
  std::vector<Effect>& effects) {
  std::ifstream file(GetDataPath(filename));
  if (!file.is_open()) {
    std::cerr << "Ошибка открытия файла эффектов: " << filename << std::endl;
    return false;
  }

  std::string line;
  std::getline(file, line);

  while (std::getline(file, line)) {
    std::vector<std::string> fields;
    std::string field;
    std::stringstream ss(line);

    while (std::getline(ss, field, ',')) {
      field.erase(0, field.find_first_not_of(" \t"));
      field.erase(field.find_last_not_of(" \t") + 1);
      fields.push_back(field);
    }

    if (fields.size() != 11) {
      std::cerr << "Неправильное количество полей в строке: " << line
        << std::endl;
      continue;
    }

    try {
      Effect e(fields[0], fields[1], std::stoi(fields[2]),
        std::stoi(fields[3]), std::stoi(fields[4]), std::stoi(fields[5]),
        std::stoi(fields[6]), std::stoi(fields[7]), std::stoi(fields[8]),
        std::stoi(fields[9]), std::stoi(fields[10]));
      effects.push_back(e);
    }
    catch (...) {
      std::cerr << "Ошибка парсинга строки: " << line << std::endl;
    }
  }
  return true;
}

bool GameWorld::LoadMonsters(const std::string& filename,
  std::vector<Monster>& monsters) {
  std::ifstream file(GetDataPath(filename));
  if (!file.is_open()) {
    std::cerr << "Ошибка открытия файла монстров: " << filename << std::endl;
    return false;
  }

  std::string line;
  std::getline(file, line);

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string name, element_str, weakness_str, health_str;

    std::getline(ss, name, ',');
    std::getline(ss, element_str, ',');
    std::getline(ss, weakness_str, ',');
    std::getline(ss, health_str, ',');

    Element element = Element::kNone;
    if (element_str == "EARTH")
      element = Element::kEarth;
    else if (element_str == "FIRE")
      element = Element::kFire;
    else if (element_str == "POISON")
      element = Element::kPoison;
    else if (element_str == "WATER")
      element = Element::kWater;

    Element weakness = Element::kNone;
    if (weakness_str == "EARTH")
      weakness = Element::kEarth;
    else if (weakness_str == "FIRE")
      weakness = Element::kFire;
    else if (weakness_str == "POISON")
      weakness = Element::kPoison;
    else if (weakness_str == "WATER")
      weakness = Element::kWater;

    int health = std::stoi(health_str);
    monsters.emplace_back(name, health, element, weakness);
  }
  return true;
}

bool GameWorld::LoadConfig(const std::string& filename) {
  std::ifstream file(GetDataPath(filename));
  if (!file.is_open()) {
    std::cerr << "Ошибка открытия конфига: " << filename << std::endl;
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.find("floors:") == 0) {
      try {
        num_floors_ = std::stoi(line.substr(7));
      }
      catch (...) {
        std::cerr << "Ошибка чтения количества этажей" << std::endl;
        return false;
      }
    }
    else if (line.find("min_melee_damage:") == 0) {
      combat_config_.min_melee_damage = std::stoi(line.substr(17));
    }
    else if (line.find("max_melee_damage:") == 0) {
      combat_config_.max_melee_damage = std::stoi(line.substr(17));
    }
    else if (line.find("min_monster_damage:") == 0) {
      combat_config_.min_monster_damage = std::stoi(line.substr(19));
    }
    else if (line.find("max_monster_damage:") == 0) {
      combat_config_.max_monster_damage = std::stoi(line.substr(19));
    }
    else if (line.find("melee_crit_chance:") == 0) {
      combat_config_.melee_crit_chance = std::stoi(line.substr(18));
    }
  }
  return num_floors_ > 0;
}

bool GameWorld::LoadAllFloors(Player& player) {
  if (num_floors_ <= 0) {
    std::cerr << "Количество этажей не задано!" << std::endl;
    return false;
  }
  floors_.clear();
  floors_.resize(num_floors_);

  if (!LoadPlayerStats("player_stats.txt", player)) {
    std::cerr << "Failed to load player stats" << std::endl;
    return false;
  }

  if (!LoadWeapons("weapons.txt")) {
    std::cerr << "Failed to load weapons" << std::endl;
    return false;
  }

  for (int i = 0; i < num_floors_; ++i) {
    if (!LoadFloorData(i)) {
      std::cerr << "Failed to load floor " << i << std::endl;
      return false;
    }
  }
  return true;
}

bool GameWorld::LoadFloorData(int floor_number) {
  if (floor_number < 0 || floor_number >= static_cast<int>(floors_.size())) {
    std::cerr << "Неверный номер этажа: " << floor_number << std::endl;
    return false;
  }

  if (!floors_[floor_number].rooms.empty()) {
    return true;
  }

  std::string room_file = "room" + std::to_string(floor_number + 1) + ".txt";
  std::string action_file = "actions" +
    std::to_string(floor_number + 1) + ".txt";
  std::string result_file = "results" +
    std::to_string(floor_number + 1) + ".txt";

  bool success = true;
  success &= LoadRooms(room_file, floors_[floor_number].rooms, floor_number);
  success &= LoadActions({ action_file }, { result_file },
    floors_[floor_number].actions, floors_[floor_number].action_map);

  if (floor_number == 0) {
    success &= LoadEffects("effects.txt", floors_[floor_number].effects);
    success &= LoadMonsters("monsters.txt", floors_[floor_number].monsters);
  }
  else {
    floors_[floor_number].effects = floors_[0].effects;
    floors_[floor_number].monsters = floors_[0].monsters;
  }

  if (!success) {
    std::cerr << "Ошибка загрузки данных для этажа " << (floor_number + 1)
      << std::endl;
    return false;
  }
  return true;
}

const FloorData& GameWorld::GetCurrentFloor() const {
  if (floors_.empty()) {
    throw std::runtime_error("No floors loaded");
  }
  if (current_floor_ < 0 || current_floor_ >=
    static_cast<int>(floors_.size())) {
    throw std::runtime_error("Invalid current floor index");
  }
  return floors_[current_floor_];
}

const Room& GameWorld::GetCurrentRoom(int floor, int room) const {
  if (floor < 0 || floor >= static_cast<int>(floors_.size())) {
    throw std::out_of_range("Invalid floor number");
  }
  if (room < 0 || room >= static_cast<int>(floors_[floor].rooms.size())) {
    throw std::out_of_range("Invalid room number");
  }
  return floors_[floor].rooms[room];
}

Room& GameWorld::GetCurrentRoomMutable() {
  if (floors_.empty()) {
    throw std::runtime_error("Не загружены данные этажей");
  }
  if (current_floor_ < 0 || current_floor_ >=
    static_cast<int>(floors_.size())) {
    throw std::runtime_error("Неверный индекс текущего этажа");
  }
  if (current_room_ < 0 ||
    current_room_ >= static_cast<int>(floors_[current_floor_].rooms.size())) {
    throw std::runtime_error("Неверный индекс текущей комнаты");
  }
  return floors_[current_floor_].rooms[current_room_];
}

int GameWorld::GetFloorCount() const {
  return static_cast<int>(floors_.size());
}

void GameWorld::SetCurrentPosition(int floor, int room) {
  if (floor < 0 || floor >= static_cast<int>(floors_.size()) || room < 0 ||
    room >= static_cast<int>(floors_[floor].rooms.size())) {
    throw std::out_of_range("Invalid position");
  }
  current_floor_ = floor;
  current_room_ = room;
}