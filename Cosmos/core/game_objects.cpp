//game_objects.cpp
#include "game_objects.h"
#include "../ui/console_ui.h"
#include <algorithm>

ActionResult::ActionResult(const std::string& desc, const std::string& itm)
    : description(desc), item(itm) {
}

Effect::Effect(const std::string& type, const std::string& name, int health,
    int str, int agi, int intl, int end,
    int earth, int fire, int poison, int water)
    : type(type), name(name), healthEffect(health),
    strengthEffect(str), agilityEffect(agi), intellectEffect(intl), enduranceEffect(end),
    earthResistance(earth), fireResistance(fire), poisonResistance(poison), waterResistance(water) {
}

Monster::Monster(const std::string& name, int health, Element element, Element weakness)
    : name(name), health(health), element(element), weakness(weakness) {
}

void Monster::takeDamage(int damage) {
    health = std::max(0, health - damage);
}

Action::Action(const std::string& id, const std::string& desc,
    const std::string& stat, const ActionResult& succ, const ActionResult& fail)
    : id(id), description(desc), success(succ), failure(fail) {
    if (stat == "СИЛ" || stat == "ЛОВ" || stat == "ИНТ" || stat == "ВЫН") {
        requiredStat = stat;
    }
    else {
        requiredStat = "NONE";
    }
}

Room::Room(int id, const std::string& name, const std::string& desc,
    const std::vector<std::string>& actionIds)
    : id(id), name(name), description(desc), actionIds(actionIds) {
}

Player::Player()
    : health(150), shipParts(0), strength(30), agility(30), intellect(30), endurance(30),
    suddenStrengthPotions(1), lifeCrystals(1), currentFloor(0), currentRoom(0), actionsTaken(0) {
}

void Player::takeDamage(int damage) {
    health = std::max(0, health - damage);
}

void Player::heal(int amount) {
    health += amount;
}

void Player::addItem(const std::string& item) {
    if (item == "Флакон Внезапной Силы") suddenStrengthPotions++;
    else if (item == "Кристалл Жизни") lifeCrystals++;
}

void Player::useLifeCrystal() {
    if (lifeCrystals > 0) {
        heal(20);
        lifeCrystals--;
    }
}

void Player::useSuddenStrengthPotion() {
    if (suddenStrengthPotions > 0) {
        suddenStrengthPotions--;
    }
}

void Player::increaseStat(const std::string& stat, int amount) {
    if (amount == 0) return;

    if (stat == "СИЛ") strength = std::min(80, strength + amount);
    else if (stat == "ЛОВ") agility = std::min(80, agility + amount);
    else if (stat == "ИНТ") intellect = std::min(80, intellect + amount);
    else if (stat == "ВЫН") endurance = std::min(80, endurance + amount);
}

bool Player::canFindPart() const {
    std::string key = "floor_" + std::to_string(currentFloor) + "_room_" + std::to_string(currentRoom);
    return foundParts.find(key) == foundParts.end();
}

bool Player::canFindArtifact() const {
    std::string key = "floor_" + std::to_string(currentFloor) + "_room_" + std::to_string(currentRoom);
    return foundArtifacts.find(key) == foundArtifacts.end();
}