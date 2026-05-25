#include "Material.h"

Material::Material(std::string textureName, float ka, float kd, float ks)
{
    usaTextura = false;
    shiny = 1;
    alpha = 1.0f;
    this->ka= ka;
    this->kd = kd;
    this->ks = ks;
    texture = new Texture(textureName);
    if (texture != nullptr && texture->textureId != 0) {
        usaTextura = true;
    }
}