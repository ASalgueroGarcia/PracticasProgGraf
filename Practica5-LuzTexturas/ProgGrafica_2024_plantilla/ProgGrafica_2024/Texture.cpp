#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "stb_image.h"

Texture::Texture(std::string fileName)
{
    textureName = fileName;
    textureId = 0;
    width = 0;
    height = 0;
    int channels;
    stbi_uc* data = stbi_load(textureName.c_str(), &width, &height, &channels, STBI_rgb);
    if (data == nullptr) {
        std::cout << "ERROR loading texture: " << textureName << std::endl;
        return;
    }
    size_t size = width * height * channels;
    rawImage.assign(data, data + size);
    stbi_image_free(data);
    glGenTextures(1, &textureId);
    updateTexture();
}

void Texture::updateTexture()
{
    if (rawImage.empty()) return;
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rawImage.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}