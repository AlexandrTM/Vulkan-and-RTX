#include "pch.h"
#ifndef MOVABLE_H
#define MOVABLE_H

class IsMovable
{
private:
	glm::dvec3 _position;
	float _movementSpeed;

public:
	IsMovable(glm::dvec3 position, float movementSpeed);

	void setPosition(glm::dvec3 position);
	glm::dvec3 getPosition();
	void move(glm::vec3 direction);

};

#endif