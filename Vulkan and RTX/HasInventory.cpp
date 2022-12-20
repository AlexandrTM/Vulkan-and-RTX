#include "pch.h"
#include "HasInventory.h"

HasInventory::HasInventory() 
{ 
	_inventorySlots = 1, _maxStackSize = 1; 
	_inventory.resize(_inventorySlots);
}
HasInventory::HasInventory(int inventorySlots, int maxStackSize) 
{ 
	_inventorySlots = inventorySlots, _maxStackSize = maxStackSize; 
	_inventory.resize(_inventorySlots);
}

//void HasInventory::putItemInSlot(int slotId, Item item) 
//{
//	if (_inventory[slotId].getId() == item.getId() and _inventory[slotId].getName() == item.getName())
//	{
//		// work out moving item with cursor
//		_inventory[slotId].setQuntity(_inventory[slotId].getQuantity() + item.getQuantity());
//	}
//	
//}
void HasInventory::putItemInFirstEmptySlot() {}
void HasInventory::checkSlot(int slotId) {}
void HasInventory::checkInventoryFullness() {}

//bool HasInventory::checkForEmptySlotAvailability() {}
//bool HasInventory::checkForItemsInInventory(std::string itemIds[], int itemAmounts[]) {}
void HasInventory::placeItemBackInInventory(Item item) {}