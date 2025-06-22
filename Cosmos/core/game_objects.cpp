#include <algorithm>

#include "../ui/console_ui.h"
#include "game_objects.h"

ActionResult::ActionResult(const std::string& desc, const std::string& itm)
  : description(desc), item(itm) {
}

Effect::Effect(const std::string& type, const std::string& name,
  int health_effect, int strength_effect, int agility_effect,
  int intellect_effect, int endurance_effect, int earth_resistance,
  int fire_resistance, int poison_resistance, int water_resistance)
  : type(type),
  name(name),
  health_effect(health_effect),
  strength_effect(strength_effect),
  agility_effect(agility_effect),
  intellect_effect(intellect_effect),
  endurance_effect(endurance_effect),
  earth_resistance(earth_resistance),
  fire_resistance(fire_resistance),
  poison_resistance(poison_resistance),
  water_resistance(water_resistance) {
}

Monster::Monster(const std::string& name, int health, Element element,
  Element weakness)
  : name(name), health(health), element(element), weakness(weakness) {
}

void Monster::TakeDamage(int damage) {
  health = std::max(0, health - damage);
}

Action::Action(const std::string& id, const std::string& description,
  const std::string& required_stat, const ActionResult& success,
  const ActionResult& failure)
  : id(id),
  description(description),
  required_stat(required_stat),
  success(success),
  failure(failure) {
  if (required_stat != "ÑÈË" && required_stat != "ËÎÂ" &&
    required_stat != "ÈÍÒ" && required_stat != "ÂÛÍ") {
    this->required_stat = "NONE";
  }
}

Room::Room(int id, const std::string& name, const std::string& description,
  const std::vector<std::string>& action_ids)
  : id(id),
  name(name),
  description(description),
  action_ids(action_ids) {
}

Player::Player() {}

Weapon::Weapon(const std::string& name, Element element,
  int min_neutral, int max_neutral,
  int min_weak, int max_weak,
  int crit_chance)
  : name(name),
  element(element),
  min_neutral_damage(min_neutral),
  max_neutral_damage(max_neutral),
  min_weak_damage(min_weak),
  max_weak_damage(max_weak),
  crit_chance(crit_chance) {
}

void Player::TakeDamage(int damage) {
  health = std::max(0, health - damage);
}

void Player::Heal(int amount) {
  health += amount;
}

void Player::AddItem(const std::string& item) {
  if (item == "Ôëàêîí Âíåçàïíîé Ñèëû") {
    sudden_strength_potions++;
  }
  else if (item == "Êğèñòàëë Æèçíè") {
    life_crystals++;
  }
}

void Player::UseLifeCrystal() {
  if (life_crystals > 0) {
    Heal(20);
    life_crystals--;
  }
}

void Player::UseSuddenStrengthPotion() {
  if (sudden_strength_potions > 0) {
    sudden_strength_potions--;
  }
}

void Player::IncreaseStat(const std::string& stat, int amount) {
  if (amount == 0) return;

  if (stat == "ÑÈË") {
    strength = std::min(80, strength + amount);
  }
  else if (stat == "ËÎÂ") {
    agility = std::min(80, agility + amount);
  }
  else if (stat == "ÈÍÒ") {
    intellect = std::min(80, intellect + amount);
  }
  else if (stat == "ÂÛÍ") {
    endurance = std::min(80, endurance + amount);
  }
}

bool Player::CanFindPart() const {
  std::string key = std::to_string(current_floor) + "_" +
    std::to_string(current_room);
  return found_parts.find(key) == found_parts.end();
}

bool Player::CanFindArtifact() const {
  std::string key = std::to_string(current_floor) + "_" +
    std::to_string(current_room);
  return found_artifacts.find(key) == found_artifacts.end();
}