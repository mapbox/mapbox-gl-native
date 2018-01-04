#include <mbgl/programs/binary_program.hpp>

#include <protozero/pbf_reader.hpp>
#include <protozero/pbf_writer.hpp>
#include <utility>
#include <stdexcept>

template <class Binding>
static std::pair<const std::string, Binding> parseBinding(protozero::pbf_reader&& pbf) {
    bool hasName = false, hasValue = false;
    std::pair<std::string, Binding> binding;
    while (pbf.next()) {
        switch (pbf.tag()) {
        case 1: // name
            binding.first = pbf.get_string();
            hasName = true;
            break;
        case 2: // value
            binding.second = pbf.get_uint32();
            hasValue = true;
            break;
        default:
            pbf.skip();
            break;
        }
    }
    if (!hasName || !hasValue) {
        throw std::runtime_error("BinaryProgram binding is missing required fields");
    }
    return binding;
}

namespace mbgl {

BinaryProgram::BinaryProgram(std::string&& data) {
    bool hasFormat = false, hasCode = false;
    protozero::pbf_reader pbf(data);
    while (pbf.next()) {
        switch (pbf.tag()) {
        case 1: // format
            binaryFormat = pbf.get_uint32();
            hasFormat = true;
            break;
        case 2: // code
            binaryCode = pbf.get_bytes();
            hasCode = true;
            break;
        case 3: // variable
            attributes.emplace_back(parseBinding<gl::AttributeLocation>(pbf.get_message()));
            break;
        case 4: // uniform
            uniforms.emplace_back(parseBinding<gl::UniformLocation>(pbf.get_message()));
            break;
        case 5: // identifier
        default:
            binaryIdentifier = pbf.get_string();
            break;
        }
    }

    if (!hasFormat || !hasCode) {
        throw std::runtime_error("BinaryProgram is missing required fields");
    }
}

BinaryProgram::BinaryProgram(
    gl::BinaryProgramFormat binaryFormat_,
    std::string&& binaryCode_,
    std::string binaryIdentifier_,
    std::vector<std::pair<const std::string, gl::AttributeLocation>>&& attributes_,
    std::vector<std::pair<const std::string, gl::UniformLocation>>&& uniforms_)
    : binaryFormat(binaryFormat_),
      binaryCode(std::move(binaryCode_)),
      binaryIdentifier(std::move(binaryIdentifier_)),
      attributes(std::move(attributes_)),
      uniforms(std::move(uniforms_)) {
}

std::string BinaryProgram::serialize() const {
    std::string data;
    data.reserve(32 + binaryCode.size() + uniforms.size() * 32 + attributes.size() * 32);
    protozero::pbf_writer pbf(data);
    pbf.add_uint32(1 /* format */, binaryFormat);
    pbf.add_bytes(2 /* code */, binaryCode.data(), binaryCode.size());
    for (const auto& binding : attributes) {
        protozero::pbf_writer pbf_binding(pbf, 3 /* attribute */);
        pbf_binding.add_string(1 /* name */, binding.first);
        pbf_binding.add_uint32(2 /* value */, binding.second);
    }
    for (const auto& binding : uniforms) {
        protozero::pbf_writer pbf_binding(pbf, 4 /* uniform */);
        pbf_binding.add_string(1 /* name */, binding.first);
        pbf_binding.add_uint32(2 /* value */, binding.second);
    }
    if (!binaryIdentifier.empty()) {
        pbf.add_string(5 /* identifier */, binaryIdentifier);
    }
    return data;
}

optional<gl::AttributeLocation> BinaryProgram::attributeLocation(const std::string& name) const {
    for (const auto& pair : attributes) {
        if (pair.first == name) {
            return pair.second;
        }
    }
    return {};
}

gl::UniformLocation BinaryProgram::uniformLocation(const std::string& name) const {
    for (const auto& pair : uniforms) {
        if (pair.first == name) {
            return pair.second;
        }
    }
    return -1;
}

} // namespace mbgl
