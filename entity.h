#pragma once
#include <iostream>
#include <stdexcept> 
#include <cmath>
#include <string>
#include <map>
using namespace std;

enum class ItemType {
	WEAPON,
	ARMOR,
	ACCESSORY,
	CONSUMABLE
};

enum class EquipmentSlot {
	HEAD,
	CHEST,
	HANDS,
	LEGS,
	FEET,
	MAIN_HAND,
	OFF_HAND,
	NECK,
	RING1,
	RING2,
	NONE // Для неподходящих предметов
};

class Item {
private:
	string m_name;
	string m_description;
	ItemType m_type;
	EquipmentSlot m_slot;
	map<string, int> m_stats; // Характеристики предмета

public:
	Item(const string& name, const  string& description,
		ItemType type, EquipmentSlot slot,
		const map< string, int>& stats)
		: m_name(name), m_description(description),
		m_type(type), m_slot(slot), m_stats(stats) {}

	// Геттеры
	string getName() const { return m_name; }
	string getDescription() const { return m_description; }
	ItemType getType() const { return m_type; }
	EquipmentSlot getSlot() const { return m_slot; }
	int getStat(const string& statName) const {
		auto it = m_stats.find(statName);
		return it != m_stats.end() ? it->second : 0;
	}

	// Отображение информации о предмете
	void displayInfo() const {
		cout << "=== " << m_name << " ===\n";
		cout << "Type: ";
		switch (m_type) {
		case ItemType::WEAPON: cout << "Weapon"; break;
		case ItemType::ARMOR: cout << "Armor"; break;
		case ItemType::ACCESSORY: cout << "Accessory"; break;
		case ItemType::CONSUMABLE: cout << "Consumable"; break;
		}
		cout << "\nSlot: ";
		switch (m_slot) {
		case EquipmentSlot::HEAD: cout << "Head"; break;
		case EquipmentSlot::CHEST: cout << "Chest"; break;
		case EquipmentSlot::HANDS: cout << "Hands"; break;
		case EquipmentSlot::LEGS: cout << "Legs"; break;
		case EquipmentSlot::FEET: cout << "Feet"; break;
		case EquipmentSlot::MAIN_HAND: cout << "Main Hand"; break;
		case EquipmentSlot::OFF_HAND: cout << "Off Hand"; break;
		case EquipmentSlot::NECK: cout << "Neck"; break;
		case EquipmentSlot::RING1: cout << "Ring"; break;
		case EquipmentSlot::RING2: cout << "Ring"; break;
		case EquipmentSlot::NONE: cout << "None"; break;
		}
		cout << "\nDescription: " << m_description << "\n";
		cout << "Stats:\n";
		for (const auto& stat : m_stats) {
			cout << "  " << stat.first << ": " << stat.second << "\n";
		}
		cout << "==================\n";
	}
};

class Entity {
public:
	Entity(int max_hp = 100, int damage = 10, int defense = 0,
		int attack = 0, int max_stamina = 1, int c_stamina = 1, int initiative = 10,
		int attack_range = 0)
		: m_max_healthpoint(max_hp), m_current_healthpoint(max_hp),
		m_damage(damage), m_defense(defense), m_attack(attack),
		m_max_stamina(max_stamina), m_current_stamina(c_stamina),m_initiative(initiative),
		m_attack_range(attack_range){}
	void setMaxHealthPoint(int max_hp) {
		if (max_hp > 0) {
			m_max_healthpoint = max_hp;
		}
		else {
			throw  invalid_argument("Health cannot be negative!");
		}
	}
	void setCurrentHealthPoint(int c_hp) {
		if (c_hp >= 0 && c_hp <= m_max_healthpoint) {
			m_current_healthpoint = c_hp;
		}
		else {
			throw  invalid_argument("Current HP set error");
		}
	}
	void setDamage(int dmg) {
		if (dmg > 0) {
			m_damage = dmg;
		}
		else
		{
			throw  invalid_argument("Damage cannot be negative!");
		}
	}
	void setDefence(int def) {
		if (def >= 0) {
			m_defense = def;
		}
		else {
			throw  invalid_argument("Defence cannot be negative!");
		}
	}
	void setAttack(int atk) {
		if (atk >= 0) {
			m_attack = atk;
		}
		else {
			throw  invalid_argument("Attack cannot be negative!");
		}
	}
	void setMaxStamina(int max_stam) {
		if (max_stam > 0) {
			m_max_stamina = max_stam;
		}
		else {
			throw  invalid_argument("Stamina cannot be negative!");
		}
	}
	void setCurrentStamina(int c_stam) {
		if (c_stam >= 0 && c_stam <= m_max_stamina) {
			m_current_stamina = c_stam;
		}
		else {
			throw  invalid_argument("Current stamina set error");
		}
	}
	void setInitiative(int init) {
		if (init > 0) {
			m_initiative = init;
		}
		else {
			throw  invalid_argument("Initiative cannot be negative!");
		}
	}
	void setAttackRange(int range) {
		if (range >= 0 && range <= 3) {
			m_attack_range = range;
		}
		else {
			throw  invalid_argument("Attack range set error");
		}
	}

