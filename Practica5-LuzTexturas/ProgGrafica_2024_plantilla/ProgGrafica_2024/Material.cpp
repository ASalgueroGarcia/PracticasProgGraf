#include "Material.h"

Material::Material(std::string textureName, float ka, float kd, float ks)
{
    usaTextura = true;
    shiny = 1;
    alpha = 1.0f;
    ka = ka;
    kd = kd;
    ks = ks;
    texture = new Texture(textureName);
}