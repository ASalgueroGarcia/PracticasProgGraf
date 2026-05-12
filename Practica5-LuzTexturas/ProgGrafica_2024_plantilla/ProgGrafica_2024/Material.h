#pragma once

#include "common.h"
#include "Texture.h"

class Material
{
public:
    bool usaTextura;
    int shiny;
    float alpha;
    float ka, kd, ks;
    Texture* texture;

    Material(std::string textureName, float ka, float kd, float ks);
};