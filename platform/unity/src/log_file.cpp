#include <mbgl/platform/log.hpp>
#include <mbgl/util/enum.hpp>

#include <sstream>
#include <string>

#include <stdio.h>

namespace mbgl {

void Log::platformRecord(EventSeverity severity, const std::string &msg) {
    std::stringstream logStream;

    logStream << "[" << Enum<EventSeverity>::toString(severity) << "] " << msg << std::endl;

    FILE* file = fopen("plugin.log", "a");
    if (file) {
        fprintf(file, "%s", logStream.str().c_str());
        fclose(file);
    }
}

} // namespace mbgl
