#include "pch.h"

#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
public:
	Camera();

	void setWidthHeight(uint32_t width, uint32_t height);

	void setLookFrom(glm::vec3 lookFrom);
	void setLookAt(glm::vec3 lookAt);
	void setDirection(glm::vec3 cameraDirection);
	void setVerticalFov(float vFov);
	glm::vec3 getLookFrom();
	glm::vec3 getLookAt();
	glm::vec3 getDirection();
	float getVerticalFov();

	void setLastX(double lastX);
	void setLastY(double lastY);
	double getLastX();
	double getLastY();

	void setYaw(double yaw);
	void setPitch(double pitch);
	void setRoll(double roll);
	double getYaw();
	double getPitch();
	double getRoll();

	glm::vec3 getVerticalWorldAxis();

private:
	uint32_t _width = 800;
	uint32_t _height = 450;

	glm::vec3 _lookFrom = glm::vec3(0.0f, 2.0f, 0.0f);
	glm::vec3 _lookAt = glm::vec3(1.0f, 2.0f, 0.0f);
	glm::vec3 _verticalWorldAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 _cameraDirection = glm::normalize(_lookAt - _lookFrom);

	float _verticalFov = 60.0f;

	double _lastX = _width / 2;
	double _lastY = _height / 2;
	double _yaw = 0.0;
	double _pitch = 0.0;
	double _roll = 0.0;
};

#endif