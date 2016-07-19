#include <mbgl/platform/log.hpp>
#include <mbgl/style/parser.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/tile/vector_tile_data.hpp>

#include <mbgl/style/rapidjson_conversion.hpp>

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/filter.hpp>

#include <protozero/exception.hpp>

#include <iostream>
#include <istream>
#include <iterator>
#include <ostream>
#include <string>
#include <unordered_map>

typedef void (*Callback)(std::string);
static const std::unordered_map<std::string, Callback> fuzzers{
    { "source-url",
      [](std::string input) { mbgl::util::mapbox::normalizeSourceURL(input, "key"); } },
    { "style-url", [](std::string input) { mbgl::util::mapbox::normalizeStyleURL(input, "key"); } },
    { "sprite-url",
      [](std::string input) { mbgl::util::mapbox::normalizeSpriteURL(input, "key"); } },
    { "glyphs-url",
      [](std::string input) { mbgl::util::mapbox::normalizeGlyphsURL(input, "key"); } },
    { "tile-url", [](std::string input) { mbgl::util::mapbox::normalizeTileURL(input, "key"); } },
    { "canonicalize-tile-url",
      [](std::string input) {
          mbgl::util::mapbox::canonicalizeTileURL(input, mbgl::SourceType::Raster, 42);
      } },
    { "parse-style",
      [](std::string input) {
          mbgl::style::Parser parser;
          parser.parse(input);
      } },
    { "parse-filter",
      [](std::string input) {
          using namespace mbgl;
          using namespace mbgl::style;
          rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> document;
          document.Parse<0>(input.c_str());
          if (!document.HasParseError()) {
              conversion::convert<Filter>(document);
          }
      } },
    { "parse-tile",
      [](std::string input) {
          mbgl::VectorTileData tile(std::make_shared<const std::string>(std::move(input)));
          try {
              tile.getLayer("foo");
          } catch (protozero::exception& ex) {
          }
      } },
};

void printValidFuzzers() {
    std::cerr << "Valid fuzzers are:" << std::endl;
    for (auto& fuzzer : fuzzers) {
        std::cerr << "  - " << fuzzer.first << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: fuzz <fuzzers>" << std::endl;
        printValidFuzzers();
        return 1;
    }

    auto it = fuzzers.find(argv[1]);
    if (it == fuzzers.end()) {
        std::cerr << "Fuzzer '" << argv[1] << "' does not exist." << std::endl;
        printValidFuzzers();
        return 1;
    }

    // Disables logging
    mbgl::Log::setObserver(std::make_unique<mbgl::Log::NullObserver>());

    while (__AFL_LOOP(1000)) {
        // Pass stdin to the function.
        std::cin >> std::noskipws;
        it->second({ std::istream_iterator<char>(std::cin), std::istream_iterator<char>() });
    }

    return 0;
}
