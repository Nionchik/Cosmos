// game_objects.h
#pragma once
#include <map>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

enum class Element { kEarth, kFire, kPoison, kWater, kNone };

struct ActionResult {
    std::string description;
    std::string item;

    ActionResult(const std::string& desc = "",
        const std::string& itm = "");
};

class Effect {
public:
    std::string type;
    std::string name;
    int health_effect;
    int strength_effect;
    int agility_effect;
    int intellect_effect;
    int endurance_effect;
    int earth_resistance;
    int fire_resistance;
    int poison_resistance;
    int water_resistance;

    Effect(const std::string& type, const std::string& name, int health_effect,
        int strength_effect, int agility_effect, int intellect_effect,
        int endurance_effect, int earth_resistance, int fire_resistance,
        int poison_resistance, int water_resistance);
};

class Monster {
public:
    std::string name;
    int health;
    Element element;
    Element weakness;

    Monster(const std::string& name, int health, Element element, Element weakness);
    void TakeDamage(int damage);
};

class Action {
public:
    std::string id;
    std::string description;
    std::string required_stat;
    ActionResult success;
    ActionResult failure;

    Action(const std::string& id = "",
        const std::string& desc = "",
        const std::string& stat = "NONE",
        const ActionResult& succ = ActionResult(),
        const ActionResult& fail = ActionResult());
};

class Room {
public:
    int id;
    std::string name;
    std::string description;
    std::vector<std::string> action_ids;
    std::unordered_set<std::string> completed_actions;
    bool first_visit = true;

    Room(int id = -1,
        const std::string& name = "",
        const std::string& desc = "",
        const std::vector<std::string>& action_ids = {});
};

class Player {
public:
    int health = 0;
    int ship_parts = 0;
    int strength = 0;
    int agility = 0;
    int intellect = 0;
    int endurance = 0;
    int sudden_strength_potions = 0;
    int life_crystals = 0;
    int current_floor = 0;
    int current_room = 0;
    int actions_taken = 0;
    int earth_resistance = 0;
    int fire_resistance = 0;
    int poison_resistance = 0;
    int water_resistance = 0;
    std::map<std::string, bool> found_parts;
    std::map<std::string, bool> found_artifacts;

    Player();
    void TakeDamage(int damage);
    void Heal(int amount);
    void AddItem(const std::string& item);
    void UseLifeCrystal();
    void UseSuddenStrengthPotion();
    void IncreaseStat(const std::string& stat, int amount);
    bool CanFindPart() const;
    bool CanFindArtifact() const;
};