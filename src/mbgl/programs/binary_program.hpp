#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/util/optional.hpp>

#include <string>
#include <vector>

namespace mbgl {

class BinaryProgram {
public:
    // Initialize a BinaryProgram object from a serialized represenation.
    BinaryProgram(std::string&& data);

    BinaryProgram(gl::BinaryProgramFormat,
                  std::string&& binaryCode,
                  std::string binaryIdentifier,
                  std::vector<std::pair<const std::string, gl::AttributeLocation>>&&,
                  std::vector<std::pair<const std::string, gl::UniformLocation>>&&);

    std::string serialize() const;

    gl::BinaryProgramFormat format() const {
        return binaryFormat;
    }
    const std::string& code() const {
        return binaryCode;
    }
    const std::string& identifier() const {
        return binaryIdentifier;
    }

    optional<gl::AttributeLocation> attributeLocation(const std::string& name) const;
    gl::UniformLocation uniformLocation(const std::string& name) const;

private:
    gl::BinaryProgramFormat binaryFormat = 0;
    std::string binaryCode;
    std::string binaryIdentifier;
    std::vector<std::pair<const std::string, gl::AttributeLocation>> attributes;
    std::vector<std::pair<const std::string, gl::UniformLocation>> uniforms;
};

} // namespace mbgl
