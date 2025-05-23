//game_world.h
#pragma once
#include "game_objects.h"
#include <vector>
#include <string>

struct FloorData {
    std::vector<Room> rooms;
    std::vector<Action> actions;
    std::map<std::string, Action> actionMap;
    std::vector<Effect> effects;
    std::vector<Monster> monsters;
};

class GameWorld {
public:
    bool loadAllFloors(int numFloors);
    bool loadFloorData(int floorNumber);
    const FloorData& getCurrentFloor() const;
    const Room& getCurrentRoom(int floor, int room) const;
    Room& getCurrentRoomMutable();
    int getFloorCount() const;
    void setCurrentPosition(int floor, int room);

    int getCurrentFloorIndex() const { return currentFloor; }
    int getCurrentRoomIndex() const { return currentRoom; }

private:
    std::vector<FloorData> floors;
    int currentFloor = 0;
    int currentRoom = 0;

    bool loadRooms(const std::string& filename, std::vector<Room>& rooms, int floorNumber);
    bool loadActions(const std::vector<std::string>& actionFiles,
        const std::vector<std::string>& resultFiles,
        std::vector<Action>& actions,
        std::map<std::string, Action>& actionMap);
    bool loadEffects(const std::string& filename, std::vector<Effect>& effects);
    bool loadMonsters(const std::string& filename, std::vector<Monster>& monsters);
};