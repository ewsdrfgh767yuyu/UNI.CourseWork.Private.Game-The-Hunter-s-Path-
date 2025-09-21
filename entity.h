#pragma once
#include <iostream>
#include <stdexcept> 
#include <cmath>
#include <string>
#include <map>
#include <vector>
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
	NONE 
};

class Item {
private:
	string m_name;
	string m_description;
	ItemType m_type;
	EquipmentSlot m_slot;
	map<string, int> m_stats;

public:
	Item(const string& name, const  string& description,
		ItemType type, EquipmentSlot slot,
		const map< string, int>& stats)
		: m_name(name), m_description(description),
		m_type(type), m_slot(slot), m_stats(stats) {}

	string getName() const { return m_name; }
	string getDescription() const { return m_description; }
	ItemType getType() const { return m_type; }
	EquipmentSlot getSlot() const { return m_slot; }
	int getStat(const string& statName) const {
		auto it = m_stats.find(statName);
		return it != m_stats.end() ? it->second : 0;
	}

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

	vector<Item> m_inventory;
	map<EquipmentSlot, Item> m_equipment;

	void increaseRequiredExperience() {
		int required_experience = 250 * pow((m_level + 1), 1.8);
		setRequiredExperience(required_experience);
	}

	void recalculateStats() {
		int healthBonus = 0;
		int damageBonus = 0;
		int defenseBonus = 0;
		int attackBonus = 0;
		int staminaBonus = 0;
		int initiativeBonus = 0;

		for (const auto& item : m_equipment) {
			healthBonus += item.second.getStat("health");
			damageBonus += item.second.getStat("damage");
			defenseBonus += item.second.getStat("defense");
			attackBonus += item.second.getStat("attack");
			staminaBonus += item.second.getStat("stamina");
			initiativeBonus += item.second.getStat("initiative");
		}

		Entity::setMaxHealthPoint(100 + 10 * (m_level - 1) + healthBonus);
		Entity::setDamage(10 + 2 * (m_level - 1) + damageBonus);
		Entity::setDefence(0 + 1 * (m_level - 1) + defenseBonus);
		Entity::setAttack(0 + 1 * (m_level - 1) + attackBonus);
		Entity::setMaxStamina(1 + (m_level / 5) + staminaBonus);
		Entity::setInitiative(10 + (m_level / 3) + initiativeBonus);

		regenerateStamina();
	}

	static const map<EquipmentSlot, string> slotNames;

public:

	Player(int max_hp = 100, int damage = 10, int defense = 0,
		int attack = 0, int max_stamina = 1, int c_stamina = 1, int initiative = 10,
		int attack_range = 0, int level = 1, int required_experience = 1000,
		int received_experience = 0)
		: Entity(max_hp, damage, defense, attack,
			max_stamina, c_stamina, initiative, attack_range),
		m_level(level), m_required_experience(required_experience),
		m_received_experience(received_experience) {

		m_equipment = {
			{EquipmentSlot::HEAD, Item("None", "No item", ItemType::ARMOR, EquipmentSlot::HEAD, {})},
			{EquipmentSlot::CHEST, Item("None", "No item", ItemType::ARMOR, EquipmentSlot::CHEST, {})},
			{EquipmentSlot::HANDS, Item("None", "No item", ItemType::ARMOR, EquipmentSlot::HANDS, {})},
			{EquipmentSlot::LEGS, Item("None", "No item", ItemType::ARMOR, EquipmentSlot::LEGS, {})},
			{EquipmentSlot::FEET, Item("None", "No item", ItemType::ARMOR, EquipmentSlot::FEET, {})},
			{EquipmentSlot::MAIN_HAND, Item("None", "No item", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, {})},
			{EquipmentSlot::OFF_HAND, Item("None", "No item", ItemType::WEAPON, EquipmentSlot::OFF_HAND, {})},
			{EquipmentSlot::NECK, Item("None", "No item", ItemType::ACCESSORY, EquipmentSlot::NECK, {})},
			{EquipmentSlot::RING1, Item("None", "No item", ItemType::ACCESSORY, EquipmentSlot::RING1, {})},
			{EquipmentSlot::RING2, Item("None", "No item", ItemType::ACCESSORY, EquipmentSlot::RING2, {})}
		};
	}

	void setLevel(int level) {
		if (level > 0) {
			m_level = level;
		}
		else {
			throw invalid_argument("Level cannot be negative!");
		}
	}

	void setRequiredExperience(int req_level) {
		if (req_level > 0) {
			m_required_experience = req_level;
		}
		else {
			throw invalid_argument("Experience cannot be negative!");
		}
	}

