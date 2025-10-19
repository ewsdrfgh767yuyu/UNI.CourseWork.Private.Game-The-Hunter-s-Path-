# Техническая документация игры "The Hunter's Path"

## Обзор проекта

Игра в жанре Darkest Dungeon с пошаговыми боями, нелинейной навигацией по локациям и элементами rogue-like.

## Архитектурные решения

### 1. Модульная структура
Проект разделен на независимые системы для обеспечения:
- **Тестируемости** - каждая система может тестироваться отдельно
- **Расширяемости** - легко добавлять новые механики
- **Поддержки** - четкое разделение ответственности

### 2. Наследование сущностей
Используется иерархия классов:
- **Entity** - базовая функциональность всех существ
- **Player** - специализация для игровых персонажей
- **Enemy** - специализация для врагов

### 3. Компонентный подход
Каждая система отвечает за свою область:
- **BattleSystem** - логика боя
- **LocationSystem** - навигация по миру
- **EventSystem** - обработка событий
- **PartySystem** - управление отрядом

## Детали реализации

### Система боя (BattleSystem)

#### Механика инициативы
```cpp
// Расчет веса хода на основе инициативы
int turnWeight = entity->getInitiative() / 10;
// Юнит с инициативой 20: turnWeight = 2 (ходит в 2 раза чаще)
```

#### Позиционная система
```cpp
class BattlePosition {
    vector<Entity*> frontLine;    // Первая линия (позиции 0-1)
    vector<Entity*> backLine;     // Вторая линия (позиции 2-3)
    vector<Entity*> corpses;      // Трупы на позициях
};

// Проверка доступности цели
bool canAttackTarget(Entity* attacker, Entity* target) {
    int attackerRange = attacker->getAttackRange();
    int targetPosition = getPosition(target);
    
    // Проверка наличия трупов на пути
    return isTargetReachable(attackerRange, targetPosition);
}
```

### Система локаций

#### Граф навигации
```cpp
struct LocationConnection {
    Location* from;
    Location* to;
    int travelCost;  // Стоимость перемещения (выносливость/здоровье)
};

// Нелинейные связи между локациями
vector<LocationConnection> connections = {
    {FOREST, CAVE, 10},
    {FOREST, DEAD_CITY, 15},
    {CAVE, CASTLE, 20},
    {CAVE, DEAD_CITY, 12},
    {DEAD_CITY, CASTLE, 18},
    {DEAD_CITY, FOREST, 15},
    {CASTLE, CAVE, 20},
    {CASTLE, FOREST, 25}
};
```

### Система событий

#### Детальная механика генерации событий

**Вероятностная система:**
```cpp
struct EventProbability {
    EventType type;
    float baseProbability;    // Базовая вероятность
    float levelMultiplier;    // Множитель от уровня отряда
    int minLevel;            // Минимальный уровень для события
    int maxLevel;            // Максимальный уровень для события
};

class EventGenerator {
private:
    map<LocationType, vector<EventProbability>> locationProbabilities;
    RandomGenerator random;
    
    // Корректировка вероятностей на основе уровня отряда
    vector<float> adjustProbabilities(LocationType location, int partyLevel) {
        vector<float> adjusted;
        for (auto& prob : locationProbabilities[location]) {
            if (partyLevel >= prob.minLevel && partyLevel <= prob.maxLevel) {
                float adjustedProb = prob.baseProbability *
                                   (1.0 + prob.levelMultiplier * (partyLevel - 1));
                adjusted.push_back(adjustedProb);
            } else {
                adjusted.push_back(0.0); // Событие недоступно для этого уровня
            }
        }
        return adjusted;
    }
    
public:
    Event* generateEvent(Location* location, int partyLevel) {
        auto probabilities = adjustProbabilities(location->getType(), partyLevel);
        EventType type = weightedRandomSelect(probabilities);
        return createEvent(type, location, partyLevel);
    }
    
    // Взвешенный случайный выбор
    EventType weightedRandomSelect(const vector<float>& probabilities) {
        float total = accumulate(probabilities.begin(), probabilities.end(), 0.0f);
        float randomValue = random.nextFloat(0.0f, total);
        
        float cumulative = 0.0f;
        for (size_t i = 0; i < probabilities.size(); i++) {
            cumulative += probabilities[i];
            if (randomValue <= cumulative) {
                return static_cast<EventType>(i);
            }
        }
        return COMBAT; // Fallback
    }
};
```

