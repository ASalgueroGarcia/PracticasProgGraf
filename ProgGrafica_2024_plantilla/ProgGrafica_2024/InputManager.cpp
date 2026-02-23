#include "InputManager.h"

void InputManager::keyboardManager(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	switch (action) {
		case GLFW_PRESS:
			InputManager::keysState[key] = true;
			break;
		case GLFW_RELEASE:
			InputManager::keysState[key] = false;
			break;
		default:
			std::cout << "ERROR: Evento de teclado no gestionado: " << action << std::endl;
			break;
	}
}

void InputManager::init(GLFWwindow* window)
{
	glfwSetKeyCallback(window, keyboardManager);
}
