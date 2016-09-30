#pragma once

#include <mbgl/shader/shader.hpp>

#include <array>

namespace mbgl {

template <typename T>
class Uniform {
public:
    Uniform(const char* name, const Shader& shader)
        : current(), location(shader.getUniformLocation(name)) {
    }

    void operator=(const T& t) {
        if (current != t) {
            current = t;
            bind(t);
        }
    }

private:
    void bind(const T&);

    T current;
    gl::UniformLocation location;
};

template <size_t C, size_t R = C>
class UniformMatrix {
public:
    typedef std::array<float, C*R> T;

    UniformMatrix(const char* name, const Shader& shader)
        : current(), location(shader.getUniformLocation(name)) {
    }

    void operator=(const std::array<double, C*R>& t) {
        bool dirty = false;
        for (unsigned int i = 0; i < C*R; i++) {
            if (current[i] != t[i]) {
                current[i] = t[i];
                dirty = true;
            }
        }
        if (dirty) {
            bind(current);
        }
    }

private:
    void bind(const T&);

    T current;
    gl::UniformLocation location;
};

} // namespace mbgl
