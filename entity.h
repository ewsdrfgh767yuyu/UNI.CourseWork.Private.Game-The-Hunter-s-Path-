#pragma once
#include <iostream>
#include <stdexcept> 
using namespace std;

class Entity {
public:
	void setMaxHealthPoint(int max_hp) {
		if (max_hp > 0) {
			m_max_healthpoint = max_hp;
		}
		else {
			throw std::invalid_argument("Health cannot be negative!");
		}
	}
	void setCurrentHealthPoint(int c_hp) {
		if (m_current_healthpoint >= 0 && c_hp <= m_max_healthpoint) {
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
	void setStamina(int stam) {
		if (stam > 0) {
			m_stamina = stam;
		}
		else {
			throw std::invalid_argument("Stamina cannot be negative!");
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
	
private:
	int m_max_healthpoint;
	int m_current_healthpoint;
	int m_damage;
	int m_defense;
	int m_attack;
	int m_stamina;
	int m_initiative;
	int m_attack_range;
};