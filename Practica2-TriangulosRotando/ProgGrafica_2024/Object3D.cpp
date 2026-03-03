#include "Object3D.h"
#include "EventManager.h"

// Generador simple de IDs único por proceso
static unsigned int nextObjectId = 1;

void Object3D::createTriangle()
{
	// Asignar ID único
	objId = nextObjectId++;

	// Inicializar vértices explícitamente
	vertexList = {
		vertex_t{ make_vector4f(0.0f, 0.5f, 0.0f, 1.0f) },		// vértice superior
		vertex_t{ make_vector4f(0.5f, -0.5f, 0.0f, 1.0f) },	// vértice inferior derecho
		vertex_t{ make_vector4f(-0.5f, -0.5f, 0.0f, 1.0f) }	// vértice inferior izq
	};

	// Indices en base 0
	idList = { 0, 1, 2 };
}

void Object3D::move(double timeStep)
{
	auto speed = 0.25f;

	if (EventManager::keyMap[GLFW_KEY_D]) rotacion.y += speed * static_cast<float>(timeStep);
	if (EventManager::keyMap[GLFW_KEY_A]) rotacion.y -= speed * static_cast<float>(timeStep);

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