#include "pch.h"
#include "Item.h"

void Item::setName(std::string name) { _name = name; }
void Item::setQuantity(int quantity) { _quantity = quantity; }
std::string Item::getName() { return _name; }
int Item::getQuantity() { return _quantity; }
std::string Item::getId() { return _id; }
int Item::getMaxStack() { return _maxStack; }