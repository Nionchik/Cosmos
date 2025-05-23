//combat_system.h
#pragma once
#include "game_objects.h"
#include <vector>

class CombatSystem {
public:
    static bool fight(Player& player, const Monster& originalMonster,
        const std::vector<Effect>& effects);
};