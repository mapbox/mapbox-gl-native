#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {

ProgramParameters::ProgramParameters(const float pixelRatio,
                                     const bool overdraw,
                                     optional<std::string> cacheDir_)
    : defines([&] {
          std::string result;
          result.reserve(32);
          result += "#define DEVICE_PIXEL_RATIO ";
          result += util::toString(pixelRatio, true);
          result += '\n';
          if (overdraw) {
              result += "#define OVERDRAW_INSPECTOR\n";
          }
          return result;
      }()),
      cacheDir(std::move(cacheDir_)) {
}

const std::string& ProgramParameters::getDefines() const {
    return defines;
}

optional<std::string> ProgramParameters::cachePath(const char* name) const {
    if (!cacheDir) {
        return {};
    } else {
        std::string result;
        result.reserve(cacheDir->length() + 64);
        result += *cacheDir;
        result += "/com.mapbox.gl.shader.";
        result += name;
        result += '.';
        result += util::toHex(static_cast<uint64_t>(std::hash<std::string>()(defines)));
        result += ".pbf";
        return result;
    }
}

} // namespace mbgl
