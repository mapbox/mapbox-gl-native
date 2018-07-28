#include <mbgl/util/http_header.hpp>

#include <mbgl/util/string.hpp>

#include <regex>

namespace mbgl {
namespace http {

CacheControl CacheControl::parse(const std::string& value) {
    std::regex maxAgeRegex(R"((?:[^"]*"[^"]*[^\\]")*[^"]*max-age[\s]*=[\s]*([\d]+).*)");
    std::smatch maxAgeMatch;

    CacheControl result;
    result.mustRevalidate = value.find("must-revalidate") != std::string::npos;
    if (std::regex_match(value, maxAgeMatch, maxAgeRegex) && maxAgeMatch.size() == 2) {
        result.maxAge = ::atoll(maxAgeMatch[1].str().c_str());
    }
    return result;
}

optional<Timestamp> CacheControl::toTimePoint() const {
    return maxAge ? util::now() + Seconds(*maxAge) : optional<Timestamp>{};
}

optional<Timestamp> parseRetryHeaders(const optional<std::string>& retryAfter,
                                      const optional<std::string>& xRateLimitReset) {
    if (retryAfter) {
        try {
            auto secs = std::chrono::seconds(std::stoi(*retryAfter));
            return std::chrono::time_point_cast<Seconds>(std::chrono::system_clock::now() + secs);
        } catch (...) {
            return util::parseTimestamp((*retryAfter).c_str());
        }
    } else if (xRateLimitReset) {
        try {
            return util::parseTimestamp(std::stoi(*xRateLimitReset));
        } catch (...) {
            return {};
        }
    }

    return {};
}

} // namespace http
} // namespace mbgl
