#include <mbgl/programs/program_parameters.hpp>

#include <iomanip>
#include <sstream>

namespace mbgl {

ProgramParameters::ProgramParameters(const float pixelRatio,
                                     const bool overdraw,
                                     optional<std::string> cacheDir_)
    : defines([&] {
          std::ostringstream ss;
          ss.imbue(std::locale("C"));
          ss.setf(std::ios_base::showpoint);
          ss << "#define DEVICE_PIXEL_RATIO " << pixelRatio << std::endl;
          if (overdraw) {
              ss << "#define OVERDRAW_INSPECTOR" << std::endl;
          }
          return ss.str();
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
        std::ostringstream ss;
        ss << *cacheDir << "/com.mapbox.gl.shader." << name << "." << std::setfill('0')
           << std::setw(sizeof(size_t) * 2) << std::hex << std::hash<std::string>()(defines) << ".pbf";
        return ss.str();
    }
}

ProgramParameters ProgramParameters::withAdditionalDefines(const std::vector<std::string>& additionalDefines) const {
    ProgramParameters result(*this);
    for (const auto& define : additionalDefines) {
        result.defines += define;
        result.defines += "\n";
    }
    return result;
}

} // namespace mbgl
