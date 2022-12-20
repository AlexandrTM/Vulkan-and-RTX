#include "pch.h"
#include "HasExperienceLevel.h"

HasExperienceLevel::HasExperienceLevel() {}

void HasExperienceLevel::setLevel(int level) { _level = level; }
void HasExperienceLevel::setExperience(double experience) { _experience = experience; }
double HasExperienceLevel::getExperience() { return _experience; }
int HasExperienceLevel::getLevel() { return _level; }

void HasExperienceLevel::increaseExperience(double experience) { _experience += experience; }
void HasExperienceLevel::decreaseExperience(double experience) 
{ 
	_experience -= experience; 
	if (_experience <= 0) { _experience = 0; }
}
