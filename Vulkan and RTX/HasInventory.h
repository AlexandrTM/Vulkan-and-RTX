#include "pch.h"
#include "Item.h"
#ifndef HAS_INVENTORY_H
#define HAS_INVENTORY_H

class HasInventory
{
private:
	int _inventorySlots;
	int _maxStackSize;
	std::vector<Item> _inventory;

public:
	HasInventory(); // standard inventorySlots and maxStackSize
	HasInventory(int inventorySlots, int maxStackSize);

	void putItemInSlot(int slotId, Item item);
	void putItemInFirstEmptySlot();
	void checkSlot(int slotId); // returns false if slot is empty or item name and num otherwise
	void checkInventoryFullness(); // returns empty and filled slots num

	//bool checkForEmptySlotAvailability();	
	// check if specific items and its amounts is in inventory
	//bool checkForItemsInInventory(std::string itemIds[], int itemAmounts[]); 
	void placeItemBackInInventory(Item item);

};

#endif