#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {

ProgramParameters::ProgramParameters(const float pixelRatio,
                                     const bool overdraw)
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
      }()) {
}

const std::string& ProgramParameters::getDefines() const {
    return defines;
}

} // namespace mbgl
