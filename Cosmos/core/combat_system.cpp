// combat_system.cpp
#include <algorithm>
#include <iostream>
#include <limits>
#include <random>

#include "../ui/console_ui.h"
#include "combat_system.h"

bool CombatSystem::Fight(Player& player, const Monster& original_monster,
    const std::vector<Effect>& effects) {

    Monster monster = original_monster;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> damage_dist(5, 8);
    std::uniform_int_distribution<int> crit_dist(14, 17);
    std::uniform_int_distribution<int> melee_dist(7, 10);
    std::uniform_int_distribution<int> monster_dist(9, 12);
    std::uniform_int_distribution<int> crit_chance(1, 100);

    auto ElementToString = [](Element e) {
        switch (e) {
        case Element::kEarth: return "Земля";
        case Element::kFire: return "Огонь";
        case Element::kPoison: return "Яд";
        case Element::kWater: return "Вода";
        default: return "Нет";
        }
        };

    while (player.health > 0 && monster.health > 0) {
        // Отображение информации о монстре
        ui::PrintColored("\n=== ИНФОРМАЦИЯ О МОНСТРЕ ===\n", 6);
        ui::PrintColored("Имя: " + monster.name + "\n", 14);

        // Исправленный вывод стихии и слабости
        ui::PrintColored(std::string("Стихия: ") + ElementToString(monster.element) + "\n", 12);
        ui::PrintColored((std::string("Слабость: ") + ElementToString(monster.weakness) + "\n\n").c_str(), 11);

        // Отображение здоровья
        ui::PrintColored(monster.name + ": " + std::to_string(monster.health) + " HP\n", 12);
        ui::PrintColored("Ваше здоровье: " + std::to_string(player.health) + " HP\n\n", 10);

        // Выбор действия
        ui::PrintColored("Выберите действие:\n", 14);
        ui::PrintColored("1. Атаковать оружием\n", 14);
        ui::PrintColored("2. Использовать зелье (Кристаллы: " +
            std::to_string(player.life_crystals) +
            ", Флаконы: " + std::to_string(player.sudden_strength_potions) + ")\n", 14);
        ui::PrintColored("3. Ближний бой\n", 14);

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            ui::PrintColored("Неверный ввод! Пожалуйста, введите число.\n", 12);
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Обработка выбора игрока
        if (choice == 1) {
            ui::PrintColored("\nВыберите оружие:\n", 11);
            ui::PrintColored("1. Абиобластер (Земля)\n", 11);
            ui::PrintColored("2. Омнибластер (Яд)\n", 11);
            ui::PrintColored("3. Нейробластер (Огонь)\n", 11);
            ui::PrintColored("4. Аквабластер (Вода)\n", 11);

            int weapon_choice;
            if (!(std::cin >> weapon_choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                ui::PrintColored("Неверный ввод! Пожалуйста, введите число.\n", 12);
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            int damage = 0;
            Element weapon_element = Element::kNone;
            bool is_critical = false;

            switch (weapon_choice) {
            case 1: weapon_element = Element::kEarth; break;
            case 2: weapon_element = Element::kPoison; break;
            case 3: weapon_element = Element::kFire; break;
            case 4: weapon_element = Element::kWater; break;
            default: weapon_element = Element::kNone;
            }

            if (weapon_element == monster.weakness) {
                damage = crit_dist(gen); // Базовый урон 14-17
                // Проверка на критический удар (35% шанс)
                if (crit_chance(gen) <= 35) {
                    damage *= 3;
                    is_critical = true;
                }
            }
            else if (weapon_element == monster.element) {
                damage = 0; // Нулевой урон
            }
            else {
                damage = damage_dist(gen); // Базовый урон 5-8
                // Проверка на критический удар (35% шанс)
                if (crit_chance(gen) <= 35) {
                    damage *= 3;
                    is_critical = true;
                }
            }

            if (is_critical) {
                ui::PrintColored("Критический удар! ", 12);
            }
            monster.TakeDamage(damage);
            ui::PrintColored("Вы нанесли " + std::to_string(damage) + " урона!\n", 10);
            player.TakeDamage(4); // Урон от отдачи оружия
            ui::PrintColored("Вы получили 4 урона от отдачи оружия.\n", 12);
        }
        else if (choice == 2) {
            ui::PrintColored("\nВыберите зелье:\n", 14);
            ui::PrintColored("1. Кристалл Жизни (+20 HP)\n", 14);
            ui::PrintColored("2. Флакон Внезапной Силы (-20 HP монстру)\n", 14);

            int potion_choice;
            if (!(std::cin >> potion_choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                ui::PrintColored("Неверный ввод! Пожалуйста, введите число.\n", 12);
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (potion_choice == 1) {
                if (player.life_crystals > 0) {
                    player.Heal(20);
                    player.life_crystals--;
                    ui::PrintColored("Использован Кристалл Жизни (+20 HP)\n", 10);
                }
                else {
                    ui::PrintColored("Нет Кристаллов Жизни!\n", 12);
                }
            }
            else if (potion_choice == 2) {
                if (player.sudden_strength_potions > 0) {
                    player.sudden_strength_potions--;
                    monster.TakeDamage(20);
                    ui::PrintColored("Вы использовали Флакон Внезапной Силы! Монстр получил 20 урона.\n", 10);
                }
                else {
                    ui::PrintColored("У вас нет Флаконов Внезапной Силы!\n", 12);
                }
            }

            // После использования зелья просто продолжаем цикл
            ui::WaitForEnter();
            continue;
        }
        else if (choice == 3) {
            int damage = melee_dist(gen);
            monster.TakeDamage(damage);
            ui::PrintColored("Вы нанесли " + std::to_string(damage) + " урона ближним боем!\n", 10);
        }
        else {
            ui::PrintColored("Неверный выбор! Пожалуйста, выберите действие из списка.\n", 12);
            continue;
        }

        // Атака монстра, если он еще жив
        if (monster.health > 0) {
            int damage = monster_dist(gen);
            int resistance = 0;

            // Добавляем сопротивления из характеристик игрока
            switch (monster.element) {
            case Element::kEarth: resistance += player.earth_resistance; break;
            case Element::kFire: resistance += player.fire_resistance; break;
            case Element::kPoison: resistance += player.poison_resistance; break;
            case Element::kWater: resistance += player.water_resistance; break;
            default: break;
            }

            // Добавляем бонусы от артефактов
            for (const auto& effect : effects) {
                if (effect.type == "артефакт" &&
                    player.found_artifacts.count("floor_" + std::to_string(player.current_floor) +
                        "_room_" + std::to_string(player.current_room))) {
                    switch (monster.element) {
                    case Element::kEarth: resistance += effect.earth_resistance; break;
                    case Element::kFire: resistance += effect.fire_resistance; break;
                    case Element::kPoison: resistance += effect.poison_resistance; break;
                    case Element::kWater: resistance += effect.water_resistance; break;
                    default: break;
                    }
                }
            }

            resistance = std::min(resistance, 80); // Ограничение сопротивления до 80%
            damage = damage * (100 - resistance) / 100;
            player.TakeDamage(damage);
            ui::PrintColored("\n" + monster.name + " атакует вас и наносит " +
                std::to_string(damage) + " урона! (Сопротивление: " +
                std::to_string(resistance) + "%)\n", 12);
            ui::WaitForEnter();
        }
    }

    if (monster.health <= 0) {
        ui::PrintColored("\nВы победили " + monster.name + "!\n", 10);
        return true;
    }
    return false;
}