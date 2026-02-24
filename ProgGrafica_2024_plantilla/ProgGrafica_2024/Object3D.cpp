#include "Object3D.h"

void Object3D::createTriangle()
{
	objId++;

	vertexList = {
		{ 0.0f, 0.5f, 0.0f, 1.0f },		// vertice superior
		{ 0.5f, -0.5f, 0.0f, 1.0f },	// vertice inferior derecho
		{ -0.5f, -0.5f, 0.0f, 1.0f }	// vertice inferior izq
	};

	idList = { 1,2,3};
}

void Object3D::move(double timeStep)
{
	auto speed = 0.25;

	if (EventManager::keyMap[GLFW_KEY_D]) rotacion.y += speed * timeStep;
	if (EventManager::keyMap[GLFW_KEY_A]) rotacion.y -= speed * timeStep;

	updateModelMatrix();
}

void Object3D::updateModelMatrix()
{
	// update posicion
	Matriz4x4f T = make_translate(posicion.x, posicion.y, posicion.z);

	// update rotacion
	Matriz4x4f R = make_rotate(rotacion.x, rotacion.y, rotacion.z);

	// update escala
	Matriz4x4f S = make_scale(escala.x, escala.y, escala.z);

	modelMatrix = T * R * S;
}