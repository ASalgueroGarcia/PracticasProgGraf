#include "Cubo.h"

Cubo::Cubo()
{
    posicion = make_vector4f(0, 0, 0, 1);
    rotacion = make_vector4f(0, 0, 0, 0);
    escala = make_vector4f(1, 1, 1, 0);
    updateModelMatrix();
}

void Cubo::move(double timeStep)
{
    float speed = 0.785f;
    rotacion.y += speed * static_cast<float>(timeStep);
    updateModelMatrix();
}