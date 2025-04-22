#include "pch.h"
#include "StellarCalculations.h"

namespace StellarCalculations
{
	double calculateSunDeclination(double dayOfYear) {
		return axialTilt * std::sin(glm::two_pi<double>() * (dayOfYear - 81) / daysInYear);
	}

	double calculateHourAngle(double timeOfDay, double longitude) {
		return (std::fmod(timeOfDay + (longitude / glm::two_pi<double>()), 1.0)
			- 0.5) * glm::two_pi<double>();
	}

	glm::vec3 calculateSunPosition(double timeSinceLaunch)
	{
		timeSinceLaunch += secondsInDay / 4;
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

		//std::cout << "sun position: " << x << " " << y << " " << z << "\n";
		return glm::vec3(x, y, z);
	}

    glm::vec3 calculateSPASunPosition()
    {
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		std::tm utc_tm{};
		gmtime_s(&utc_tm, &now_c);

		//utc_tm.tm_hour += 3;
		//std::mktime(&utc_tm);

		spa_data spa{};
		spa.year = utc_tm.tm_year + 1900;
		spa.month = utc_tm.tm_mon + 1;
		spa.day = utc_tm.tm_mday;
		spa.hour = utc_tm.tm_hour;
		spa.minute = utc_tm.tm_min;
		spa.second = utc_tm.tm_sec;

		/*std::cout << "[SPA DEBUG] Current UTC Time:\n";
		std::cout << "  Year   : " << spa.year << "\n";
		std::cout << "  Month  : " << spa.month << "\n";
		std::cout << "  Day    : " << spa.day << "\n";
		std::cout << "  Hour   : " << spa.hour << "\n";
		std::cout << "  Minute : " << spa.minute << "\n";
		std::cout << "  Second : " << spa.second << "\n";*/

		// Observer location (adjust to your values)
		spa.latitude = 55.7;   // degrees
		spa.longitude = 37.6;   // degrees
		spa.elevation = 200.0;  // meters above sea level

		spa.function = SPA_ZA; // we want zenith and azimuth only

		if (spa_calculate(&spa) != 0) {
			std::cerr << "SPA calculation failed!\n";
			return glm::vec3(0.0f);
		}
		else {
			// Debug: SPA Output
			/*std::cout << "[SPA DEBUG] Solar Position:\n";
			std::cout << "  Zenith    : " << spa.zenith << "\n";
			std::cout << "  Azimuth   : " << spa.azimuth << "\n";*/
		}

		// Convert azimuth and zenith to direction vector
		double zenithRad = glm::radians(spa.zenith);
		double azimuthRad = glm::radians(spa.azimuth);

		// Convert spherical to Cartesian
		double x = std::sin(zenithRad) * std::sin(azimuthRad);
		double y = std::cos(zenithRad);
		double z = std::sin(zenithRad) * std::cos(azimuthRad);

		//glm::vec3 sunDir = glm::normalize(glm::vec3(x, y, z));
		glm::vec3 sunDir = glm::vec3(x, y, z);

		//std::cout << "sun position: " << sunDir.x << " " << sunDir.y << " " << sunDir.z << "\n";
		return sunDir;
    }
}
