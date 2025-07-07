#include "pch.h"
#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() {}
ShaderProgram::~ShaderProgram() {
    if (programID) glDeleteProgram(programID);
}

bool ShaderProgram::LoadFromFiles(const std::string& vertPath, const std::string& fragPath) {
    std::string vertCode = LoadFile(vertPath);
    std::string fragCode = LoadFile(fragPath);
    if (vertCode.empty() || fragCode.empty()) return false;

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    if (!CompileShader(vertShader, vertCode)) return false;
    if (!CompileShader(fragShader, fragCode)) return false;

    programID = glCreateProgram();
    glAttachShader(programID, vertShader);
    glAttachShader(programID, fragShader);
    glLinkProgram(programID);

    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(programID, 512, nullptr, info);
        std::cerr << "Shader link error:\n" << info << std::endl;
        return false;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    return true;
}

void ShaderProgram::Use() const {
    glUseProgram(programID);
}

GLuint ShaderProgram::GetID() const {
    return programID;
}

std::string ShaderProgram::LoadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool ShaderProgram::CompileShader(GLuint shader, const std::string& source) {
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cerr << "Shader compile error:\n" << info << std::endl;
        return false;
    }
    return true;
}

GLint ShaderProgram::GetUniformLocation(const std::string& name) {
    if (uniformLocations.find(name) != uniformLocations.end())
        return uniformLocations[name];

    GLint loc = glGetUniformLocation(programID, name.c_str());
    uniformLocations[name] = loc;
    return loc;
}

// Uniform Setters
void ShaderProgram::SetUniform(const std::string& name, float value) {
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location == -1) {
        //std::cout << "[Shader] Uniform not found: " << name << std::endl;
        return;
    }
    glUniform1f(GetUniformLocation(name), value);
}
void ShaderProgram::SetUniform(const std::string& name, int value) {
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location == -1) {
        //std::cout << "[Shader] Uniform not found: " << name << std::endl;
        return;
    }
    glUniform1i(GetUniformLocation(name), value);
}
void ShaderProgram::SetUniform(const std::string& name, const glm::vec2& value) {
    glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}
void ShaderProgram::SetUniform(const std::string& name, const glm::vec3& value) {
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location == -1) {
        //std::cout << "[Shader] Uniform not found: " << name << std::endl;
        return;
    }
    glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}
void ShaderProgram::SetUniform(const std::string& name, const glm::vec4& value) {
    

    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location == -1) {
        //std::cout << "[Shader] Uniform not found: " << name << std::endl;
        return;
    }
    glUniform4fv(location, 1, glm::value_ptr(value));
}
void ShaderProgram::SetUniform(const std::string& name, const glm::mat4& value) {
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location == -1) {
        //std::cout << "[Shader] Uniform not found: " << name << std::endl;
        return;
    }
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}
