#pragma once
#include <string>
#include <vector>
#include <map>

#include "game_objects.h"

struct FloorData {
  std::vector<Room> rooms;
  std::vector<Action> actions;
  std::map<std::string, Action> action_map;
  std::vector<Effect> effects;
  std::vector<Monster> monsters;
};

class GameWorld {
public:
  bool LoadAllFloors(Player& player);
  bool LoadFloorData(int floor_number);
  bool LoadConfig(const std::string& filename);
  const FloorData& GetCurrentFloor() const;
  const Room& GetCurrentRoom(int floor, int room) const;
  const std::vector<Weapon>& GetWeapons() const { return weapons_; }
  const CombatConfig& GetCombatConfig() const { return combat_config_; }
  Room& GetCurrentRoomMutable();
  int GetFloorCount() const;
  void SetCurrentPosition(int floor, int room);

  int GetCurrentFloorIndex() const { return current_floor_; }
  int GetCurrentRoomIndex() const { return current_room_; }

  static std::string GetDataPath(const std::string& filename);

private:
  std::vector<FloorData> floors_;
  std::vector<Weapon> weapons_;
  CombatConfig combat_config_;
  int current_floor_ = 0;
  int current_room_ = 0;
  int num_floors_ = 0;

  bool LoadPlayerStats(const std::string& filename, Player& player);
  bool LoadWeapons(const std::string& filename);
  bool LoadMonsters(const std::string& filename,
    std::vector<Monster>& monsters);
  bool LoadRooms(const std::string& filename,
    std::vector<Room>& rooms,
    int floor_number);
  bool LoadActions(const std::vector<std::string>& action_files,
    const std::vector<std::string>& result_files,
    std::vector<Action>& actions,
    std::map<std::string, Action>& action_map);
  bool LoadEffects(const std::string& filename, std::vector<Effect>& effects);
};