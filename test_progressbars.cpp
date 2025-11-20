#include "entity.h"
#include <iostream>

using namespace std;

int main() {
    // Создаем героя
    Player* hero = new Player("Test Hero", 100, 10, 5, 5, 1, 1, 10, 0, 1, 500, 0, HeroClass::WARRIOR, ProgressionType::LEVEL_BASED);

    cout << "Тест прогрессбаров:\n";
    cout << "Здоровье:\n";
    hero->displayHealthBar();

    cout << "Опыт:\n";
    hero->displayExperienceBar();

    // Добавим опыт
    hero->setReceivedExperience(750);
    hero->upLevel();

    cout << "\nПосле получения опыта:\n";
    cout << "Здоровье:\n";
    hero->displayHealthBar();

    cout << "Опыт:\n";
    hero->displayExperienceBar();

    delete hero;
    return 0;
}