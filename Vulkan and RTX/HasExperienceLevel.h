#include "pch.h"
#ifndef HAS_EXPERIENCE_LEVEL_H
#define HAS_EXPERIENCE_LEVEL_H

class HasExperienceLevel
{
private:
	int _level;
	double _experience;

public:
	HasExperienceLevel();

	void setLevel(int level);
	void setExperience(double experience);
	int getLevel();
	double getExperience();

	void increaseExperience(double experience);
	void decreaseExperience(double experience);
	

};

#endif 