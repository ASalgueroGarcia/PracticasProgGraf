#include "common.h"
#include "Render.h"
#include "Object3D.h"

int main()
{
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	Render renderer(800.0f, 600.0f);

	// Crear triángulo y añadirlo al renderer
	Object3D triangle;
	triangle.posicion = make_vector4f(0.0f, 0.0f, 0.0f, 1.0f);
	triangle.rotacion = make_vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	triangle.escala = make_vector4f(1.0f, 1.0f, 1.0f, 0.0f);
	triangle.createTriangle();

	renderer.putObject(&triangle);

	renderer.mainLoop();

	glfwTerminate();
	return 0;
}