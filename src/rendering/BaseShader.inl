#include <glm/gtc/type_ptr.hpp>

namespace clgl {
    inline BaseShader::BaseShader(const std::unordered_map<GLuint, std::string> &stages)
            : Shader(stages) {}

    inline void BaseShader::uniform(const std::string &name, const float value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform1f(ID(), location, value));
    }

    inline void BaseShader::uniform(const std::string &name, const float *value, unsigned int count) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform1fv(ID(), location, count, value));
    }

    inline void BaseShader::uniform(const std::string &name, const int value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform1i(ID(), location, value));
    }

    inline void BaseShader::uniform(const std::string &name, const int *value, unsigned int count) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform1iv(ID(), location, count, value));
    }

    inline void BaseShader::uniform(const std::string &name, const unsigned int value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform1ui(ID(), location, value));
    }

    inline void BaseShader::uniform(const std::string &name, const unsigned int *value, unsigned int count) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform1uiv(ID(), location, count, value));
    }

    inline void BaseShader::uniform(const std::string &name, const glm::vec2 &value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform2f(ID(), location, value[0], value[1]));
    }

    inline void BaseShader::uniform(const std::string &name, const glm::ivec2 &value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform2i(ID(), location, value[0], value[1]));
    }

    inline void BaseShader::uniform(const std::string &name, const glm::uvec2 &value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform2ui(ID(), location, value[0], value[1]));
    }

    inline void BaseShader::uniform(const std::string &name, const glm::vec3 &value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform3f(ID(), location, value[0], value[1], value[2]));
    }

    inline void BaseShader::uniform(const std::string &name, const glm::ivec3 &value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform3i(ID(), location, value[0], value[1], value[2]));
    }

    inline void BaseShader::uniform(const std::string &name, const glm::uvec3 &value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform3ui(ID(), location, value[0], value[1], value[2]));
    }

    inline void BaseShader::uniform(const std::string &name, const glm::vec4 &value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform4f(ID(), location, value.r, value.g, value.b, value.a));
    }

    inline void BaseShader::uniform(const std::string &name, const glm::ivec4 &value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform4i(ID(), location, value[0], value[1], value[2], value[3]));
    }

    inline void BaseShader::uniform(const std::string &name, const glm::uvec4 &value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniform4ui(ID(), location, value[0], value[1], value[2], value[3]));
    }

    inline void BaseShader::uniform(const std::string &name, const glm::mat4 &value) {
        OGL_CALL(const GLint location = glGetUniformLocation(ID(), name.c_str()));
        OGL_CALL(glProgramUniformMatrix4fv(ID(), location, 1, GL_FALSE, glm::value_ptr(value)));
    }
}