	void setReceivedExperience(int rec_level) {
		if (rec_level > 0) {
			m_received_experience = rec_level;
		}
		else {
			throw invalid_argument("Experience cannot be negative!");
		}
	}

	int getLevel() const { return m_level; }
	int getRequiredExperience() const { return m_required_experience; }
	int getReceivedExperience() const { return m_received_experience; }

	void addItem(const Item& item) {
		m_inventory.push_back(item);
	}

	void equipItem(int inventoryIndex) {
		if (inventoryIndex < 0 || inventoryIndex >= m_inventory.size()) {
			throw out_of_range("Invalid inventory index");
		}

		Item item = m_inventory[inventoryIndex];
		EquipmentSlot slot = item.getSlot();

		if (slot == EquipmentSlot::NONE) {
			cout << "This item cannot be equipped.\n";
			return;
		}

		if (m_equipment[slot].getName() != "None") {
			unequipItem(slot);
		}

		m_equipment[slot] = item;
		m_inventory.erase(m_inventory.begin() + inventoryIndex);

		recalculateStats();

		cout << "Equipped: " << item.getName() << "\n";
	}

	void unequipItem(EquipmentSlot slot) {
		if (m_equipment[slot].getName() == "None") {
			cout << "No item equipped in this slot.\n";
			return;
		}

		m_inventory.push_back(m_equipment[slot]);

		m_equipment[slot] = Item("None", "No item",
			slot == EquipmentSlot::MAIN_HAND || slot == EquipmentSlot::OFF_HAND ?
			ItemType::WEAPON : ItemType::ARMOR,
			slot, {});

		recalculateStats();

		cout << "Unequipped item from slot.\n";
	}

	void displayInventory() const {
		cout << "=== INVENTORY ===\n";
		if (m_inventory.empty()) {
			cout << "Inventory is empty.\n";
		}
		else {
			for (size_t i = 0; i < m_inventory.size(); ++i) {
				cout << i << ": " << m_inventory[i].getName() << "\n";
			}
		}
		cout << "=================\n";
	}

	void displayEquipment() const {
		cout << "=== EQUIPMENT ===\n";
		for (const auto& slot : m_equipment) {
			cout << slotNames.at(slot.first) << ": " << slot.second.getName() << "\n";
		}
		cout << "=================\n";
	}

	void useConsumable(int inventoryIndex) {
		if (inventoryIndex < 0 || inventoryIndex >= m_inventory.size()) {
			throw out_of_range("Invalid inventory index");
		}

		Item item = m_inventory[inventoryIndex];
		if (item.getType() != ItemType::CONSUMABLE) {
			cout << "This item is not consumable.\n";
			return;
		}

		int healthRestore = item.getStat("health_restore");
		if (healthRestore > 0) {
			heal(healthRestore);
		}

		int staminaRestore = item.getStat("stamina_restore");
		if (staminaRestore > 0) {
			setCurrentStamina(min(getMaxStamina(), getCurrentStamina() + staminaRestore));
		}

		m_inventory.erase(m_inventory.begin() + inventoryIndex);

		cout << "Used: " << item.getName() << "\n";
	}

	void upLevel() {
		while (m_received_experience >= m_required_experience) {
			setReceivedExperience(m_received_experience - m_required_experience);
			setLevel(m_level + 1);
			increaseRequiredExperience();

			recalculateStats();
		}
	}
};

const map<EquipmentSlot, string> Player::slotNames = {
	{EquipmentSlot::HEAD, "Head"},
	{EquipmentSlot::CHEST, "Chest"},
	{EquipmentSlot::HANDS, "Hands"},
	{EquipmentSlot::LEGS, "Legs"},
	{EquipmentSlot::FEET, "Feet"},
	{EquipmentSlot::MAIN_HAND, "Main Hand"},
	{EquipmentSlot::OFF_HAND, "Off Hand"},
	{EquipmentSlot::NECK, "Neck"},
	{EquipmentSlot::RING1, "Ring 1"},
	{EquipmentSlot::RING2, "Ring 2"}
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

	void setExperienceValue(int exp_value) {
		if (exp_value >= 0) {
			m_experience_value = exp_value;
		}
		else {
			throw std::invalid_argument("Experience value cannot be negative!");
		}
	}

	int getExperienceValue() const { return m_experience_value; }

	void setDifficultyLevel(int difficulty) {
		if (difficulty > 0) {
			m_difficulty_level = difficulty;
		}
		else {
			throw std::invalid_argument("Difficulty level must be positive!");
		}
	}

	int getDifficultyLevel() const { return m_difficulty_level; }

	void setEnemyType(const std::string& type) {
		m_enemy_type = type;
	}

	const std::string& getEnemyType() const { return m_enemy_type; }
};