#include "pch.h"
#include "Camera.h"

Camera::Camera() 
{
	_lookFrom = glm::vec3(0.0f, 2.0f, 0.0f);
	_lookAt = _lookFrom + glm::vec3(1.0f, 0.0f, 0.0f);
	_cameraDirection = glm::normalize(_lookAt - _lookFrom);
	_verticalWorldAxis = glm::vec3(0.0f, 1.0f, 0.0f);

	_verticalFov = 60.0f;

	_viewportWidth = 800;
	_viewportHeight = 450;
	_lastViewportX = _viewportWidth / 2;
	_lastViewportY = _viewportHeight / 2;

	_yaw = 0.0;
	_pitch = 0.0;
	_roll = 0.0;
}

Camera::Camera(
	glm::vec3 lookFrom,
	glm::vec3 lookAt,
	glm::vec3 verticalWorldAxis,
	float verticalFov,
	uint32_t viewportWidth,
	uint32_t viewportHeight,
	double yaw,
	double pitch,
	double roll
			)
{
	_lookFrom = lookFrom;
	_lookAt = lookAt;
	_cameraDirection = glm::normalize(_lookAt - _lookFrom);
	_verticalWorldAxis = verticalWorldAxis;

	_verticalFov = verticalFov;

	_viewportWidth = viewportWidth;
	_viewportHeight = viewportHeight;
	_lastViewportX = _viewportWidth / 2;
	_lastViewportY = _viewportHeight / 2;

	_yaw = yaw;
	_pitch = pitch;
	_roll = roll;
}

void Camera::setViewportSize(uint32_t viewportWidth, uint32_t viewportHeight) 
	{ _viewportWidth = viewportWidth, _viewportHeight = viewportHeight; }

void Camera::setLookFrom(glm::vec3 lookFrom) { _lookFrom = lookFrom; }
void Camera::setLookAt(glm::vec3 lookAt) { _lookAt = lookAt; }
void Camera::setDirection(glm::vec3 cameraDirection) { _cameraDirection = cameraDirection; }
void Camera::setVerticalFov(float vFov) { _verticalFov = vFov; }
glm::vec3 Camera::getLookFrom() { return _lookFrom; }
glm::vec3 Camera::getLookAt() { return _lookAt; }
glm::vec3 Camera::getDirection() { return _cameraDirection; }
float Camera::getVerticalFov() { return _verticalFov; }

void Camera::setLastViewportX(double lastX) { _lastViewportX = lastX; }
void Camera::setLastViewportY(double lastY) { _lastViewportY = lastY; }
double Camera::getLastViewportX() { return _lastViewportX; }
double Camera::getLastViewportY() { return _lastViewportY; }

void Camera::setYaw(double yaw) { _yaw = yaw; }
void Camera::setPitch(double pitch) { _pitch = pitch; }
void Camera::setRoll(double roll) { _roll = roll; }
double Camera::getYaw() { return _yaw; }
double Camera::getPitch() { return _pitch; }
double Camera::getRoll() { return _roll; }

glm::vec3 Camera::getVerticalWorldAxis() { return _verticalWorldAxis; }
