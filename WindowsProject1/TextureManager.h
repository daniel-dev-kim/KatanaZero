#pragma once

struct TextureData {
    GLuint textureID = 0;  // OpenGL �ؽ�ó ID
    int width = 0;
    int height = 0;
};


class TextureManager {
public:
    ~TextureManager();
    bool LoadTexture(const std::string& key, const std::string& filePath);
    TextureData* GetTexture(const std::string& key);

private:
    std::unordered_map<std::string, TextureData> textures;
};