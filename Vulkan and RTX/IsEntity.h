#include "pch.h"
#ifndef IS_ENTITY_H
#define IS_ENTITY_H

class IsEntity
{
private:
	bool _invulnerable;

public:
	virtual void disappear() = 0;

};

#endif