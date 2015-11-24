#ifndef MBGL_UTIL_TOKEN
#define MBGL_UTIL_TOKEN

#include <map>
#include <string>
#include <algorithm>

namespace mbgl {
namespace util {

const static std::string tokenReservedChars = "{}()[]<>$=:;.,^";

// Replaces {tokens} in a string by calling the lookup function.
template <typename Lookup>
std::string replaceTokens(const std::string &source, const Lookup &lookup) {
    std::string result;
    result.reserve(source.size());

    auto pos = source.begin();
    const auto end = source.end();

    while (pos != end) {
        auto brace = std::find(pos, end, '{');
        result.append(pos, brace);
        pos = brace;
        if (pos != end) {
            for (brace++; brace != end && tokenReservedChars.find(*brace) == std::string::npos; brace++);
            if (brace != end && *brace == '}') {
                result.append(lookup({ pos + 1, brace }));
                pos = brace + 1;
            } else if (brace != end && *brace == ':') {
                std::string repl = lookup({ pos + 1, brace });
                result.append(repl);
                pos = brace + 1;
                int depth = 1;
                for (brace++; brace != end && depth > 0; brace++) {
                    if (*brace == '{') depth++;
                    else if (*brace == '}') depth--;
                }
                if (repl.empty()) {
                    repl = replaceTokens({ pos, brace - 1 }, lookup);
                    result.append(repl);
                }
                pos = brace;
            } else {
                result.append(pos, brace);
                pos = brace;
            }
        }
    }

    return result;
}

} // end namespace util
} // end namespace mbgl

#endif
