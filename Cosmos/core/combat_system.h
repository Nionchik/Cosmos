#pragma once
#include <vector>

#include "game_objects.h"

class CombatSystem {
public:
  static bool Fight(Player& player, const Monster& original_monster,
    const std::vector<Effect>& effects,
    const std::vector<Weapon>& weapons,
    const CombatConfig& config);
};