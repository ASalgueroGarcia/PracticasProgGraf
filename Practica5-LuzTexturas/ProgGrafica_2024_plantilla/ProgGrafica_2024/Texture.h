#pragma once

#include "common.h"
#include <string>
#include <vector>
#include <iostream>

class Texture
{
public:
    std::string textureName;
    unsigned int textureId;
    int width;
    int height;
    std::vector<unsigned char> rawImage;
    Texture(std::string fileName);
    void updateTexture();
};