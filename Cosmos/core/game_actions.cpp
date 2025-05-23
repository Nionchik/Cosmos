// game_actions.cpp
#include "game_actions.h"
#include "../ui/console_ui.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <string>

void GameActions::processEffect(Player& player, const std::string& itemName,
    const std::vector<Effect>& effects) {
    auto it = std::find_if(effects.begin(), effects.end(),
        [&](const Effect& e) { return e.name == itemName; });

    if (it != effects.end()) {
        const Effect& effect = *it;
        if (effect.type == "базовый") {
            if (effect.name == "Деталь") {
                if (player.canFindPart()) {
                    player.shipParts++;
                    std::string key = "floor_" + std::to_string(player.currentFloor) +
                        "_room_" + std::to_string(player.currentRoom);
                    player.foundParts[key] = true;
                    UI::printColored("Найдена деталь корабля! (" +
                        std::to_string(player.shipParts) + "/7)\n", 11);
                }
                else {
                    UI::printColored("Здесь вы всё уже нашли!\n", 13);
                }
            }
            else if (effect.name == "Флакон Внезапной Силы") {
                player.suddenStrengthPotions++;
                UI::printColored("Получен Флакон Внезапной Силы! (Теперь: " +
                    std::to_string(player.suddenStrengthPotions) + ")\n", 13);
            }
            else if (effect.name == "Кристалл Жизни") {
                player.lifeCrystals++;
                UI::printColored("Получен Кристалл Жизни! (Теперь: " +
                    std::to_string(player.lifeCrystals) + ")\n", 13);
            }
            else if (effect.healthEffect > 0) {
                player.heal(effect.healthEffect);
                UI::printColored("Получено " + std::to_string(effect.healthEffect) + " HP\n", 10);
            }
            else if (effect.healthEffect < 0) {
                player.takeDamage(-effect.healthEffect);
                UI::printColored("Получено " + std::to_string(-effect.healthEffect) + " урона\n", 12);
            }
        }
        else if (effect.type == "артефакт") {
            if (!player.canFindArtifact()) {
                UI::printColored("Здесь вы всё уже нашли!\n", 13);
                return;
            }

            // Сохраняем старые значения
            int oldStats[4] = {
                player.strength,
                player.agility,
                player.intellect,
                player.endurance
            };

            // Применяем изменения с проверкой и ограничением до 70%
            if (effect.strengthEffect != 0) {
                int newStrength = player.strength + effect.strengthEffect;
                player.strength = std::min(newStrength, 70);
            }
            if (effect.agilityEffect != 0) {
                int newAgility = player.agility + effect.agilityEffect;
                player.agility = std::min(newAgility, 70);
            }
            if (effect.intellectEffect != 0) {
                int newIntellect = player.intellect + effect.intellectEffect;
                player.intellect = std::min(newIntellect, 70);
            }
            if (effect.enduranceEffect != 0) {
                int newEndurance = player.endurance + effect.enduranceEffect;
                player.endurance = std::min(newEndurance, 70);
            }

            // Выводим только реально измененные характеристики
            UI::printColored("\nНайден артефакт: " + effect.name + "\n", 6);

            if (effect.strengthEffect != 0)
                UI::printColored("СИЛ: " + std::to_string(oldStats[0]) + " > " + std::to_string(player.strength) + "\n", 14);
            if (effect.agilityEffect != 0)
                UI::printColored("ЛОВ: " + std::to_string(oldStats[1]) + " > " + std::to_string(player.agility) + "\n", 14);
            if (effect.intellectEffect != 0)
                UI::printColored("ИНТ: " + std::to_string(oldStats[2]) + " > " + std::to_string(player.intellect) + "\n", 14);
            if (effect.enduranceEffect != 0)
                UI::printColored("ВЫН: " + std::to_string(oldStats[3]) + " > " + std::to_string(player.endurance) + "\n", 14);

            // Вывод сопротивлений с ограничением до 70%
            if (effect.fireResistance > 0 && effect.name == "Кольцо Огнестойкости") {
                int newResistance = std::min(effect.fireResistance, 70);
                UI::printColored("Огонь +" + std::to_string(newResistance) + "%\n", 12);
            }
            if (effect.poisonResistance > 0 && effect.name == "Амулет Ядостойкости") {
                int newResistance = std::min(effect.poisonResistance, 70);
                UI::printColored("Яд +" + std::to_string(newResistance) + "%\n", 10);
            }
            if (effect.earthResistance > 0 && effect.name == "Перчатки Землестойкости") {
                int newResistance = std::min(effect.earthResistance, 70);
                UI::printColored("Земля +" + std::to_string(newResistance) + "%\n", 6);
            }
            if (effect.waterResistance > 0 && effect.name == "Гидрофобный плащ") {
                int newResistance = std::min(effect.waterResistance, 70);
                UI::printColored("Вода +" + std::to_string(newResistance) + "%\n", 9);
            }

            // Сохраняем факт нахождения артефакта
            std::string key = "floor_" + std::to_string(player.currentFloor) + "_room_" + std::to_string(player.currentRoom);
            player.foundArtifacts[key] = true;

            // Выводим итоговые характеристики
            UI::printColored("\nТекущие характеристики:\n", 11);
            UI::printPlayerStats(player);
        }
    }
}

