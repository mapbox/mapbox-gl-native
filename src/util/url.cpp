#include <mbgl/util/url.hpp>

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>
#include <algorithm>

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

std::string percentDecode(const std::string& input) {
    std::string decoded;

    auto it = input.begin();
    const auto end = input.end();
    char hex[3] = "00";

    while (it != end) {
        auto cur = std::find(it, end, '%');
        decoded.append(it, cur);
        it = cur;
        if (cur != end) {
            it += input.copy(hex, 2, cur - input.begin() + 1) + 1;
            decoded += static_cast<char>(std::strtoul(hex, nullptr, 16));
        }
    }

    return decoded;
}

}
}
