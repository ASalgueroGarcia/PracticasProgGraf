#pragma once
#include "Object3D.h"

class Cubo : public Object3D
{
public:
    Cubo();
    void move(double timeStep) override;
};