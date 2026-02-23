#pragma once

#include "common.h"

class InputManager
{
	/// <summary>
	/// Mapa que guarda el estado del teclado. Se puede consultar si una tecla está pulsada o no
	/// </summary>
	static std::map<int, bool> keysState;

	/// <summary>
	/// Método de tipo “GLFWKeyFunc”, que recibirá los eventos de teclado y actualizará el mapa de teclas
	/// </summary>
	/// <param name="window"></param>
	/// <param name="key"></param>
	/// <param name="scancode"></param>
	/// <param name="action"></param>
	/// <param name="mods"></param>
	static void keyboardManager(GLFWwindow* window, int key, int scancode, int action, int mods);

	/// <summary>
	/// Método que recibe por parámetros la ventana de glfw sobre la que se gestionarán los
	/// eventos.Se inicializará el evento de teclado usando la función keyboardManager como
	///	parámetro.
	/// </summary>
	/// <param name="window"></param>
	static void init(GLFWwindow* window);
};

