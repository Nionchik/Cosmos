#include "combat_system.h"
#include "../ui/console_ui.h"
#include <random>
#include <algorithm>
#include <limits>
#include <iostream>

bool CombatSystem::fight(Player& player, const Monster& originalMonster,
    const std::vector<Effect>& effects) {
    Monster monster = originalMonster;  // Создаём копию для боя
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> damageDist(5, 8); // Для нейтрального оружия
    std::uniform_int_distribution<int> critDist(14, 17); // Для оружия слабости
    std::uniform_int_distribution<int> meleeDist(7, 10); // Для ближнего боя
    std::uniform_int_distribution<int> monsterDist(9, 12); // Для атаки монстра
    std::uniform_int_distribution<int> critChance(1, 100); // Для проверки крита

    while (player.health > 0 && monster.health > 0) {
        // Отображение информации о монстре
        UI::printColored("\n=== ИНФОРМАЦИЯ О МОНСТРЕ ===\n", 6);
        UI::printColored("Имя: " + monster.name + "\n", 14);

        // Отображение стихии монстра
        std::string elementStr;
        switch (monster.element) {
        case Element::EARTH: elementStr = "Земля"; break;
        case Element::FIRE: elementStr = "Огонь"; break;
        case Element::POISON: elementStr = "Яд"; break;
        case Element::WATER: elementStr = "Вода"; break;
        default: elementStr = "Нет";
        }
        UI::printColored("Стихия: " + elementStr + "\n", 12);

        // Отображение уязвимости монстра
        std::string weaknessStr;
        switch (monster.weakness) {
        case Element::EARTH: weaknessStr = "Земля"; break;
        case Element::FIRE: weaknessStr = "Огонь"; break;
        case Element::POISON: weaknessStr = "Яд"; break;
        case Element::WATER: weaknessStr = "Вода"; break;
        default: weaknessStr = "Нет";
        }
        UI::printColored("Слабость: " + weaknessStr + "\n\n", 11);

        // Отображение здоровья
        UI::printColored(monster.name + ": " + std::to_string(monster.health) + " HP\n", 12);
        UI::printColored("Ваше здоровье: " + std::to_string(player.health) + " HP\n\n", 10);

        // Выбор действия
        UI::printColored("Выберите действие:\n", 14);
        UI::printColored("1. Атаковать оружием\n", 14);
        UI::printColored("2. Использовать зелье (Кристаллы: " + std::to_string(player.lifeCrystals) +
            ", Флаконы: " + std::to_string(player.suddenStrengthPotions) + ")\n", 14);
        UI::printColored("3. Ближний бой\n", 14);

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            UI::printColored("Неверный ввод! Пожалуйста, введите число.\n", 12);
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Обработка выбора игрока
        if (choice == 1) {
            UI::printColored("\nВыберите оружие:\n", 11);
            UI::printColored("1. Абиобластер (Земля)\n", 11);
            UI::printColored("2. Омнибластер (Яд)\n", 11);
            UI::printColored("3. Нейробластер (Огонь)\n", 11);
            UI::printColored("4. Аквабластер (Вода)\n", 11);

            int weaponChoice;
            if (!(std::cin >> weaponChoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                UI::printColored("Неверный ввод! Пожалуйста, введите число.\n", 12);
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            int damage = 0;
            Element weaponElement = Element::NONE;
            bool isCritical = false;

            switch (weaponChoice) {
            case 1: weaponElement = Element::EARTH; break;
            case 2: weaponElement = Element::POISON; break;
            case 3: weaponElement = Element::FIRE; break;
            case 4: weaponElement = Element::WATER; break;
            default: weaponElement = Element::NONE;
            }

            if (weaponElement == monster.weakness) {
                damage = critDist(gen); // Базовый урон 14-17
                // Проверка на критический удар (35% шанс)
                if (critChance(gen) <= 35) {
                    damage *= 3;
                    isCritical = true;
                }
            }
            else if (weaponElement == monster.element) {
                damage = 0; // Нулевой урон
            }
            else {
                damage = damageDist(gen); // Базовый урон 5-8
                // Проверка на критический удар (35% шанс)
                if (critChance(gen) <= 35) {
                    damage *= 3;
                    isCritical = true;
                }
            }

            if (isCritical) {
                UI::printColored("Критический удар! ", 12);
            }
            monster.takeDamage(damage);
            UI::printColored("Вы нанесли " + std::to_string(damage) + " урона!\n", 10);
            player.takeDamage(4); // Урон от отдачи оружия
            UI::printColored("Вы получили 4 урона от отдачи оружия.\n", 12);
        }
        else if (choice == 2) {
            UI::printColored("\nВыберите зелье:\n", 14);
            UI::printColored("1. Кристалл Жизни (+20 HP)\n", 14);
            UI::printColored("2. Флакон Внезапной Силы (-20 HP монстру)\n", 14);

            int potionChoice;
            if (!(std::cin >> potionChoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                UI::printColored("Неверный ввод! Пожалуйста, введите число.\n", 12);
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (potionChoice == 1) {
                if (player.lifeCrystals > 0) {
                    player.heal(20);
                    player.lifeCrystals--;
                    UI::printColored("Использован Кристалл Жизни (+20 HP)\n", 10);
                }
                else {
                    UI::printColored("Нет Кристаллов Жизни!\n", 12);
                }
            }
            else if (potionChoice == 2) {
                if (player.suddenStrengthPotions > 0) {
                    player.suddenStrengthPotions--;
                    monster.takeDamage(20);
                    UI::printColored("Вы использовали Флакон Внезапной Силы! Монстр получил 20 урона.\n", 10);
                }
                else {
                    UI::printColored("У вас нет Флаконов Внезапной Силы!\n", 12);
                }
            }

            // После использования зелья игрок должен выбрать оружие
            UI::printColored("\nТеперь выберите оружие для атаки:\n", 11);
            UI::printColored("1. Абиобластер (Земля)\n", 11);
            UI::printColored("2. Омнибластер (Яд)\n", 11);
            UI::printColored("3. Нейробластер (Огонь)\n", 11);
            UI::printColored("4. Аквабластер (Вода)\n", 11);
            UI::printColored("5. Ближний бой\n", 11);

            int attackChoice;
            if (!(std::cin >> attackChoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                UI::printColored("Неверный ввод! Пожалуйста, введите число.\n", 12);
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            int damage = 0;
            if (attackChoice >= 1 && attackChoice <= 4) {
                Element weaponElement = Element::NONE;
                switch (attackChoice) {
                case 1: weaponElement = Element::EARTH; break;
                case 2: weaponElement = Element::POISON; break;
                case 3: weaponElement = Element::FIRE; break;
                case 4: weaponElement = Element::WATER; break;
                }

                if (weaponElement == monster.weakness) {
                    damage = critDist(gen);
                }
                else if (weaponElement == monster.element) {
                    damage = 0;
                }
                else {
                    damage = damageDist(gen);
                }

                monster.takeDamage(damage);
                UI::printColored("Вы нанесли " + std::to_string(damage) + " урона!\n", 10);
                player.takeDamage(4); // Урон от отдачи оружия
                UI::printColored("Вы получили 4 урона от отдачи оружия.\n", 12);
            }
            else if (attackChoice == 5) {
                damage = meleeDist(gen);
                monster.takeDamage(damage);
                UI::printColored("Вы нанесли " + std::to_string(damage) + " урона ближним боем!\n", 10);
            }
            else {
                UI::printColored("Неверный выбор! Пропускаем атаку.\n", 12);
            }
        }
        else if (choice == 3) {
            int damage = meleeDist(gen);
            monster.takeDamage(damage);
            UI::printColored("Вы нанесли " + std::to_string(damage) + " урона ближним боем!\n", 10);
        }
        else {
            UI::printColored("Неверный выбор! Пожалуйста, выберите действие из списка.\n", 12);
            continue;
        }

        // Атака монстра, если он еще жив
        if (monster.health > 0) {
            int damage = monsterDist(gen);
            int resistance = 10;

            // Расчет сопротивления игрока
            for (const auto& effect : effects) {
                if (effect.type == "артефакт" &&
                    player.foundArtifacts.count("floor_" + std::to_string(player.currentFloor) +
                        "_room_" + std::to_string(player.currentRoom))) {
                    switch (monster.element) {
                    case Element::EARTH: resistance += effect.earthResistance; break;
                    case Element::FIRE: resistance += effect.fireResistance; break;
                    case Element::POISON: resistance += effect.poisonResistance; break;
                    case Element::WATER: resistance += effect.waterResistance; break;
                    default: break;
                    }
                }
            }

            resistance = std::min(resistance, 70); // Ограничение сопротивления до 70%
            damage = damage * (100 - resistance) / 100;
            player.takeDamage(damage);
            UI::printColored("\n" + monster.name + " атакует вас и наносит " +
                std::to_string(damage) + " урона! (Сопротивление: " +
                std::to_string(resistance) + "%)\n", 12);
            UI::waitForEnter();
        }
    }

    if (monster.health <= 0) {
        UI::printColored("\nВы победили " + monster.name + "!\n", 10);
        UI::waitForEnter();
        return true;
    }
    return false;
}