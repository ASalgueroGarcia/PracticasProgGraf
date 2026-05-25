#include "common.h"
#include "Render.h"
#include "Object3D.h"
#include "Camera.h"
#include "Light.h"
#include "Cubo.h"
#include "EventManager.h"

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    Render renderer(640, 480);
    renderer.initGL();

    Object3D esfera;
    esfera.loadFromFile("data/sphere.frs");
    esfera.posicion = make_vector4f(0, 0, 0, 1);
    esfera.escala = make_vector4f(1, 1, 1, 0);
    esfera.updateModelMatrix();

    for (auto& v : esfera.vertexList) {
        v.color = make_vector4f(1.0f, 0.0f, 0.0f, 1.0f);
    }

    renderer.putObject(&esfera);

    Cubo cubo;
    cubo.loadFromFile("data/cubeText.frs");
    cubo.posicion = make_vector4f(3, 0, 0, 1);
    cubo.escala = make_vector4f(1, 1, 1, 0);
    cubo.updateModelMatrix();
    renderer.putObject(&cubo);

    Camera cam(
        make_vector4f(0, 0, 6, 1),
        make_vector4f(0, 0, 0, 0),
        make_vector4f(0, 0, 0, 1),
        make_vector4f(0, 1, 0, 0),
        90.0f, 640.0f / 480.0f, 0.01f, 100.0f
    );
    renderer.putCamera(&cam);

    Light light(
        make_vector4f(0, 0, 0, 0),
        make_vector4f(0, 0, 0, 1),
        make_vector4f(1, 1, 1, 1),
        0.2f,
        0.7f,
        0.3f
    );
    renderer.putLight(&light);

    renderer.mainLoop();

    glfwTerminate();

    return 0;
}