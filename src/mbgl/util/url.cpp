#include <mbgl/util/url.hpp>

#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>
#include <algorithm>


namespace {

// std::alnum etc. suffer from locale-dependence.

inline bool isAlphaCharacter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline bool isAlphaNumericCharacter(char c) {
    return isAlphaCharacter(c) || (c >= '0' && c <= '9');
}

inline bool isSchemeCharacter(char c) {
    return isAlphaNumericCharacter(c) || c == '-' || c == '+' || c == '.';
}

} // namespace

namespace mbgl {
namespace util {

std::string percentEncode(const std::string& input) {
    std::ostringstream encoded;

    encoded.fill('0');
    encoded << std::hex;

    for (auto c : input) {
        if (isAlphaNumericCharacter(c) || c == '-' || c == '_' || c == '.' || c == '~') {
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

// Checks whether the input string contains ://, and the part before it is all alphanumeric ASCII.
bool isURL(const std::string& input) {
    auto it = input.begin();
    // First character has to be alphabetic
    if (it == input.end() || !isAlphaCharacter(*it++)) return false;
    // The remaining characters of the scheme can be alphanumeric, or be one of +.-
    while (it != input.end() && isSchemeCharacter(*it)) ++it;
    // Check that :// follows
    return (it != input.end() && *it++ == ':') &&
           (it != input.end() && *it++ == '/') &&
           (it != input.end() && *it++ == '/');
}

} // namespace util
} // namespace mbgl
