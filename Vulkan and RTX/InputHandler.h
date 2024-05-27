#include "pch.h"
#include "Camera.h"
#include "InteractableVolume.h"

#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

class InputHandler
{
private:
	double sensitivity = 0.125;

	bool keys[1024] = { 0 };

public:
	Camera camera;

	InteractableVolume interactableCuboid = InteractableVolume(
		glm::vec3(-1.0, 0.0, -1.0), glm::vec3(1.0, 3.0, 1.0));

	void initializeInputHandler(GLFWwindow* window);

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	// mouse wheel handling
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	void movePerson(float deltaTime);
};

#endif // !INPUT_HANDLER_H