#include "pch.h"

#ifndef STELLAR_CALCULATIONS
#define STELLAR_CALCULATIONS

#ifdef __cplusplus
extern "C" {
#endif

#include "spa.h"

#ifdef __cplusplus
}
#endif

namespace StellarCalculations
{
	constexpr double secondsInDay = 1440;
	constexpr double axialTilt = 23.44 * glm::pi<double>() / 180.0;
	constexpr double daysInYear = 365.2524;

	constexpr double observerLatitude = 55.7;
	constexpr double observerLongitude = 37.6;

	double calculateSunDeclination(double dayOfYear);
	double calculateHourAngle(double timeOfDay, double longitude);
	glm::vec3 calculateSunPosition(double timeSinceLaunch);
	glm::vec3 calculateSPASunPosition();
}

#endif // STELLAR_CALCULATIONS