#include "pch.h"
#include "IsEntity.h"
#include "HasInventory.h"
#include "IsMovable.h"

#ifndef IS_CREATURE_H
#define IS_CREATURE_H

class IsCreature : public IsEntity, public IsMovable, public HasInventory
{
private:
	//IsEntty* _entity;
	IsMovable* _movable;
	HasInventory* _inventory;

public:
	IsCreature(IsEntity* entity, IsMovable* movable, HasInventory* inventory);
};

#endif