#pragma once
#include "Object3D.h"
#include "MathLib.h"

class Cubo : public Object3D
{
public:
    Cubo();
    void move(double timeStep) override;

private:
    Vector3f initialPos;
    float angle;
};