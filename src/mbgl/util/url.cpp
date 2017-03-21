#include <mbgl/util/url.hpp>
#include <mbgl/util/token.hpp>

#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <cstring>

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

URL::URL(const std::string& str)
    : query([&]() -> Segment {
          const auto hashPos = str.find('#');
          const auto queryPos = str.find('?');
          if (queryPos == std::string::npos || hashPos < queryPos) {
              return { hashPos != std::string::npos ? hashPos : str.size(), 0 };
          }
          return { queryPos, (hashPos != std::string::npos ? hashPos : str.size()) - queryPos };
      }()),
      scheme([&]() -> Segment {
          if (str.empty() || !isAlphaCharacter(str.front())) return { 0, 0 };
          size_t schemeEnd = 0;
          while (schemeEnd < query.first && isSchemeCharacter(str[schemeEnd])) ++schemeEnd;
          return { 0, str[schemeEnd] == ':' ? schemeEnd : 0 };
      }()),
      domain([&]() -> Segment {
          auto domainPos = scheme.first + scheme.second;
          while (domainPos < query.first && (str[domainPos] == ':' || str[domainPos] == '/')) {
              ++domainPos;
          }
          const bool isData = str.compare(scheme.first, scheme.second, "data") == 0;
          const auto endPos = str.find(isData ? ',' : '/', domainPos);
          return { domainPos, std::min(query.first, endPos) - domainPos };
      }()),
      path([&]() -> Segment {
          auto pathPos = domain.first + domain.second;
          const bool isData = str.compare(scheme.first, scheme.second, "data") == 0;
          if (isData) {
              // Skip comma
              pathPos++;
          }
          return { pathPos, query.first - pathPos };
      }()) {
}

Path::Path(const std::string& str, const size_t pos, const size_t count)
    : directory([&]() -> Segment {
          // Finds the string between pos and the first /, if it exists
          const auto endPos = count == std::string::npos ? str.size() : pos + count;
          const auto slashPos = str.rfind('/', endPos);
          return { pos, slashPos == std::string::npos || slashPos < pos ? 0 : slashPos + 1 - pos };
      }()),
      extension([&]() -> Segment {
          auto dotPos = str.rfind('.', pos + count);
          const auto endPos = count == std::string::npos ? str.size() : pos + count;
          // Count a preceding @2x to the file extension as well.
          const char* factor = "@2x";
          const size_t factorLen = strlen(factor);
          if (dotPos >= factorLen && dotPos < endPos &&
              str.compare(dotPos - factorLen, factorLen, factor) == 0) {
              dotPos -= factorLen;
          }
          if (dotPos == std::string::npos || dotPos < directory.first + directory.second) {
              return { endPos, 0 };
          }
          return { dotPos, endPos - dotPos };
      }()),
      filename([&]() -> Segment {
          const auto filePos = directory.first + directory.second;
          return { filePos, extension.first - filePos };
      }()) {
}

std::string transformURL(const std::string& tpl, const std::string& str, const URL& url) {
    auto result = util::replaceTokens(tpl, [&](const std::string& token) -> std::string {
        if (token == "path") {
            return str.substr(url.path.first, url.path.second);
        } else if (token == "domain") {
            return str.substr(url.domain.first, url.domain.second);
        } else if (token == "scheme") {
            return str.substr(url.scheme.first, url.scheme.second);
        } else if (token == "directory") {
            const Path path(str, url.path.first, url.path.second);
            return str.substr(path.directory.first, path.directory.second);
        } else if (token == "filename") {
            const Path path(str, url.path.first, url.path.second);
            return str.substr(path.filename.first, path.filename.second);
        } else if (token == "extension") {
            const Path path(str, url.path.first, url.path.second);
            return str.substr(path.extension.first, path.extension.second);
        }
        return "";
    });

    // Append the query string if it exists.
    if (url.query.second > 1) {
        const auto amp = result.find('?') != std::string::npos ? result.size() : std::string::npos;
        result.append(str, url.query.first, url.query.second);
        // Transform the question mark to an ampersand if we had a query string previously.
        if (amp < result.size()) {
            result[amp] = '&';
        }
    }
    return result;
}

} // namespace util
} // namespace mbgl
