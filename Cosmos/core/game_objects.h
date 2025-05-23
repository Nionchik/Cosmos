//game_objects.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <utility>

enum class Element { EARTH, FIRE, POISON, WATER, NONE };

struct ActionResult {
    std::string description;
    std::string item;

    ActionResult(const std::string& desc = "", const std::string& itm = "");
};

class Effect {
public:
    std::string type;
    std::string name;
    int healthEffect;
    int strengthEffect;
    int agilityEffect;
    int intellectEffect;
    int enduranceEffect;
    int earthResistance;
    int fireResistance;
    int poisonResistance;
    int waterResistance;

    Effect(const std::string& type, const std::string& name, int health,
        int str, int agi, int intl, int end,
        int earth, int fire, int poison, int water);
};

class Monster {
public:
    std::string name;
    int health;
    Element element;
    Element weakness;

    Monster(const std::string& name, int health, Element element, Element weakness);
    void takeDamage(int damage);
};

class Action {
public:
    std::string id;
    std::string description;
    std::string requiredStat;
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
    std::vector<std::string> actionIds;
    std::unordered_set<std::string> completedActions;

    Room(int id = -1,
        const std::string& name = "",
        const std::string& desc = "",
        const std::vector<std::string>& actionIds = {});

    void resetActions() { completedActions.clear(); }
};

class Player {
public:
    int health;
    int shipParts;
    int strength;
    int agility;
    int intellect;
    int endurance;
    int suddenStrengthPotions;
    int lifeCrystals;
    int currentFloor;
    int currentRoom;
    int actionsTaken;
    std::map<std::string, bool> foundParts;
    std::map<std::string, bool> foundArtifacts;

    Player();
    void takeDamage(int damage);
    void heal(int amount);
    void addItem(const std::string& item);
    void useLifeCrystal();
    void useSuddenStrengthPotion();
    void increaseStat(const std::string& stat, int amount);
    bool canFindPart() const;
    bool canFindArtifact() const;
};