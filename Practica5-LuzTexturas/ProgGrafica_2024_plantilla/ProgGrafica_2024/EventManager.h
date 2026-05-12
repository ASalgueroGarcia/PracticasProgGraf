#pragma once
#include "common.h"

using namespace std;

class EventManager
{
public:
	static inline map<int, bool> keyMap; //estado del teclado
	static inline GLFWwindow* window; // inline: permite hacer variables estaticas

	static void initEventManager(GLFWwindow* window);
	static void keyboardManager(GLFWwindow* window, int key, int scancode, int action, int mods); //eventos de teclado
	static void updateEvents();
};