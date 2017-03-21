#pragma once

#include <string>

namespace mbgl {

namespace util {

std::string percentEncode(const std::string&);
std::string percentDecode(const std::string&);

// Class that holds position + lenth pairs for scheme, domain, path + query string of a URL.
class URL {
public:
    using Segment = std::pair<size_t, size_t>; // position, length

    const Segment query;
    const Segment scheme;
    const Segment domain;
    const Segment path;

    URL(const std::string&);
};

// Class that holds position + lenth pairs for directory, extension, and filename of a path.
// The extension will contain the preceding ., and optionally a preceding @2x specifier.
// The filename will not contain the file extension.
class Path {
public:
    using Segment = std::pair<size_t, size_t>; // position, length

    const Segment directory;
    const Segment extension;
    const Segment filename;

    Path(const std::string&, size_t pos = 0, size_t count = std::string::npos);
};

// Parses the given URL and replaces the tokens in template with parts of the URL.
// When parsing "http://example.com/foo/bar/baz.png", valid tokens are:
// * {scheme} == "http"
// * {domain} == "example.com"
// * {path} == "foo/bar/baz.png"
// * {directory} == "foo/bar/"
// * {filename} == "baz"
// * {extension} == ".png"
// The query string of the source URL will always be appended.
std::string transformURL(const std::string& tpl, const std::string& url, const URL&);
inline std::string transformURL(const std::string& tpl, const std::string& url) {
    return transformURL(tpl, url, URL(url));
}

} // namespace util
} // namespace mbgl
