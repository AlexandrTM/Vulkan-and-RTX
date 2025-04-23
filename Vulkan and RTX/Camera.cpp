#include "pch.h"
#include "Camera.h"

Camera::Camera() 
{
	_position = glm::vec3(0.0f, 0.0f, 0.0f);
	//_lookAt = _position + glm::vec3(1.0f, 0.0f, 0.0f);
	//_cameraDirection = glm::normalize(_lookAt - _position);
	_verticalWorldAxis = glm::vec3(0.0f, 1.0f, 0.0f);

	_verticalFov = 63.0f;

	_viewportWidth = 800;
	_viewportHeight = 450;
	_lastXScreenPosition = _viewportWidth / 2;
	_lastYScreenPosition = _viewportHeight / 2;

	_yaw = 0.0;
	_pitch = -50.0;
	_roll = 0.0;

	_targetYaw = 0.0;
	_targetPitch = -50.0;
	_targetRoll = 0.0;

	glm::vec3 front{};
	front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.y = sin(glm::radians(_pitch));
	front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_lookAt = _position + glm::normalize(front);
	_cameraDirection = glm::normalize(front);
}

Camera::Camera(
	glm::vec3 position,
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
	_position = position;
	_lookAt = lookAt;
	_cameraDirection = glm::normalize(_lookAt - _position);
	_verticalWorldAxis = verticalWorldAxis;

	_verticalFov = verticalFov;

	_viewportWidth = viewportWidth;
	_viewportHeight = viewportHeight;
	_lastXScreenPosition = _viewportWidth / 2;
	_lastYScreenPosition = _viewportHeight / 2;

	_yaw = yaw;
	_pitch = pitch;
	_roll = roll;
}

void Camera::handleRotation(double& latestMouseDx, double& latestMouseDy)
{
	if (_isFirstMouse) {
		_isFirstMouse = false;
		return;
	}

	if (!_isFirstMouse) {
		addRotationDelta(latestMouseDx, latestMouseDy);
		latestMouseDx = 0.0;
		latestMouseDy = 0.0;
	}

	interpolateRotation(1.0);
}

void Camera::rotateAbsolute(double xpos, double ypos, double sensitivity)
{
	if (_isFirstMouse)
	{
		_lastXScreenPosition = xpos;
		_lastYScreenPosition = ypos;
		_isFirstMouse = false;
	}
	double roll = _roll;

	double xoffset = xpos - _lastXScreenPosition;
	double yoffset = _lastYScreenPosition - ypos;
	_lastXScreenPosition = xpos;
	_lastYScreenPosition = ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	_pitch = std::clamp(_pitch + yoffset, -89.99, 89.99);

	_yaw = _yaw + xoffset;

	glm::vec3 front{};
	front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.y = sin(glm::radians(_pitch));
	front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_lookAt = _position + glm::normalize(front);
	_cameraDirection = glm::normalize(front);
}
void Camera::rotateRelative(double dx, double dy, double sensitivity)
{
	dx *= sensitivity;
	dy *= -sensitivity;

	_pitch = std::clamp(_pitch + dy, -89.99, 89.99);

	_yaw = _yaw + dx;

	glm::vec3 front{};
	front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.y = sin(glm::radians(_pitch));
	front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_lookAt = _position + glm::normalize(front);
	_cameraDirection = glm::normalize(front);
}

void Camera::interpolateRotation(double lerpFactor) {
	_yaw += (_targetYaw - _yaw) * lerpFactor;
	_pitch += (_targetPitch - _pitch) * lerpFactor;

	/*_pitch = -45.99;
	_yaw += 1.0;*/

	_pitch = std::clamp(_pitch, -89.99, 89.99);

	glm::vec3 front{};
	front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.y = sin(glm::radians(_pitch));
	front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_lookAt = _position + glm::normalize(front);
	_cameraDirection = glm::normalize(front);
}

void Camera::addRotationDelta(double dx, double dy) {
	_targetYaw += dx;
	_targetPitch = std::clamp(_targetPitch + dy, -89.99, 89.99);
}

void Camera::setViewportSize(uint32_t viewportWidth, uint32_t viewportHeight) 
	{ _viewportWidth = viewportWidth, _viewportHeight = viewportHeight; }

void Camera::setPosition(glm::vec3 position) { _position = position; }
void Camera::setLookAt(glm::vec3 lookAt) { _lookAt = lookAt; }
void Camera::setDirection(glm::vec3 cameraDirection) { _cameraDirection = cameraDirection; }
void Camera::setVerticalFov(float vFov) { _verticalFov = vFov; }
glm::vec3& Camera::getPosition() { return _position; }
glm::vec3& Camera::getLookAt() { return _lookAt; }
glm::vec3& Camera::getDirection() { return _cameraDirection; }
float& Camera::getVerticalFov() { return _verticalFov; }

void Camera::setLastViewportX(double lastX) { _lastXScreenPosition = lastX; }
void Camera::setLastViewportY(double lastY) { _lastYScreenPosition = lastY; }
double& Camera::getLastViewportX() { return _lastXScreenPosition; }
double& Camera::getLastViewportY() { return _lastYScreenPosition; }

void Camera::setYaw(double yaw) { _yaw = yaw; }
void Camera::setPitch(double pitch) { _pitch = pitch; }
void Camera::setRoll(double roll) { _roll = roll; }
double& Camera::getYaw() { return _yaw; }
double& Camera::getPitch() { return _pitch; }
double& Camera::getRoll() { return _roll; }

glm::vec3& Camera::getVerticalWorldAxis() { return _verticalWorldAxis; }
