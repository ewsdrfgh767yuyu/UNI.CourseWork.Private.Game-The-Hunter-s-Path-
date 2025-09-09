#pragma once

class Entity{
public:
	void setMaxHealthPoint(int max_hp) {
		m_max_healthpoint = max_hp;
	}
	void setCurrentHealthPoint(int c_hp) {
		m_current_healthpoint = c_hp;
	}
	void setDamage(int dmg) {
		m_damage = dmg;
	}
	void setDefence(int def) {
		m_defense = def;
	}
	void setAttack(int atk) {
		m_attack = atk;
	}
	void setStamina(int stam) {
		m_stamina = stam;
	}
	void setInitiative(int init) {
		m_initiative = init;
	}
	void setAttackRange(int range) {
		m_attack_range = range;
	}
private:
	int m_max_healthpoint;
	int m_current_healthpoint;
	int m_damage;
	int m_defense;
	int m_attack;
	int m_stamina;
	int m_initiative;
	int m_attack_range
};

Entity::Entity()
{
}

Entity::~Entity()
{
}