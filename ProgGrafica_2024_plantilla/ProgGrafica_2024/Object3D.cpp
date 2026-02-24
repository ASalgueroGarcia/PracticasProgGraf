#include "Object3D.h"

void Object3D::createTriangle()
{
	objId++;

	vertexList = {
		{ 0, 0.5, 0, 1 },		// vertice superior
		{ 0.5, -0.5, 0, 1 },	// vertice inferior derecho
		{ -0.5, -0.5, 0, 1 }	// vertice inferior izq
	};

	idList = { 1,2,3 };
}

void Object3D::move(double timeStep)
{
	auto speed = 0.25;

	if (EventManager::keyMap[GLFW_KEY_D]) rotacion.y += speed * timeStep;
	if (EventManager::keyMap[GLFW_KEY_A]) rotacion.y -= speed * timeStep;
}

void Object3D::updateModelMatrix()
{
	// update posicion
	modelMatrix = make_translate(posicion.x, posicion.y, posicion.z);

	// update rotacion
	modelMatrix = make_rotate(rotacion.x, rotacion.y, rotacion.z);

	// update escala
	modelMatrix = make_scale(escala.x, escala.y, escala.z);
}
