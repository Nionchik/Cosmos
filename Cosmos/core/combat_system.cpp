#include "combat_system.h"
#include "../ui/console_ui.h"
#include <random>
#include <algorithm>
#include <limits>
#include <iostream>

bool CombatSystem::fight(Player& player, const Monster& originalMonster,
    const std::vector<Effect>& effects) {
    Monster monster = originalMonster;  // ������ ����� ��� ���
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> damageDist(5, 8); // ��� ������������ ������
    std::uniform_int_distribution<int> critDist(14, 17); // ��� ������ ��������
    std::uniform_int_distribution<int> meleeDist(7, 10); // ��� �������� ���
    std::uniform_int_distribution<int> monsterDist(9, 12); // ��� ����� �������
    std::uniform_int_distribution<int> critChance(1, 100); // ��� �������� �����

    while (player.health > 0 && monster.health > 0) {
        // ����������� ���������� � �������
        UI::printColored("\n=== ���������� � ������� ===\n", 6);
        UI::printColored("���: " + monster.name + "\n", 14);

        // ����������� ������ �������
        std::string elementStr;
        switch (monster.element) {
        case Element::EARTH: elementStr = "�����"; break;
        case Element::FIRE: elementStr = "�����"; break;
        case Element::POISON: elementStr = "��"; break;
        case Element::WATER: elementStr = "����"; break;
        default: elementStr = "���";
        }
        UI::printColored("������: " + elementStr + "\n", 12);

        // ����������� ���������� �������
        std::string weaknessStr;
        switch (monster.weakness) {
        case Element::EARTH: weaknessStr = "�����"; break;
        case Element::FIRE: weaknessStr = "�����"; break;
        case Element::POISON: weaknessStr = "��"; break;
        case Element::WATER: weaknessStr = "����"; break;
        default: weaknessStr = "���";
        }
        UI::printColored("��������: " + weaknessStr + "\n\n", 11);

        // ����������� ��������
        UI::printColored(monster.name + ": " + std::to_string(monster.health) + " HP\n", 12);
        UI::printColored("���� ��������: " + std::to_string(player.health) + " HP\n\n", 10);

        // ����� ��������
        UI::printColored("�������� ��������:\n", 14);
        UI::printColored("1. ��������� �������\n", 14);
        UI::printColored("2. ������������ ����� (���������: " + std::to_string(player.lifeCrystals) +
            ", �������: " + std::to_string(player.suddenStrengthPotions) + ")\n", 14);
        UI::printColored("3. ������� ���\n", 14);

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            UI::printColored("�������� ����! ����������, ������� �����.\n", 12);
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // ��������� ������ ������
        if (choice == 1) {
            UI::printColored("\n�������� ������:\n", 11);
            UI::printColored("1. ����������� (�����)\n", 11);
            UI::printColored("2. ����������� (��)\n", 11);
            UI::printColored("3. ������������ (�����)\n", 11);
            UI::printColored("4. ����������� (����)\n", 11);

            int weaponChoice;
            if (!(std::cin >> weaponChoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                UI::printColored("�������� ����! ����������, ������� �����.\n", 12);
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
                damage = critDist(gen); // ������� ���� 14-17
                // �������� �� ����������� ���� (35% ����)
                if (critChance(gen) <= 35) {
                    damage *= 3;
                    isCritical = true;
                }
            }
            else if (weaponElement == monster.element) {
                damage = 0; // ������� ����
            }
            else {
                damage = damageDist(gen); // ������� ���� 5-8
                // �������� �� ����������� ���� (35% ����)
                if (critChance(gen) <= 35) {
                    damage *= 3;
                    isCritical = true;
                }
            }

            if (isCritical) {
                UI::printColored("����������� ����! ", 12);
            }
            monster.takeDamage(damage);
            UI::printColored("�� ������� " + std::to_string(damage) + " �����!\n", 10);
            player.takeDamage(4); // ���� �� ������ ������
            UI::printColored("�� �������� 4 ����� �� ������ ������.\n", 12);
        }
        else if (choice == 2) {
            UI::printColored("\n�������� �����:\n", 14);
            UI::printColored("1. �������� ����� (+20 HP)\n", 14);
            UI::printColored("2. ������ ��������� ���� (-20 HP �������)\n", 14);

            int potionChoice;
            if (!(std::cin >> potionChoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                UI::printColored("�������� ����! ����������, ������� �����.\n", 12);
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (potionChoice == 1) {
                if (player.lifeCrystals > 0) {
                    player.heal(20);
                    player.lifeCrystals--;
                    UI::printColored("����������� �������� ����� (+20 HP)\n", 10);
                }
                else {
                    UI::printColored("��� ���������� �����!\n", 12);
                }
            }
            else if (potionChoice == 2) {
                if (player.suddenStrengthPotions > 0) {
                    player.suddenStrengthPotions--;
                    monster.takeDamage(20);
                    UI::printColored("�� ������������ ������ ��������� ����! ������ ������� 20 �����.\n", 10);
                }
                else {
                    UI::printColored("� ��� ��� �������� ��������� ����!\n", 12);
                }
            }

            // ����� ������������� ����� ����� ������ ������� ������
            UI::printColored("\n������ �������� ������ ��� �����:\n", 11);
            UI::printColored("1. ����������� (�����)\n", 11);
            UI::printColored("2. ����������� (��)\n", 11);
            UI::printColored("3. ������������ (�����)\n", 11);
            UI::printColored("4. ����������� (����)\n", 11);
            UI::printColored("5. ������� ���\n", 11);

            int attackChoice;
            if (!(std::cin >> attackChoice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                UI::printColored("�������� ����! ����������, ������� �����.\n", 12);
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
                UI::printColored("�� ������� " + std::to_string(damage) + " �����!\n", 10);
                player.takeDamage(4); // ���� �� ������ ������
                UI::printColored("�� �������� 4 ����� �� ������ ������.\n", 12);
            }
            else if (attackChoice == 5) {
                damage = meleeDist(gen);
                monster.takeDamage(damage);
                UI::printColored("�� ������� " + std::to_string(damage) + " ����� ������� ����!\n", 10);
            }
            else {
                UI::printColored("�������� �����! ���������� �����.\n", 12);
            }
        }
        else if (choice == 3) {
            int damage = meleeDist(gen);
            monster.takeDamage(damage);
            UI::printColored("�� ������� " + std::to_string(damage) + " ����� ������� ����!\n", 10);
        }
        else {
            UI::printColored("�������� �����! ����������, �������� �������� �� ������.\n", 12);
            continue;
        }

        // ����� �������, ���� �� ��� ���
        if (monster.health > 0) {
            int damage = monsterDist(gen);
            int resistance = 10;

            // ������ ������������� ������
            for (const auto& effect : effects) {
                if (effect.type == "��������" &&
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

            resistance = std::min(resistance, 70); // ����������� ������������� �� 70%
            damage = damage * (100 - resistance) / 100;
            player.takeDamage(damage);
            UI::printColored("\n" + monster.name + " ������� ��� � ������� " +
                std::to_string(damage) + " �����! (�������������: " +
                std::to_string(resistance) + "%)\n", 12);
            UI::waitForEnter();
        }
    }

    if (monster.health <= 0) {
        UI::printColored("\n�� �������� " + monster.name + "!\n", 10);
        UI::waitForEnter();
        return true;
    }
    return false;
}