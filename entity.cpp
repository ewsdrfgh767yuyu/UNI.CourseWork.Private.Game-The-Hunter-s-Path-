#include "entity.h"

map<EquipmentSlot, string> Player::slotNames = {
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
