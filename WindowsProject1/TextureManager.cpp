#include "pch.h"
#include "TextureManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureManager::~TextureManager() {
    for (auto& tex : textures) {
        if (tex.second.textureID != 0) {
            glDeleteTextures(1, &tex.second.textureID);
        }
    }
}

bool TextureManager::LoadTexture(const std::string& key, const std::string& filePath) {
    if (textures.find(key) != textures.end())
        return true;
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    unsigned char* imageData = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!imageData) {
        printf("Failed to load image: %s\n", filePath.c_str());
        return false;
    }

    GLuint texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // 텍스처 데이터 업로드
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    // 텍스처 필터링 및 랩핑 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 클린업
    stbi_image_free(imageData);
    glBindTexture(GL_TEXTURE_2D, 0);

    TextureData tex;
    tex.textureID = texID;
    tex.width = width;
    tex.height = height;

    textures[key] = tex;
    return true;
}

TextureData* TextureManager::GetTexture(const std::string& key) {
    auto it = textures.find(key);
    if (it == textures.end()) return nullptr;
    return &it->second;
}