bool GameActions::checkActionSuccess(const Player& player, const std::string& statName) {
    int statValue = 0;
    if (statName == "СИЛ") statValue = player.strength;
    else if (statName == "ЛОВ") statValue = player.agility;
    else if (statName == "ИНТ") statValue = player.intellect;
    else if (statName == "ВЫН") statValue = player.endurance;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 100);
    return (dist(gen) <= statValue);
}

void GameActions::handleRoomAction(Player& player, GameWorld& world) {
    const FloorData& currentFloor = world.getCurrentFloor();
    Room& currentRoom = world.getCurrentRoomMutable();

    UI::printColored("=== " + std::to_string(world.getCurrentFloorIndex() + 1) + " этаж ===\n", 11);
    UI::printRoomDescription(currentRoom);
    UI::waitForEnter();

    // Генерация ID действий
    std::vector<std::string> fullActionIds;
    std::vector<std::string> availableActions;

    for (const auto& actionId : currentRoom.actionIds) {
        std::string fullId = std::to_string(world.getCurrentFloorIndex() + 1) + "_" +
            std::to_string(currentRoom.id) + "_" +
            actionId.substr(actionId.find_last_of('_') + 1);

        if (currentRoom.completedActions.find(fullId) == currentRoom.completedActions.end()) {
            fullActionIds.push_back(fullId);
            availableActions.push_back(actionId);
        }
    }

    // Проверка на базовые действия (без изменений)
    if (availableActions.empty()) {
        bool hasBasicActions = false;
        for (const auto& actionId : currentRoom.actionIds) {
            std::string fullId = std::to_string(world.getCurrentFloorIndex() + 1) + "_" +
                std::to_string(currentRoom.id) + "_" +
                actionId.substr(actionId.find_last_of('_') + 1);

            const Action& action = currentFloor.actionMap.at(fullId);
            auto effectIt = std::find_if(currentFloor.effects.begin(), currentFloor.effects.end(),
                [&](const Effect& e) { return e.name == action.success.item; });

            if (action.success.item.empty() ||
                (effectIt != currentFloor.effects.end() &&
                    effectIt->type != "артефакт" &&
                    action.success.item != "Деталь")) {
                hasBasicActions = true;
                break;
            }
        }

        if (hasBasicActions) {
            for (const auto& actionId : currentRoom.actionIds) {
                std::string fullId = std::to_string(world.getCurrentFloorIndex() + 1) + "_" +
                    std::to_string(currentRoom.id) + "_" +
                    actionId.substr(actionId.find_last_of('_') + 1);

                const Action& action = currentFloor.actionMap.at(fullId);
                bool isBasicAction = action.success.item.empty();
                if (!isBasicAction) {
                    auto effectIt = std::find_if(currentFloor.effects.begin(), currentFloor.effects.end(),
                        [&](const Effect& e) { return e.name == action.success.item; });

                    if (effectIt != currentFloor.effects.end()) {
                        isBasicAction = (action.success.item != "Деталь" && effectIt->type != "артефакт");
                    }
                }

                if (isBasicAction) {
                    currentRoom.completedActions.erase(fullId);
                    fullActionIds.push_back(fullId);
                    availableActions.push_back(actionId);
                }
            }

            if (!availableActions.empty()) {
                UI::printColored("\nВы можете повторить некоторые действия здесь\n", 11);
            }
        }

        // Единственное место, где выводится "Вы все проверили здесь!"
        if (availableActions.empty()) {
            UI::printColored("\nВы все проверили здесь!\n", 13);
            UI::waitForEnter();
            moveToNextLocation(player, world);
            return;
        }
    }

    // Показ доступных действий
    UI::printAvailableActions(player, fullActionIds, currentFloor.actionMap);
    std::cout << (fullActionIds.size() + 1) << ". ";
    UI::printColored("Перейти дальше", 7);
    std::cout << std::endl;

    // Обработка выбора игрока
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        UI::printColored("Неверный ввод! Пожалуйста, введите число.\n", 12);
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice == static_cast<int>(fullActionIds.size()) + 1) {
        moveToNextLocation(player, world);
        return;
    }

    if (choice > 0 && choice <= static_cast<int>(fullActionIds.size())) {
        const Action& action = currentFloor.actionMap.at(fullActionIds[choice - 1]);
        bool success = GameActions::checkActionSuccess(player, action.requiredStat);

        showActionResults(player, action, success, currentFloor);
        currentRoom.completedActions.insert(fullActionIds[choice - 1]);

        // Проверяем, остались ли еще действия
        bool hasMoreActions = false;
        for (const auto& actionId : currentRoom.actionIds) {
            std::string fullId = std::to_string(world.getCurrentFloorIndex() + 1) + "_" +
                std::to_string(currentRoom.id) + "_" +
                actionId.substr(actionId.find_last_of('_') + 1);
            if (currentRoom.completedActions.find(fullId) == currentRoom.completedActions.end()) {
                hasMoreActions = true;
                break;
            }
        }

        // Если действий не осталось - переходим дальше (без повторного сообщения)
        if (!hasMoreActions) {
            UI::printColored("\nВы все проверили здесь!\n", 13);
            moveToNextLocation(player, world);
        }
    }
    else {
        UI::printColored("Неверный выбор!\n", 12);
        UI::waitForEnter();
    }
}