	int attack(int recipient_protection) {
		return (m_damage + m_attack - recipient_protection);
		spendStamina;
	}
	void heal(int heal_amount) {
		if (m_current_healthpoint + heal_amount >= m_max_healthpoint) {
			setCurrentHealthPoint(m_max_healthpoint);
			spendStamina();
		}
		else {
			setCurrentHealthPoint(m_current_healthpoint + heal_amount);
		}
	}
	void takeDamage(int recieved_damage) {
		m_current_healthpoint -= recieved_damage;
	}
	void regenerateStamina() {
		m_current_stamina = m_max_stamina;
	}

	int getMaxHealthPoint() const { return m_max_healthpoint; };
	int getCurrentHealthPoint() const { return m_current_healthpoint; };
	int getDamage() const { return m_damage; };
	int getDefense() const { return m_defense; };
	int getAttack() const { return m_attack; };
	int getMaxStamina() const { return m_max_stamina; };
	int getCurrentStamina() const { return m_current_stamina; };
	int getInitiative() const { return m_initiative; };
	int getAttackRange() const { return m_attack_range; };
private:
	int m_max_healthpoint;
	int m_current_healthpoint;
	int m_damage;
	int m_defense;
	int m_attack;
	int m_max_stamina;
	int m_current_stamina;
	int m_initiative;
	int m_attack_range;

	void spendStamina() {
		if (m_current_stamina > 0)
		{
			m_current_stamina--;
		}
		else {
			throw  invalid_argument("Negative stamina");
		}
	}
};

class Player : public Entity {
private:
	int m_level;
	int m_required_experience;
	int m_received_experience;
	void increaseRequiredExperience() {
		int required_experience = 250 * pow((m_level + 1), 1.8);
		setRequiredExperience(required_experience);
	}
public:
	Player(int max_hp = 100, int damage = 10, int defense = 0,
		int attack = 0, int max_stamina = 1, int c_stamina = 1, int initiative = 10,
		int attack_range = 0, int level = 1, int required_experience = 1000, int received_experience = 0)
		: Entity(max_hp, damage, defense, attack,
			max_stamina, c_stamina, initiative, attack_range),
		m_level(level), m_required_experience(required_experience), m_received_experience(received_experience) {}

	void setLevel(int level) {
		if (level > 0) {
			m_level = level;
		}
		else {
			throw  invalid_argument("Level cannot be negative!");
		}
	}
	void setRequiredExperience(int req_level) {
		if (req_level > 0) {
			m_required_experience = req_level;
		}
		else {
			throw  invalid_argument("Experience cannot be negative!");
		}
	}
	void setReceivedExperience(int rec_level) {
		if (rec_level > 0) {
			m_received_experience = rec_level;
		}
		else {
			throw  invalid_argument("Experience cannot be negative!");
		}
	}

	int getLevel() const { return m_level; };
	int getRequiredExperience() const { return m_required_experience; };
	int getReceivedExperience() const { return m_received_experience; };

	void upLevel() {
		while (m_received_experience >= m_required_experience) {
			setReceivedExperience(m_received_experience - m_required_experience);
			setLevel(m_level + 1);
			increaseRequiredExperience();

			setMaxHealthPoint(static_cast<int>(getMaxHealthPoint() * 1.1));
			setDamage(static_cast<int>(getDamage() + 2));
			setDefence(static_cast<int>(getDefense() + 1));
			setAttack(static_cast<int>(getAttack() + 1));
			setInitiative(static_cast<int>(getInitiative() ));

			setCurrentHealthPoint(getMaxHealthPoint());
			setCurrentStamina(getMaxStamina());
		}
	}
};

class Enemy : public Entity {
private:
	int m_experience_value;
	int m_difficulty_level;
	string m_enemy_type; 

public:
	Enemy(int max_hp = 50, int damage = 8, int defense = 2,
		int attack = 2, int max_stamina = 1, int c_stamina = 1,
		int initiative = 8, int attack_range = 0,
		int exp_value = 50, int difficulty = 1,
		const string& type = "common_enemy")
		: Entity(max_hp, damage, defense, attack, max_stamina,
			c_stamina, initiative, attack_range),
		m_experience_value(exp_value),
		m_difficulty_level(difficulty),
		m_enemy_type(type) {}
};