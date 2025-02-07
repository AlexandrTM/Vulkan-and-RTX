#include "pch.h"
#include "Camera.h"

Camera::Camera() 
{
	_lookFrom = glm::vec3(0.0f, 4.0f, 0.0f);
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

void Camera::rotateAbsolute(double xpos, double ypos, double sensitivity)
{
	if (_firstMouse)
	{
		_lastViewportX = xpos;
		_lastViewportY = ypos;
		_firstMouse = false;
	}
	double roll = _roll;

	double xoffset = xpos - _lastViewportX;
	double yoffset = _lastViewportY - ypos;
	_lastViewportX = xpos;
	_lastViewportY = ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	if (_pitch + yoffset > 89.99) {
		_pitch = 89.99;
	}
	else if (_pitch + yoffset < -89.99) {
		_pitch = -89.99;
	}
	else {
		_pitch = _pitch + yoffset;
	}

	_yaw = _yaw + xoffset;

	glm::vec3 front{};
	front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.y = sin(glm::radians(_pitch));
	front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_cameraDirection = glm::normalize(front);
}
void Camera::rotateRelative(double dx, double dy, double sensitivity)
{
	dx *= sensitivity;
	dy *= -sensitivity;

	if (_pitch + dy > 89.99) {
		_pitch = 89.99;
	}
	else if (_pitch + dy < -89.99) {
		_pitch = -89.99;
	}
	else {
		_pitch = _pitch + dy;
	}

	_yaw = _yaw + dx;

	glm::vec3 front{};
	front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.y = sin(glm::radians(_pitch));
	front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_cameraDirection = glm::normalize(front);
}

void Camera::setViewportSize(uint32_t viewportWidth, uint32_t viewportHeight) 
	{ _viewportWidth = viewportWidth, _viewportHeight = viewportHeight; }

void Camera::setLookFrom(glm::vec3 lookFrom) { _lookFrom = lookFrom; }
void Camera::setLookAt(glm::vec3 lookAt) { _lookAt = lookAt; }
void Camera::setDirection(glm::vec3 cameraDirection) { _cameraDirection = cameraDirection; }
void Camera::setVerticalFov(float vFov) { _verticalFov = vFov; }
glm::vec3 Camera::getLookFrom() const { return _lookFrom; }
glm::vec3 Camera::getLookAt() const { return _lookAt; }
glm::vec3 Camera::getDirection() const { return _cameraDirection; }
float Camera::getVerticalFov() const { return _verticalFov; }

void Camera::setLastViewportX(double lastX) { _lastViewportX = lastX; }
void Camera::setLastViewportY(double lastY) { _lastViewportY = lastY; }
double Camera::getLastViewportX() const { return _lastViewportX; }
double Camera::getLastViewportY() const { return _lastViewportY; }

void Camera::setYaw(double yaw) { _yaw = yaw; }
void Camera::setPitch(double pitch) { _pitch = pitch; }
void Camera::setRoll(double roll) { _roll = roll; }
double Camera::getYaw() const { return _yaw; }
double Camera::getPitch() const { return _pitch; }
double Camera::getRoll() const { return _roll; }

glm::vec3 Camera::getVerticalWorldAxis() { return _verticalWorldAxis; }
