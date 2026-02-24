#pragma once

#include "InputManager.h"
#include "Object3D.h"
#include "common.h"

class Render
{
	typedef struct {
		unsigned int bufferId;
		unsigned int vertexBufferId;
		unsigned int indexBufferId;
	} bufferObject_t;

	/// <summary>
	/// Atributo que almacena una referencia a la ventana de opengl que usaremos para dibujar
	/// </summary>
	GLFWwindow* window;

	/// <summary>
	/// Lista de objetos que se dibujarán en la ventana de openGL
	/// </summary>
	std::vector<Object3D> objectList;

	/// <summary>
	/// Lista de identificadores de buffer
	/// </summary>
	std::map<int, bufferObject_t> bufferList;

	Render(float anchura, float altura);

	/// <summary>
	/// La clase render recibía en su constructor el tamaño del frameBuffer (coincide con la
	/// resolución de la pantalla o tamaño de la ventana). Este método se usará para crear una
	/// ventana de openGL con esos datos, e inicializar el sistema de renderizado.
	/// También llamará al método “initInputManager” de la clase InputManager para poder
	/// inicializar los eventos de teclado con la ventana creada anteriormente.
	/// </summary>
	void initGL();

	/// <summary>
	/// Método que añade un objeto a la lista de objetos. Debe crear su bufferObject y
	/// almacenar en el mapa bufferObjets los identificadores de buffers creados / cargados en GPU.
	/// </summary>
	/// <param name="obj">: Objeto que se desea añadir</param>
	void putObject(Object3D* obj);

	/// <summary>
	/// Método que elimina un objeto a la lista de objetos. Debe destruir su bufferObject y
	/// buffers creados / cargados en GPU.
	/// </summary>
	/// <param name="obj">: Objeto que se desea eliminar</param>
	void removeObject(Object3D* obj);

	/// <summary>
	/// Método que dibuja los objetos en la ventana de openGL. Este método recorre la lista de
	/// objetos, y por cada uno de ellos los carga en GPU y dibuja sus datos de vértices
	/// </summary>
	void DrawGL();

	/// <summary>
	/// Método que se mantiene en un bucle mientras esté abierta la ventana de OpenGL
	/// </summary>
	void mainLoop();
};

