#define GLAD_BIN
#include "common.h"
#include "Object3D.h"
#include <iostream>
#include "EventManager.h"
#include "Render.h"
#include "System.h"

int main(int argc, char** argv)
{
    System::render = new Render();

    auto cubo = new Object3D("data/cube.frs");
    cubo->posicion = { 0, 0, 0, 1 };
    System::render->addObject(cubo);

    System::render->luz->posicion = { 3, 3, 3, 1 };

    delete System::render->cam;
    System::render->cam = new Camera({ 0, 0, 3, 1 }, { 0, 0, 0, 1 });

    System::render->mainLoop();

    return 0;
}