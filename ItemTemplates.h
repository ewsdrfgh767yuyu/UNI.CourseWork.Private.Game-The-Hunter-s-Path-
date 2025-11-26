#pragma once
#include "entity.h"
#include <vector>
#include <map>
#include <string>

// Структура шаблона предмета
struct ItemTemplate
{
    std::string name = "";
    std::string description = "";
    ItemType type = ItemType::CONSUMABLE;
    EquipmentSlot slot = EquipmentSlot::NONE;
    std::map<std::string, int> stats = {};
};

// Фабрика для создания предметов
class ItemFactory
{
private:
    static std::vector<ItemTemplate> itemTemplates;

    static void initializeTemplates();

public:
    // Получить список всех предметов
    static const std::vector<ItemTemplate> &getAllItems();

    // Получить предметы по типу
    static std::vector<ItemTemplate> getItemsByType(ItemType type);

    // Создать предмет по индексу
    static Item createItem(int index);

    // Создать случайный предмет
    static Item createRandomItem();

    // Создать случайный предмет определенного типа
    static Item createRandomItemOfType(ItemType type);

    // Генерировать строку способностей
    static std::string generateAbilityString(const std::map<std::string, int>& stats);
};