#include "pch.h"
#include "Camera.h"

Camera::Camera() {}

void Camera::setWidthHeight(uint32_t width, uint32_t height) { _width = width, _height = height; }

void Camera::setLookFrom(glm::vec3 lookFrom) { _lookFrom = lookFrom; }
void Camera::setLookAt(glm::vec3 lookAt) { _lookAt = lookAt; }
void Camera::setDirection(glm::vec3 cameraDirection) { _cameraDirection = cameraDirection; }
void Camera::setVerticalFov(float vFov) { _verticalFov = vFov; }
glm::vec3 Camera::getLookFrom() { return _lookFrom; }
glm::vec3 Camera::getLookAt() { return _lookAt; }
glm::vec3 Camera::getDirection() { return _cameraDirection; }
float Camera::getVerticalFov() { return _verticalFov; }

void Camera::setLastX(double lastX) { _lastX = lastX; }
void Camera::setLastY(double lastY) { _lastY = lastY; }
double Camera::getLastX() { return _lastX; }
double Camera::getLastY() { return _lastY; }

void Camera::setYaw(double yaw) { _yaw = yaw; }
void Camera::setPitch(double pitch) { _pitch = pitch; }
void Camera::setRoll(double roll) { _roll = roll; }
double Camera::getYaw() { return _yaw; }
double Camera::getPitch() { return _pitch; }
double Camera::getRoll() { return _roll; }

glm::vec3 Camera::getVerticalWorldAxis() { return _verticalWorldAxis; }
