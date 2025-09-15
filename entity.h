#pragma once
#include <iostream>
#include <stdexcept> 
using namespace std;

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
			throw std::invalid_argument("Health cannot be negative!");
		}
	}
	void setCurrentHealthPoint(int c_hp) {
		if (c_hp >= 0 && c_hp <= m_max_healthpoint) {
			m_current_healthpoint = c_hp;
		}
		else {
			throw std::invalid_argument("Current HP set error");
		}
	}
	void setDamage(int dmg) {
		if (dmg > 0) {
			m_damage = dmg;
		}
		else
		{
			throw std::invalid_argument("Damage cannot be negative!");
		}
	}
	void setDefence(int def) {
		if (def >= 0) {
			m_defense = def;
		}
		else {
			throw std::invalid_argument("Defence cannot be negative!");
		}
	}
	void setAttack(int atk) {
		if (atk >= 0) {
			m_attack = atk;
		}
		else {
			throw std::invalid_argument("Attack cannot be negative!");
		}
	}
	void setMaxStamina(int max_stam) {
		if (max_stam > 0) {
			m_max_stamina = max_stam;
		}
		else {
			throw std::invalid_argument("Stamina cannot be negative!");
		}
	}
	void setCurrentStamina(int c_stam) {
		if (c_stam >= 0 && c_stam <= m_max_stamina) {
			m_current_stamina = c_stam;
		}
		else {
			throw std::invalid_argument("Current stamina set error");
		}
	}
	void setInitiative(int init) {
		if (init > 0) {
			m_initiative = init;
		}
		else {
			throw std::invalid_argument("Initiative cannot be negative!");
		}
	}
	void setAttackRange(int range) {
		if (range >= 0 && range <= 3) {
			m_attack_range = range;
		}
		else {
			throw std::invalid_argument("Attack range set error");
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
			throw std::invalid_argument("Negative stamina");
		}
	}
};

class Player : public Entity {
private:
	int m_level;
	int m_required_experience;
	int m_received_experience;
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
			throw std::invalid_argument("Level cannot be negative!");
		}
	}
};