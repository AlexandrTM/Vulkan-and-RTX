#include "pch.h"

#ifndef ITEM_H
#define ITEM_H

class Item
{
private:
	std::string _name;
	std::string _id;
	int _quantity;
	int _maxStack;

public:
	void setName(std::string name);
	void setId(std::string id);
	void setQuantity(int quantity);
	void setMaxStack(int maxStack);
	std::string getName();
	int getQuantity();
	std::string getId();
	int getMaxStack();

};


#endif 