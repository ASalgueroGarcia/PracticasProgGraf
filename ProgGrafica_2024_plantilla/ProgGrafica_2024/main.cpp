#include "common.h"
#include "Render.h"

int main()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    Render renderer(800.0f, 600.0f);

    renderer.mainLoop();

    glfwTerminate();
    return 0;
}