#include <mbgl/shaders/shaders.hpp>
#include <mbgl/shaders/preludes.hpp>
#include <mbgl/programs/program_parameters.hpp>

#include <cassert>
#include <sstream>
#include <iomanip>

namespace mbgl {
namespace shaders {

static std::string pixelRatioDefine(const ProgramParameters& parameters) {
    std::ostringstream pixelRatioSS;
    pixelRatioSS.imbue(std::locale("C"));
    pixelRatioSS.setf(std::ios_base::showpoint);
    pixelRatioSS << parameters.pixelRatio;
    return std::string("#define DEVICE_PIXEL_RATIO ") + pixelRatioSS.str() + "\n";
}

std::string fragmentSource(const ProgramParameters& parameters, const char* fragmentSource) {
    std::string source = pixelRatioDefine(parameters) + fragmentPrelude + fragmentSource;
    if (parameters.overdraw) {
        assert(source.find("#ifdef OVERDRAW_INSPECTOR") != std::string::npos);
        source.replace(source.find_first_of('\n'), 1, "\n#define OVERDRAW_INSPECTOR\n");
    }
    return source;
}

std::string vertexSource(const ProgramParameters& parameters, const char* vertexSource) {
    return pixelRatioDefine(parameters) + vertexPrelude + vertexSource;
}

std::string programCachePath(const ProgramParameters& parameters, const char* name) {
    return parameters.cacheDir + "/com.mapbox.gl.shader." + name +
           (parameters.overdraw ? ".overdraw.pbf" : ".pbf");
}

std::string programIdentifier(const std::string& vertexSource, const std::string& fragmentSource) {
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(sizeof(size_t) * 2) << std::hex;
    ss << std::hash<std::string>()(vertexSource);
    ss << std::hash<std::string>()(fragmentSource);
    return ss.str();
}

} // namespace shaders
} // namespace mbgl
