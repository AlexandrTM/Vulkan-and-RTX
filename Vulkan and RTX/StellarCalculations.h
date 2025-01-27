#include "pch.h"

#ifndef STELLAR_CALCULATIONS
#define STELLAR_CALCULATIONS

namespace StellarCalculations
{
	extern double secondsInDay;
	extern double axialTilt;
	extern double daysInYear;

	extern double observerLatitude;
	extern double observerLongitude;

	double calculateSunDeclination(double dayOfYear);
	double calculateHourAngle(double timeOfDay, double longitude);
	glm::vec3 calculateSunPosition(double timeSinceLaunch);
}

#endif // STELLAR_CALCULATIONS