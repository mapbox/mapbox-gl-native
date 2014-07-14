#ifndef LLMR_STYLE_STYLE_SOURCE
#define LLMR_STYLE_STYLE_SOURCE

#include <llmr/style/types.hpp>

#include <memory>

namespace llmr {

class Source;

struct StyleSource {
    const SourceType type;
    const std::string url;
    const uint32_t tile_size;
    const int32_t min_zoom;
    const int32_t max_zoom;

    bool enabled = false;
    std::shared_ptr<Source> source;

    StyleSource(SourceType type = SourceType::Vector,
              const std::string &url = "",
              uint32_t tile_size = 512, uint32_t min_zoom = 0,
              uint32_t max_zoom = 22)
      : type(type),
        url(url),
        tile_size(tile_size),
        min_zoom(min_zoom),
        max_zoom(max_zoom) {}
  };
};

#endif
