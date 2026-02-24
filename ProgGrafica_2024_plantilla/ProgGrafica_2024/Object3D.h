#pragma once

#include "MathLib.h"
#include "Vertex.h"
#include "common.h"
#include "EventManager.h"

using namespace std;

class Object3D
{
public:
	unsigned int objId;
	Vector4f posicion;
	Vector4f rotacion;
	Vector4f escala;
	Matriz4x4f modelMatrix;

	vector<vertex_t> vertexList; // Lista de vertices del obj
	vector<int> idList; // Lista de indices para dibujar la lista de vertices

	/// <summary>
	/// Método que inicializa la lista de vértices con 3 vértices formando un triángulo cuyo
	/// centro se encontrará en el origen de coordenadas(0, 0, 0)
	/// </summary>
	void createTriangle();

	/// <summary>
	/// Método virtual (podrá ser reescrito por clases herederas) que actualiza la
	/// posición / rotación / escalado del objeto. Deberá consultar el estado del teclado, y si se
	///	pulsa la tecla “D” aumentará el ángulo de giro en el eje Y. 
	/// Si se pulsa la tecla “A” disminuirá el ángulo de giro en el eje Y
	/// </summary>
	/// <param name="timeStep"></param>
	virtual void move(double timeStep);

	/// <summary>
	/// Método que actualiza la matriz modelo en base a los datos de posición, rotación y
	/// escalado de este objeto.
	/// </summary>
	void updateModelMatrix();
};