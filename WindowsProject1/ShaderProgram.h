#pragma once
class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    bool LoadFromFiles(const std::string& vertPath, const std::string& fragPath);
    void Use() const;
    GLuint GetID() const;

    // À¯´ÏÆû ¼³Á¤
    void SetUniform(const std::string& name, float value);
    void SetUniform(const std::string& name, int value);
    void SetUniform(const std::string& name, const glm::vec2& value);
    void SetUniform(const std::string& name, const glm::vec3& value);
    void SetUniform(const std::string& name, const glm::vec4& value);
    void SetUniform(const std::string& name, const glm::mat4& value);

private:
    GLuint programID = 0;
    std::unordered_map<std::string, GLint> uniformLocations;

    std::string LoadFile(const std::string& path);
    bool CompileShader(GLuint shader, const std::string& source);
    GLint GetUniformLocation(const std::string& name);
};