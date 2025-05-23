// game_world.cpp
#include "game_world.h"
#include "../ui/console_ui.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <algorithm>
#include <iostream>

using namespace std;

// Вспомогательная функция для формирования пути к файлам данных
string getDataPath(const string& filename) {
    return "data/" + filename;
}

bool GameWorld::loadRooms(const string& filename, vector<Room>& rooms, int floorNumber) {
    ifstream file(getDataPath(filename));
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла комнат: " << filename << endl;
        return false;
    }

    unordered_set<int> roomIds;
    string line;
    Room currentRoom(-1, "", "", {});
    string* currentField = nullptr;

    while (getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty()) continue;

        if (line[0] == '[') {
            if (currentRoom.id != -1) {
                if (roomIds.count(currentRoom.id)) {
                    cerr << "Ошибка: дубликат ID комнаты " << currentRoom.id << endl;
                    return false;
                }
                roomIds.insert(currentRoom.id);
                rooms.push_back(currentRoom);
            }

            currentRoom = Room(-1, "", "", {});
            currentField = nullptr;

            string idStr = line.substr(1, line.find(']') - 1);
            size_t underscore = idStr.find('_');
            if (underscore != string::npos) {
                try {
                    string floorStr = idStr.substr(0, underscore);
                    string roomStr = idStr.substr(underscore + 1);
                    int fileFloorNumber = stoi(floorStr) - 1;
                    if (fileFloorNumber == floorNumber) {
                        currentRoom.id = stoi(roomStr);
                    }
                }
                catch (...) {
                    currentRoom.id = -1;
                }
            }
            continue;
        }

        size_t sepPos = line.find('=');
        if (sepPos != string::npos) {
            string key = line.substr(0, sepPos);
            string value = line.substr(sepPos + 1);

            if (key == "Название") {
                currentRoom.name = value;
                currentField = &currentRoom.name;
            }
            else if (key == "Описание") {
                currentRoom.description = value;
                currentField = &currentRoom.description;
            }
            else if (key == "Действия") {
                stringstream ss(value);
                string actionId;
                while (getline(ss, actionId, ',')) {
                    actionId.erase(0, actionId.find_first_not_of(" \t"));
                    actionId.erase(actionId.find_last_not_of(" \t") + 1);
                    if (!actionId.empty()) {
                        currentRoom.actionIds.push_back(actionId);
                    }
                }
                currentField = nullptr;
            }
        }
        else if (currentField) {
            *currentField += "\n" + line;
        }
    }

    if (currentRoom.id != -1) {
        if (roomIds.count(currentRoom.id)) {
            cerr << "Ошибка: дубликат ID комнаты " << currentRoom.id << endl;
            return false;
        }
        rooms.push_back(currentRoom);
    }

    return !rooms.empty();
}

bool GameWorld::loadActions(const vector<string>& actionFiles,
    const vector<string>& resultFiles,
    vector<Action>& actions,
    map<string, Action>& actionMap) {
    // 1. Загрузка действий
    for (const auto& filename : actionFiles) {
        ifstream file(getDataPath(filename));
        if (!file.is_open()) {
            cerr << "Ошибка открытия файла действий: " << filename << endl;
            continue;
        }

        string line;
        string currentSection;

        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (line.empty()) continue;

            if (line[0] == '[') {
                currentSection = line.substr(1, line.find(']') - 1);
                continue;
            }

            if (currentSection == "Действия") {
                size_t sepPos = line.find('=');
                if (sepPos != string::npos) {
                    string fullId = line.substr(0, sepPos);
                    string value = line.substr(sepPos + 1);

                    size_t statPos = value.find_last_of(';');
                    if (statPos != string::npos) {
                        string description = value.substr(0, statPos);
                        string stat = value.substr(statPos + 1);

                        Action newAction(fullId, description, stat);
                        actions.push_back(newAction);
                        actionMap[fullId] = newAction;
                    }
                }
            }
        }
    }

    // 2. Загрузка результатов
    for (const auto& filename : resultFiles) {
        ifstream file(getDataPath(filename));
        if (!file.is_open()) {
            cerr << "Ошибка открытия файла результатов: " << filename << endl;
            continue;
        }

        string line;
        string currentSection;
        ActionResult* currentResult = nullptr;
        string currentFullId;

        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (line.empty()) continue;

            if (line[0] == '[') {
                string section = line.substr(1, line.find(']') - 1);
                if (section == "Результаты") {
                    currentSection = section;
                    continue;
                }

                size_t lastUnderscore = section.rfind('_');
                if (lastUnderscore != string::npos) {
                    string resultType = section.substr(lastUnderscore + 1);
                    currentFullId = section.substr(0, lastUnderscore);

                    auto mapIt = actionMap.find(currentFullId);
                    if (mapIt != actionMap.end()) {
                        currentResult = (resultType == "success") ? &mapIt->second.success : &mapIt->second.failure;
                    }
                }
                continue;
            }

            if (currentResult) {
                size_t eqPos = line.find('=');
                if (eqPos != string::npos) {
                    string key = line.substr(0, eqPos);
                    string value = line.substr(eqPos + 1);

                    if (key == "Описание") {
                        size_t pos = 0;
                        while ((pos = value.find("\\n", pos)) != string::npos) {
                            value.replace(pos, 2, "\n");
                            pos += 1;
                        }
                        currentResult->description = value;
                    }
                    else if (key == "Предмет") {
                        currentResult->item = value;
                    }
                }
            }
        }
    }

    return true;
}