**Конфигурация вероятностей по локациям:**
```cpp
// Инициализация вероятностей событий
void initializeProbabilities() {
    locationProbabilities[FOREST] = {
        {COMBAT, 0.6, 0.1, 1, 10},     // 60% база, +10% за уровень
        {TREASURE, 0.2, 0.05, 1, 10},  // 20% база, +5% за уровень
        {RECRUIT, 0.1, 0.02, 2, 8},    // Только с 2 уровня
        {TRAP, 0.05, 0.01, 1, 10},
        {REST, 0.05, 0.0, 1, 10}       // Вероятность не растет с уровнем
    };
    
    locationProbabilities[CAVE] = {
        {COMBAT, 0.7, 0.15, 1, 10},    // Пещеры более опасны
        {TREASURE, 0.15, 0.08, 1, 10}, // Но награды лучше
        {TRAP, 0.1, 0.05, 1, 10},
        {REST, 0.05, 0.0, 1, 10}
    };
    
    locationProbabilities[CASTLE] = {
        {BOSS, 1.0, 0.0, 3, 10}        // Только босс с 3 уровня
    };
}
```

**Динамическая генерация контента событий:**
```cpp
Event* createEvent(EventType type, Location* location, int partyLevel) {
    Event* event = new Event();
    event->type = type;
    event->difficultyLevel = calculateDifficulty(partyLevel, location);
    
    switch (type) {
    case COMBAT:
        event->involvedEntities = generateEnemies(event->difficultyLevel, location);
        event->rewards = generateCombatRewards(event->difficultyLevel);
        break;
        
    case TREASURE:
        event->rewards = generateTreasure(event->difficultyLevel);
        event->choices = {"Взять сокровище", "Оставить", "Осмотреть окружение"};
        break;
        
    case RECRUIT:
        event->involvedEntities = {generateRecruitableHero(partyLevel)};
        event->choices = {"Принять в отряд", "Отказать", "Обменяться предметами"};
        break;
    }
    
    return event;
}
```

**Система последствий выборов:**
- Каждый выбор в событии имеет последствия
- Влияние на отношения между персонажами
- Изменение вероятностей будущих событий
- Временные эффекты (баффы/дебаффы)

## Интерфейсные решения

### Консольный интерфейс (текущий этап)
- Текстовое представление состояния игры
- Командный ввод для управления
- Пошаговое отображение боя

### Планируемый графический интерфейс
- SDL2 или аналогичная библиотека
- Изометрическое или 2D представление
- Анимации боя и перемещений

## Алгоритмы и структуры данных

### 1. Очередь ходов в бою
```cpp
vector<pair<Entity*, int>> calculateTurnOrder() {
    vector<pair<Entity*, int>> order;
    
    for (Entity* entity : allEntities) {
        int weight = entity->getInitiative() / 10;
        for (int i = 0; i < weight; i++) {
            order.push_back({entity, calculatePriority(entity)});
        }
    }
    
    sort(order.begin(), order.end(), 
         [](auto a, auto b) { return a.second > b.second; });
    return order;
}
```

### 2. Поиск пути между локациями
```cpp
vector<Location*> findPath(Location* start, Location* end) {
    // Алгоритм A* или BFS для нахождения кратчайшего пути
    // Учитывает стоимость перемещения и доступность локаций
}
```

### 3. Распределение опыта
```cpp
void Party::distributeExperience(int totalExp) {
    int expPerMember = totalExp / members.size();
    for (Player* member : members) {
        if (!member->isDead()) {
            member->addExperience(expPerMember);
        }
    }
}
```

## Ограничения и требования

### Технические ограничения
- Максимальный размер отряда: 4 персонажа
- Максимальное количество врагов в бою: 6
- Количество локаций: 4 основных + связи
- Глубина инвентаря: ограничена только памятью

### Требования к производительности
- Быстрая генерация случайных событий
- Эффективный расчет очереди ходов
- Минимальные задержки при отображении боя

## Рекомендации по разработке

### 1. Приоритеты реализации
1. **Базовые системы** - BattleSystem, PartySystem
2. **Навигация** - LocationSystem, EventSystem  
3. **Контент** - враги, предметы, события
4. **Интерфейс** - консольный → графический

### 2. Тестирование
- Модульные тесты для каждой системы
- Интеграционные тесты для взаимодействия систем
- Тесты баланса боя и генерации событий

### 3. Балансировка
- Постепенная настройка характеристик врагов
- Баланс системы инициативы
- Равномерное распределение событий по локациям

## Будущие расширения

### Возможные улучшения
1. **Система навыков** - уникальные способности персонажей
2. **Эффекты статусов** - отравление, ослабление и т.д.
3. **Погодные условия** - влияние на бой и перемещения
4. **Диалоговая система** - взаимодействие с NPC
5. **Система достижений** - отслеживание прогресса

### Мета-прогресс (опционально)
- Перманентные улучшения между забегами
- Разблокировка новых персонажей
- Улучшение базовых характеристик

Эта архитектура обеспечивает прочную основу для разработки полноценной игры в стиле Darkest Dungeon с возможностью постепенного расширения функциональности.