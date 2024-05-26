#include "pch.h"
#include "InputHandler.h"
#include "Camera.h"

void InputHandler::initializeInputHandler(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mode) {
		InputHandler* inputHandler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		inputHandler->keyCallback(window, key, scancode, action, mode);
		});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		InputHandler* inputHandler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		inputHandler->mouseButtonCallback(window, button, action, mods);
		});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		InputHandler* inputHandler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		inputHandler->mouseCallback(window, xpos, ypos);
		});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		InputHandler* inputHandler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		inputHandler->scrollCallback(window, xoffset, yoffset);
		});
}

void InputHandler::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) 
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// altering mouse sensivity
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		sensitivity = std::clamp(sensitivity * 1.3, 0.001, 10.0);
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		sensitivity = std::clamp(sensitivity * 0.75, 0.001, 10.0);
	}
	// changing mipmap level of detail
	/*if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		if (!(currentMinMipLevels < 0.0f))
		{
			currentMinMipLevels -= 1.0f;
		}
		if (currentMinMipLevels < 0.0f)
		{
			currentMinMipLevels = 0.0f;
		}
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		if (!(currentMinMipLevels > 14.0f))
		{
			currentMinMipLevels += 1.0f;
		}
		if (currentMinMipLevels > 14.0f)
		{
			currentMinMipLevels = 14.0f;
		}
	}*/
}
void InputHandler::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) 
	{
		camera.setVerticalFov(60.0f);
	}
}
void InputHandler::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	camera.rotate(xpos, ypos, sensitivity);
}
// mouse wheel handling
void InputHandler::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.setVerticalFov(std::clamp(camera.getVerticalFov() - static_cast<float>(yoffset), 1.0f, 130.0f));
}

void InputHandler::movePerson(float deltaTime)
{
	float movementSpeed = 8.0 * deltaTime;

	glm::vec3 verticalWorldAxis = camera.getVerticalWorldAxis();
	glm::vec3 cameraDirection = camera.getDirection();

	glm::vec3 rightVector = glm::normalize(glm::cross(cameraDirection, verticalWorldAxis));

	if (keys[GLFW_KEY_LEFT_CONTROL])
	{
		movementSpeed *= 1.9;
	}
	if (keys[GLFW_KEY_LEFT_ALT])
	{
		movementSpeed *= 0.4;
	}
	if (keys[GLFW_KEY_W])
	{
		camera.setLookFrom(camera.getLookFrom() + movementSpeed * glm::normalize(glm::vec3(
			cameraDirection.x,
			0.0f,
			cameraDirection.z)
		));
	}
	if (keys[GLFW_KEY_A])
	{
		camera.setLookFrom(camera.getLookFrom() - rightVector * movementSpeed);
	}
	if (keys[GLFW_KEY_S])
	{
		camera.setLookFrom(camera.getLookFrom() - movementSpeed * glm::normalize(glm::vec3(
			cameraDirection.x,
			0.0f,
			cameraDirection.z)
		));
	}
	if (keys[GLFW_KEY_D])
	{
		camera.setLookFrom(camera.getLookFrom() + rightVector * movementSpeed);
	}
	if (keys[GLFW_KEY_SPACE])
	{
		camera.setLookFrom(camera.getLookFrom() + verticalWorldAxis * movementSpeed * 0.7f);
	}
	if (keys[GLFW_KEY_LEFT_SHIFT])
	{
		camera.setLookFrom(camera.getLookFrom() - verticalWorldAxis * movementSpeed * 0.7f);
	}
}
