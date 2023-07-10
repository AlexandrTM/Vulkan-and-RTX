#include "pch.h"

#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
public:
	Camera();
	Camera(
		glm::vec3 lookFrom,
		glm::vec3 lookAt,
		glm::vec3 verticalWorldAxis,
		float verticalFov,
		uint32_t viewportWidth,
		uint32_t viewportHeight,
		double yaw,
		double pitch,
		double roll
	);

	void setViewportSize(uint32_t viewportWidth, uint32_t viewportHeight);

	void setLookFrom(glm::vec3 lookFrom);
	void setLookAt(glm::vec3 lookAt);
	void setDirection(glm::vec3 cameraDirection);
	void setVerticalFov(float vFov);
	glm::vec3 getLookFrom();
	glm::vec3 getLookAt();
	glm::vec3 getDirection();
	float getVerticalFov();

	void setLastViewportX(double lastViewportX);
	void setLastViewportY(double lastViewportY);
	double getLastViewportX();
	double getLastViewportY();

	void setYaw(double yaw);
	void setPitch(double pitch);
	void setRoll(double roll);
	double getYaw();
	double getPitch();
	double getRoll();

	glm::vec3 getVerticalWorldAxis();

private:

	glm::vec3 _lookFrom;
	glm::vec3 _lookAt;
	glm::vec3 _cameraDirection;
	glm::vec3 _verticalWorldAxis;

	float _verticalFov;

	uint32_t _viewportWidth;
	uint32_t _viewportHeight;
	double _lastViewportX;
	double _lastViewportY;

	double _yaw;
	double _pitch;
	double _roll;
};

#endif