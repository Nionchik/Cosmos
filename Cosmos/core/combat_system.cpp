#include <algorithm>
#include <iostream>
#include <limits>
#include <random>

#include "../ui/console_ui.h"
#include "combat_system.h"

bool CombatSystem::Fight(Player& player, const Monster& original_monster,
  const std::vector<Effect>& effects,
  const std::vector<Weapon>& weapons,
  const CombatConfig& config) {

  Monster monster = original_monster;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> melee_dist(
    config.min_melee_damage,
    config.max_melee_damage
  );
  std::uniform_int_distribution<int> monster_dist(
    config.min_monster_damage,
    config.max_monster_damage
  );
  std::uniform_int_distribution<int> crit_chance_dist(1, 100);

  auto ElementToString = [](Element e) {
    switch (e) {
    case Element::kEarth:
      return "�����";
    case Element::kFire:
      return "�����";
    case Element::kPoison:
      return "��";
    case Element::kWater:
      return "����";
    default:
      return "���";
    }
    };

  while (player.health > 0 && monster.health > 0) {
    ui::PrintColored("\n=== ���������� � ������� ===\n", 6);
    ui::PrintColored("���: " + monster.name + "\n", 14);
    ui::PrintColored(std::string("������: ") +
      ElementToString(monster.element) + "\n", 12);
    ui::PrintColored(
      (std::string("��������: ") + ElementToString(monster.weakness) + "\n\n")
      .c_str(), 11);
    ui::PrintColored(monster.name + ": " + std::to_string(monster.health) +
      " HP\n", 12);
    ui::PrintColored("���� ��������: " + std::to_string(player.health) +
      " HP\n\n", 10);

    ui::PrintColored("�������� ��������:\n", 14);
    ui::PrintColored("1. ��������� �������\n", 14);
    ui::PrintColored("2. ������������ ����� (���������: " +
      std::to_string(player.life_crystals) +
      ", �������: " + std::to_string(player.sudden_strength_potions) +
      ")\n", 14);
    ui::PrintColored("3. ������� ���\n", 14);

    size_t choice;
    if (!(std::cin >> choice)) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      ui::PrintColored("�������� ����! ����������, ������� �����.\n", 12);
      continue;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice < 1 || choice > 3) {
      ui::PrintColored("�������� �����! ����������, �������� �������� "
        "�� ������.\n",
        12);
      continue;
    }

    if (choice == 1) {
      ui::PrintColored("\n�������� ������:\n", 11);
      for (size_t i = 0; i < weapons.size(); i++) {
        std::cout << (i + 1) << ". " << weapons[i].name << " (";
        switch (weapons[i].element) {
        case Element::kEarth:
          ui::PrintColored("�����", 6);
          break;
        case Element::kFire:
          ui::PrintColored("�����", 12);
          break;
        case Element::kPoison:
          ui::PrintColored("��", 10);
          break;
        case Element::kWater:
          ui::PrintColored("����", 9);
          break;
        default:
          ui::PrintColored("���", 7);
          break;
        }
        std::cout << ")\n";
      }

      size_t weapon_choice;
      if (!(std::cin >> weapon_choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        ui::PrintColored("�������� ����! ����������, ������� �����.\n", 12);
        continue;
      }
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

      if (weapon_choice < 1 || weapon_choice > weapons.size()) {
        ui::PrintColored("�������� ����� ������!\n", 12);
        continue;
      }

      const Weapon& weapon = weapons[weapon_choice - 1];
      int damage = 0;
      bool is_critical = false;

      if (weapon.element == monster.weakness) {
        std::uniform_int_distribution<int> dmg_dist(weapon.min_weak_damage,
          weapon.max_weak_damage);
        damage = dmg_dist(gen);

        if (crit_chance_dist(gen) <= weapon.crit_chance) {
          damage *= 3;
          is_critical = true;
        }
      }
      else if (weapon.element == monster.element) {
        damage = 0;
      }
      else {
        std::uniform_int_distribution<int> dmg_dist(weapon.min_neutral_damage,
          weapon.max_neutral_damage);
        damage = dmg_dist(gen);

        if (crit_chance_dist(gen) <= weapon.crit_chance) {
          damage *= 3;
          is_critical = true;
        }
      }

      if (is_critical) {
        ui::PrintColored("����������� ����! ", 12);
      }
      monster.TakeDamage(damage);
      ui::PrintColored("�� ������� " + std::to_string(damage) +
        " �����!\n", 10);
      player.TakeDamage(4);
      ui::PrintColored("�� �������� 4 ����� �� ������ ������.\n", 12);
    }
    else if (choice == 2) {
      ui::PrintColored("\n�������� �����:\n", 14);
      ui::PrintColored("1. �������� ����� (+20 HP)\n", 14);
      ui::PrintColored("2. ������ ��������� ���� (-20 HP �������)\n", 14);

      size_t potion_choice;
      if (!(std::cin >> potion_choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        ui::PrintColored("�������� ����! ����������, ������� �����.\n", 12);
        continue;
      }
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

      if (potion_choice < 1 || potion_choice > 2) {
        ui::PrintColored("�������� ����� �����!\n", 12);
        ui::WaitForEnter();
        continue;
      }

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
          ui::PrintColored("�� ������������ ������ ��������� ����! ������ "
            "������� 20 �����.\n", 10);
        }
        else {
          ui::PrintColored("� ��� ��� �������� ��������� ����!\n", 12);
        }
      }
      ui::WaitForEnter();
      continue;
    }
    else if (choice == 3) {
      int damage = melee_dist(gen);
      bool is_critical = false;

      if (crit_chance_dist(gen) <= config.melee_crit_chance) {
        damage *= 3;
        is_critical = true;
        ui::PrintColored("����������� ����! ", 12);
      }

      monster.TakeDamage(damage);
      ui::PrintColored("�� ������� " + std::to_string(damage) +
        " ����� ������� ����!\n", 10);
    }

    if (monster.health > 0) {
      int damage = monster_dist(gen);
      int resistance = 0;

      switch (monster.element) {
      case Element::kEarth:
        resistance += player.earth_resistance;
        break;
      case Element::kFire:
        resistance += player.fire_resistance;
        break;
      case Element::kPoison:
        resistance += player.poison_resistance;
        break;
      case Element::kWater:
        resistance += player.water_resistance;
        break;
      default:
        break;
      }

      for (const auto& effect : effects) {
        if (effect.type == "��������" &&
          player.found_artifacts.count("floor_" +
            std::to_string(player.current_floor) +
            "_room_" +
            std::to_string(player.current_room))) {
          switch (monster.element) {
          case Element::kEarth:
            resistance += effect.earth_resistance;
            break;
          case Element::kFire:
            resistance += effect.fire_resistance;
            break;
          case Element::kPoison:
            resistance += effect.poison_resistance;
            break;
          case Element::kWater:
            resistance += effect.water_resistance;
            break;
          default:
            break;
          }
        }
      }

      resistance = std::min(resistance, 80);
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