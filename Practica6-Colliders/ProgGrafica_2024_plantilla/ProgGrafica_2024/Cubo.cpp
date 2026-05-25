#include "Cubo.h"
#include <cmath>

Cubo::Cubo()
{
    posicion = make_vector4f(3, 0, 0, 1);
    rotacion = make_vector4f(0, 0, 0, 0);
    escala = make_vector4f(1, 1, 1, 0);
    updateModelMatrix();
    initialPos = make_vector3f(3, 0, 0);
    angle = 0.0f;
}

void Cubo::move(double timeStep)
{
    float speed = 45.0f * 3.14159265f / 180.0f;
    angle += speed * static_cast<float>(timeStep);
    posicion.x = initialPos.x * cos(angle);
    posicion.z = initialPos.x * sin(angle);
    updateModelMatrix();
}