#pragma once

#include <glm/glm.hpp>
#include <bwgl/bwgl.hpp>

namespace pbf {
    /// @brief //todo add brief description to BaseShader
    /// @author Benjamin Wiberg
    class BaseShader : public bwgl::Shader {
    public:
        BaseShader(const std::unordered_map<GLuint, std::string> &stages);

        /////////////////////////////////////////////////////////////////
        /// Calls to glUniform wrapped in overloaded member functions ///
        /////////////////////////////////////////////////////////////////

        void uniform(const std::string &name, const float value);
        void uniform(const std::string &name, const float *value, unsigned int count);
        void uniform(const std::string &name, const int value);
        void uniform(const std::string &name, const int *value, unsigned int count);
        void uniform(const std::string &name, const unsigned int value);
        void uniform(const std::string &name, const unsigned int *value, unsigned int count);
        void uniform(const std::string &name, const glm::vec2 &value);
        void uniform(const std::string &name, const glm::ivec2 &value);
        void uniform(const std::string &name, const glm::uvec2 &value);
        void uniform(const std::string &name, const glm::vec3 &value);
        void uniform(const std::string &name, const glm::ivec3 &value);
        void uniform(const std::string &name, const glm::uvec3 &value);
        void uniform(const std::string &name, const glm::vec4 &value);
        void uniform(const std::string &name, const glm::ivec4 &value);
        void uniform(const std::string &name, const glm::uvec4 &value);

        void uniform(const std::string &name, const glm::mat4 &value);

    };
}

#include "BaseShader.inl"