#include "common.h"
#include "Render.h"
#include "Object3D.h"
#include "Camera.h"

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	Render renderer(640, 480);

	Object3D cubo;

	cubo.loadFromFile("data/cubo.frs");

	cubo.posicion = make_vector4f(0, 0, 0, 1);
	cubo.rotacion = make_vector4f(0.785f, 0, 0.785f, 0);
	cubo.escala = make_vector4f(1, 1, 1, 0);

	cubo.updateModelMatrix();

	renderer.putObject(&cubo);

	Camera cam(
		make_vector4f(0, 0, -3, 1),
		make_vector4f(0, 0, 0, 0),
		make_vector4f(0, 0, 0, 1),
		make_vector4f(0, 1, 0, 0),
		90.0f, 640.0f / 480.0f, 0.01f, 100.0f
	);

	renderer.putCamera(&cam);

	renderer.mainLoop();

	glfwTerminate();

	return 0;
}