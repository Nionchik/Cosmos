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
        case Element::kEarth: return "�����";
        case Element::kFire: return "�����";
        case Element::kPoison: return "��";
        case Element::kWater: return "����";
        default: return "���";
        }
        };

    while (player.health > 0 && monster.health > 0) {
        // ����������� ���������� � �������
        ui::PrintColored("\n=== ���������� � ������� ===\n", 6);
        ui::PrintColored("���: " + monster.name + "\n", 14);

        // ������������ ����� ������ � ��������
        ui::PrintColored(std::string("������: ") + ElementToString(monster.element) + "\n", 12);
        ui::PrintColored((std::string("��������: ") + ElementToString(monster.weakness) + "\n\n").c_str(), 11);

        // ����������� ��������
        ui::PrintColored(monster.name + ": " + std::to_string(monster.health) + " HP\n", 12);
        ui::PrintColored("���� ��������: " + std::to_string(player.health) + " HP\n\n", 10);

        // ����� ��������
        ui::PrintColored("�������� ��������:\n", 14);
        ui::PrintColored("1. ��������� �������\n", 14);
        ui::PrintColored("2. ������������ ����� (���������: " +
            std::to_string(player.life_crystals) +
            ", �������: " + std::to_string(player.sudden_strength_potions) + ")\n", 14);
        ui::PrintColored("3. ������� ���\n", 14);

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            ui::PrintColored("�������� ����! ����������, ������� �����.\n", 12);
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // ��������� ������ ������
        if (choice == 1) {
            ui::PrintColored("\n�������� ������:\n", 11);
            ui::PrintColored("1. ����������� (�����)\n", 11);
            ui::PrintColored("2. ����������� (��)\n", 11);
            ui::PrintColored("3. ������������ (�����)\n", 11);
            ui::PrintColored("4. ����������� (����)\n", 11);

            int weapon_choice;
            if (!(std::cin >> weapon_choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                ui::PrintColored("�������� ����! ����������, ������� �����.\n", 12);
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
                damage = crit_dist(gen); // ������� ���� 14-17
                // �������� �� ����������� ���� (35% ����)
                if (crit_chance(gen) <= 35) {
                    damage *= 3;
                    is_critical = true;
                }
            }
            else if (weapon_element == monster.element) {
                damage = 0; // ������� ����
            }
            else {
                damage = damage_dist(gen); // ������� ���� 5-8
                // �������� �� ����������� ���� (35% ����)
                if (crit_chance(gen) <= 35) {
                    damage *= 3;
                    is_critical = true;
                }
            }

            if (is_critical) {
                ui::PrintColored("����������� ����! ", 12);
            }
            monster.TakeDamage(damage);
            ui::PrintColored("�� ������� " + std::to_string(damage) + " �����!\n", 10);
            player.TakeDamage(4); // ���� �� ������ ������
            ui::PrintColored("�� �������� 4 ����� �� ������ ������.\n", 12);
        }
        else if (choice == 2) {
            ui::PrintColored("\n�������� �����:\n", 14);
            ui::PrintColored("1. �������� ����� (+20 HP)\n", 14);
            ui::PrintColored("2. ������ ��������� ���� (-20 HP �������)\n", 14);

            int potion_choice;
            if (!(std::cin >> potion_choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                ui::PrintColored("�������� ����! ����������, ������� �����.\n", 12);
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (potion_choice == 1) {
                if (player.life_crystals > 0) {
                    player.Heal(20);
                    player.life_crystals--;
                    ui::PrintColored("����������� �������� ����� (+20 HP)\n", 10);
                }
                else {
                    ui::PrintColored("��� ���������� �����!\n", 12);
                }
            }
            else if (potion_choice == 2) {
                if (player.sudden_strength_potions > 0) {
                    player.sudden_strength_potions--;
                    monster.TakeDamage(20);
                    ui::PrintColored("�� ������������ ������ ��������� ����! ������ ������� 20 �����.\n", 10);
                }
                else {
                    ui::PrintColored("� ��� ��� �������� ��������� ����!\n", 12);
                }
            }

            // ����� ������������� ����� ������ ���������� ����
            ui::WaitForEnter();
            continue;
        }
        else if (choice == 3) {
            int damage = melee_dist(gen);
            monster.TakeDamage(damage);
            ui::PrintColored("�� ������� " + std::to_string(damage) + " ����� ������� ����!\n", 10);
        }
        else {
            ui::PrintColored("�������� �����! ����������, �������� �������� �� ������.\n", 12);
            continue;
        }

        // ����� �������, ���� �� ��� ���
        if (monster.health > 0) {
            int damage = monster_dist(gen);
            int resistance = 0;

            // ��������� ������������� �� ������������� ������
            switch (monster.element) {
            case Element::kEarth: resistance += player.earth_resistance; break;
            case Element::kFire: resistance += player.fire_resistance; break;
            case Element::kPoison: resistance += player.poison_resistance; break;
            case Element::kWater: resistance += player.water_resistance; break;
            default: break;
            }

            // ��������� ������ �� ����������
            for (const auto& effect : effects) {
                if (effect.type == "��������" &&
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

            resistance = std::min(resistance, 80); // ����������� ������������� �� 80%
            damage = damage * (100 - resistance) / 100;
            player.TakeDamage(damage);
            ui::PrintColored("\n" + monster.name + " ������� ��� � ������� " +
                std::to_string(damage) + " �����! (�������������: " +
                std::to_string(resistance) + "%)\n", 12);
            ui::WaitForEnter();
        }
    }

    if (monster.health <= 0) {
        ui::PrintColored("\n�� �������� " + monster.name + "!\n", 10);
        return true;
    }
    return false;
}