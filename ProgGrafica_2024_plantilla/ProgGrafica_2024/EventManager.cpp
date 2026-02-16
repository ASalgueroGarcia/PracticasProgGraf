#include "EventManager.h"

void EventManager::initEventManager(GLFWwindow* window)
{
	glfwSetKeyCallback(window, keyboardManager); // Vincular la ventana con el teclado
	EventManager::window = window;
}

void EventManager::keyboardManager(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (action) {
	case GLFW_PRESS:
		EventManager::keyMap[key] = true;
		break;
	case GLFW_RELEASE:
		EventManager::keyMap[key] = false;
		break;
	default:
		cout << "Evento de teclado no gestionado: " << action << endl;
		break;
	}
}

void EventManager::updateEvents()
{
	glfwPollEvents(); //Actualizar resto de eventos de ventana
}
