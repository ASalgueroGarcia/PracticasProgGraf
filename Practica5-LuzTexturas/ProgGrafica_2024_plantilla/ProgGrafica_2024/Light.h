#pragma once

#include "common.h"
#include "MathLib.h"

class Light {
public:
    Vector4f direction, position, color;
    float ka, kd, ks;

    Light(Vector4f direction, Vector4f position, Vector4f color, float ka, float kd, float ks);
    
    void move(double timeStep);
};