void GameActions::moveToNextLocation(Player& player, GameWorld& world) {
    // Сбрасываем completedActions для текущей комнаты при выходе
    Room& currentRoom = world.getCurrentRoomMutable();
    currentRoom.completedActions.clear();

    player.currentRoom++;
    if (player.currentRoom >= world.getCurrentFloor().rooms.size()) {
        player.currentRoom = 0;
        player.currentFloor++;
        if (player.currentFloor >= world.getFloorCount()) {
            player.currentFloor = 0;
        }
    }
    player.actionsTaken = 0;
    world.setCurrentPosition(player.currentFloor, player.currentRoom);
}

void GameActions::showActionResults(Player& player, const Action& action,
    bool success, const FloorData& floor) {
    UI::printColored("=== " + std::to_string(player.currentFloor + 1) + " этаж ===\n", 11);

    if (success) {
        UI::printColored("\n=== УСПЕХ ===\n", 10);
        UI::printMultiline(action.success.description, 15, true);
        std::cout << std::endl;

        if (!action.success.item.empty()) {
            GameActions::processEffect(player, action.success.item, floor.effects);
        }
    }
    else {
        UI::printColored("\n=== ПРОВАЛ ===\n", 12);
        UI::printMultiline(action.failure.description, 15, true);
        std::cout << std::endl;

        if (!action.failure.item.empty()) {
            if (action.failure.item == "бой") {
                UI::printColored("\nПриготовьтесь к бою!\n", 12);
                if (!floor.monsters.empty()) {
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<size_t> dist(0, floor.monsters.size() - 1);
                    Monster monster = floor.monsters[dist(gen)];
                    if (!CombatSystem::fight(player, monster, floor.effects)) {
                        return;
                    }
                }
            }
            else {
                UI::printColored("\nПоследствия: ", 13);
                UI::printColored(action.failure.item + "\n", 14);
                GameActions::processEffect(player, action.failure.item, floor.effects);
            }
        }
    }
}