bool GameWorld::loadEffects(const string& filename, vector<Effect>& effects) {
    ifstream file(getDataPath(filename));
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла эффектов: " << filename << endl;
        return false;
    }

    string line;
    getline(file, line); // Пропускаем заголовок

    while (getline(file, line)) {
        vector<string> fields;
        string field;
        stringstream ss(line);

        while (getline(ss, field, ',')) {
            field.erase(0, field.find_first_not_of(" \t"));
            field.erase(field.find_last_not_of(" \t") + 1);
            fields.push_back(field);
        }

        if (fields.size() != 11) {
            cerr << "Неправильное количество полей в строке: " << line << endl;
            continue;
        }

        try {
            Effect e(
                fields[0], fields[1], stoi(fields[2]),
                stoi(fields[3]), stoi(fields[4]), stoi(fields[5]), stoi(fields[6]),
                stoi(fields[7]), stoi(fields[8]), stoi(fields[9]), stoi(fields[10])
            );
            effects.push_back(e);
        }
        catch (...) {
            cerr << "Ошибка парсинга строки: " << line << endl;
        }
    }

    return true;
}

bool GameWorld::loadMonsters(const string& filename, vector<Monster>& monsters) {
    ifstream file(getDataPath(filename));
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла монстров: " << filename << endl;
        return false;
    }

    string line;
    getline(file, line); // Пропускаем заголовок

    while (getline(file, line)) {
        stringstream ss(line);
        string name, elementStr, weaknessStr;

        getline(ss, name, ',');
        getline(ss, elementStr, ',');
        getline(ss, weaknessStr, ',');

        Element element = Element::NONE;
        if (elementStr == "EARTH") element = Element::EARTH;
        else if (elementStr == "FIRE") element = Element::FIRE;
        else if (elementStr == "POISON") element = Element::POISON;
        else if (elementStr == "WATER") element = Element::WATER;

        Element weakness = Element::NONE;
        if (weaknessStr == "EARTH") weakness = Element::EARTH;
        else if (weaknessStr == "FIRE") weakness = Element::FIRE;
        else if (weaknessStr == "POISON") weakness = Element::POISON;
        else if (weaknessStr == "WATER") weakness = Element::WATER;

        monsters.emplace_back(name, 50, element, weakness);
    }

    return true;
}

bool GameWorld::loadAllFloors(int numFloors) {
    floors.clear();
    floors.resize(numFloors);

    for (int i = 0; i < numFloors; ++i) {
        if (!loadFloorData(i)) {
            return false;
        }
    }

    return true;
}

bool GameWorld::loadFloorData(int floorNumber) {
    if (floorNumber < 0 || floorNumber >= static_cast<int>(floors.size())) {
        cerr << "Неверный номер этажа: " << floorNumber << endl;
        return false;
    }

    if (!floors[floorNumber].rooms.empty()) {
        return true;
    }

    string roomFile = "room" + to_string(floorNumber + 1) + ".txt";
    string actionFile = "actions" + to_string(floorNumber + 1) + ".txt";
    string resultFile = "results" + to_string(floorNumber + 1) + ".txt";

    bool success = true;
    success &= loadRooms(roomFile, floors[floorNumber].rooms, floorNumber);
    success &= loadActions({ actionFile }, { resultFile },
        floors[floorNumber].actions,
        floors[floorNumber].actionMap);

    if (floorNumber == 0) {
        success &= loadEffects("effects.txt", floors[floorNumber].effects);
        success &= loadMonsters("monsters.txt", floors[floorNumber].monsters);
    }
    else {
        floors[floorNumber].effects = floors[0].effects;
        floors[floorNumber].monsters = floors[0].monsters;
    }

    if (!success) {
        cerr << "Ошибка загрузки данных для этажа " << (floorNumber + 1) << endl;
        return false;
    }

    return true;
}

const FloorData& GameWorld::getCurrentFloor() const {
    if (floors.empty()) {
        throw runtime_error("No floors loaded");
    }
    if (currentFloor < 0 || currentFloor >= static_cast<int>(floors.size())) {
        throw runtime_error("Invalid current floor index");
    }
    return floors[currentFloor];
}

const Room& GameWorld::getCurrentRoom(int floor, int room) const {
    if (floor < 0 || floor >= floors.size()) {
        throw out_of_range("Invalid floor number");
    }
    if (room < 0 || room >= floors[floor].rooms.size()) {
        throw out_of_range("Invalid room number");
    }
    return floors[floor].rooms[room];
}

Room& GameWorld::getCurrentRoomMutable() {
    if (floors.empty()) {
        throw runtime_error("Не загружены данные этажей");
    }
    if (currentFloor < 0 || currentFloor >= static_cast<int>(floors.size())) {
        throw runtime_error("Неверный индекс текущего этажа");
    }
    if (currentRoom < 0 || currentRoom >= static_cast<int>(floors[currentFloor].rooms.size())) {
        throw runtime_error("Неверный индекс текущей комнаты");
    }
    return floors[currentFloor].rooms[currentRoom];
}

int GameWorld::getFloorCount() const {
    return static_cast<int>(floors.size());
}

void GameWorld::setCurrentPosition(int floor, int room) {
    if (floor < 0 || floor >= floors.size() ||
        room < 0 || room >= floors[floor].rooms.size()) {
        throw out_of_range("Invalid position");
    }
    currentFloor = floor;
    currentRoom = room;
}