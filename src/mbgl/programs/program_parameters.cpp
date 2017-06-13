#include <mbgl/programs/program_parameters.hpp>

#include <iomanip>
#include <sstream>

namespace mbgl {

ProgramParameters::ProgramParameters(const float pixelRatio,
                                     const bool overdraw,
                                     std::string cacheDir_)
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
      hash(std::hash<std::string>()(defines)),
      cacheDir(std::move(cacheDir_)) {
}

std::string ProgramParameters::cachePath(const char* name) const {
    std::ostringstream ss;
    ss << cacheDir << "/com.mapbox.gl.shader." << name << "." << std::setfill('0')
       << std::setw(sizeof(size_t) * 2) << std::hex << hash << ".pbf";
    return ss.str();
}

} // namespace mbgl
