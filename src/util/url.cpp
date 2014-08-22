#include <mbgl/util/url.hpp>

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>

namespace mbgl {
namespace util {

std::string percentEncode(const std::string& input) {
    std::ostringstream encoded;

    encoded.fill('0');
    encoded << std::hex;

    for (auto c : input) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        } else {
            encoded << '%' << std::setw(2) << int(c);
        }
    }

    return encoded.str();
}

}
}
