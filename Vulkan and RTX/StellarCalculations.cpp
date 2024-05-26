#include "pch.h"
#include "StellarCalculations.h"

namespace StellarCalculations
{
	double secondsInDay = 1440;
	double axialTilt = 23.44 * glm::pi<double>() / 180.0;
	double daysInYear = 365.2524;

	double observerLatitude = 55.7;
	double observerLongitude = 37.6;

	double calculateSunDeclination(double dayOfYear) {
		return axialTilt * std::sin(glm::two_pi<double>() * (dayOfYear - 81) / daysInYear);
	}

	double calculateHourAngle(double timeOfDay, double longitude) {
		return (std::fmod(timeOfDay + (longitude / glm::two_pi<double>()), 1.0)
			- 0.5) * glm::two_pi<double>();
	}

	glm::vec3 calculateSunPosition(float timeSinceLaunch)
	{
		double timeOfDay = std::fmod(timeSinceLaunch, secondsInDay) / secondsInDay;
		double currentDay = timeSinceLaunch / secondsInDay;
		double dayOfYear = std::fmod(currentDay, daysInYear);

		double declination = calculateSunDeclination(dayOfYear);
		double hourAngle = calculateHourAngle(timeOfDay, observerLongitude);

		double x = std::cos(declination)      * std::sin(hourAngle);
		double y = std::sin(observerLatitude) * std::sin(declination) +
				   std::cos(observerLatitude) * std::cos(declination) * std::cos(hourAngle);
		double z = std::cos(observerLatitude) * std::sin(declination) -
				   std::sin(observerLatitude) * std::cos(declination) * std::cos(hourAngle);

		return glm::vec3(x, y, z);
	}
}
